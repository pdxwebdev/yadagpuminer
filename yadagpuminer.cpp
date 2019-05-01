#include "common.h"


#include "uint256.h"
#include "sha256.h"

#include <unistd.h>
#include "MiningStat.h"
#include "SimpleHttpClient.h"
#include "PoolClient.h"
#include "log.h"
#include "ParametersReader.h"
#include "cuda_helper.h"


MiningStat miningStat;

MinerParams params;


struct MinerThread {
	pthread_t thread_id;
	int gpu_index;
	MinerParams params;
};

struct best_hash_str {
	uint64_t nonce;
	uint64_t hash_up64;
	uint8_t hash[32];
};

//int n_threads;

MinerThread threads[8];

Work newWork;

int found_count = 0;
int found_shares_count = 0;
 
PoolClient* poolClient = NULL;

void sha256d_setBlock_80(const void *data, int data_len);
void sha256d_hash_80(uint32_t threads, uint64_t startNonce, uint64_t* best_nonce, uint64_t* best_value, int threadsperblock);


uint64_t get_hash_up64(BYTE hash[32])
{
	uint64_t val = 0;
	
	for(int i=0; i<8; i++)
	{
		val |= ((uint64_t)hash[24+i]) << (i * 8);
	}

	return val;
}

void sha256d(const char* buff, BYTE hash2[32])
{	
	int n = strlen(buff);
	SHA256_CTX ctx1;			
	SHA256_CTX ctx2;
	BYTE hash1[32];

	sha256_init(&ctx1);
	sha256_update(&ctx1, (const BYTE*)buff, n);
	sha256_final(&ctx1, hash1);
	
	sha256_init(&ctx2);
	sha256_update(&ctx2, hash1, 32);
	sha256_final(&ctx2, hash2);		
	
}

best_hash_str find_best_nonce_gpu2(int device, std::string header,  uint64_t batch_size, int block_size)
{	


	best_hash_str best_hash;

	std::string nonce_tag("{nonce}");
	size_t nonce_size = nonce_tag.size();
	size_t nonce_pos = header.find(nonce_tag);
	size_t header_size = header.size();

	if (nonce_pos != std::string::npos)
	{
		std::string left_str = header.substr(0, nonce_pos);
		std::string right_str = header.substr(nonce_pos + nonce_size, header_size - nonce_pos - nonce_size);

		std::string current_header;
		current_header.append(left_str);

		uint64_t nonce_tpl = (100000000+(rand() % 900000000)) * 1000000000UL;
		char nonce_buff[30];
		
		sprintf(nonce_buff, "%lu", nonce_tpl);
		
		current_header.append(std::string(nonce_buff));	
		current_header.append(right_str);
				
		best_hash.hash_up64 = 0xffffffffffffffffUL;
		best_hash.nonce = 0;
		
		sha256d_setBlock_80(current_header.c_str(), current_header.length());
		sha256d_hash_80(batch_size, 0, &(best_hash.nonce), &(best_hash.hash_up64), block_size);


		best_hash.nonce += nonce_tpl;

		BYTE hash[32];
		current_header.clear();
		current_header.append(left_str);
		current_header.append(std::to_string(best_hash.nonce));
		current_header.append(right_str);
		
		sha256d(current_header.c_str(), hash);
		memcpy(best_hash.hash, hash, 32);

	}
			
	return best_hash;
}

WorkResult mine(int batch_size, int block_size, int device, const Work& work, int share_difficulty)
{
	best_hash_str best_hash = find_best_nonce_gpu2(device, work.header, batch_size, block_size);

	WorkResult result;
	result.hash_up64 = best_hash.hash_up64;
	result.hash.from_array32((uint32_t*) best_hash.hash );
	result.nonce = best_hash.nonce;
	result.found = (result.hash_up64 < work.target_up64);
	result.share_found = (result.hash_up64 < (share_difficulty * work.target_up64));
		
	return result;
}



Work getBenchmarkWork()
{
	Work work;
	work.header = "2155415885003fc7a89c59f094569435fe53e5c42daae6092e79e52baefb8ce3d081c58390e56493820000000004cf5d92b70274c17fc26108c3fa755fc7c6138c2245990dfbd20779{nonce}False5619147491189905241504060482922981236721380648793587878958993412092a01bcae5ed18d778d3c80c73b7ac84268751d7617667415b871166386362079";
	work.target_up64 = 1000000000UL;
	work.nonces[0] = 1000;
	work.nonces[1] = 2000;
	work.special_min = false;
	work.active = true;
	return work;
}



void display_found_message(const Work& work, const WorkResult& result)
{
	if (result.found) 
	{
		if (work.special_min)
		{
			log_warn("FOUND SPECIAL MIN BLOCK");			
		}
		else
		{							
			log_warn("FOUND BLOCK!");
		}					
	}
	else
	{
		log_warn("FOUND SHARE!");
	}
	
	log_debug("nonce       = %lu", result.nonce);
	log_debug("hash        = %s", result.hash.GetHex().c_str());
	log_debug("target_up64 = %016lx", work.target_up64);				

}

void *miner_worker( void *ptr )
{
	MinerThread* thread = (MinerThread*) ptr;
	CUDA_SAFE_CALL(cudaSetDevice(thread->gpu_index));
	CUDA_SAFE_CALL(cudaSetDeviceFlags(cudaDeviceScheduleBlockingSync));

	do {

		std::string lastHeader = newWork.header;

		while (newWork.active && (newWork.header == lastHeader)) {

			WorkResult result = mine( 			
			 	thread->params.batch_size, thread->params.block_size, thread->gpu_index, newWork, thread->params.share_difficulty);
			
			miningStat.addHashes(thread->gpu_index, thread->params.batch_size);

			
			if (result.found || result.share_found)
			{				
				display_found_message(newWork, result);

				result.address = thread->params.address;

				if (result.address != "benchmark")
					poolClient->sendResult(result);

				if (!newWork.special_min)
				{
					found_shares_count++;
					if (result.found)						
					{
						found_count++;
						newWork.active = false;
						break;							
					}
				}
								
			}


		}			
	} while(true);
}

void start_miner_threads(MinerParams params)
{
	for(int i=0; i<params.gpu_devices; i++)
	{		
		threads[i].params = params;
		threads[i].gpu_index = i;

		

		int error = pthread_create( &(threads[i].thread_id), NULL, miner_worker, (void*) (&threads[i]));
		if (error)
		{			
			log_fatal ("pthread_create error %d", error);			
			exit(1);
		}
	}
}


void stop_miner_threads()
{
	for(int i=0; i<params.gpu_devices; i++)
		pthread_join(threads[i].thread_id, NULL);
}

void handleStat(uint64_t target_up64)
{
	if ((target_up64 > 1) && (miningStat.getTotalHashPerSec() > 1))
	{
		double timeToBlock = 1.0 / ( miningStat.getTotalHashPerSec() * (target_up64 / 1.8446744e+19) );		
		log_info("Found: %d/%d, Hashrate: %.2f MH/s, Current block time %.0f s, Time to block: %.0f s", 
			found_count,
			found_shares_count,
			miningStat.getTotalHashPerSec() / 1000000,
			miningStat.elapsedTime(), 			
			timeToBlock
			);		
	}
	char buff[256];
	int len = 0;
	for(int i=0; i<params.gpu_devices; i++)
	{

		int res = sprintf(buff + len, "%.2f MH/s, ", miningStat.getDeviceHashPerSec(i) / 1000000);
		if (res > 0)
			len+=res;
	}
	log_debug(buff);

}


int main(int argc, char* argv[])
{
	log_set_level(LOG_DEBUG);
	srand(time(0));

	if (!ParametersReader::HandleParams(argc, argv, params))
		return -1;

	poolClient = new PoolClient(params.pool_url);


	if (params.address == "benchmark")
	{		
		log_warn("Benchmark mode!");
	}

	found_count =0;
	int iter = 0;

	newWork.active = false;

	start_miner_threads(params);

	std::string last_header = "";	
  while(true)
	{
		try
		{
			handleStat(newWork.target_up64);
			//receive work
			if (params.address != "benchmark")
				newWork = poolClient->getWork();							
			else
				newWork = getBenchmarkWork();
			
			//handle stats
			if (newWork.active)
			{					
				if (newWork.header != last_header)
				{
					
					log_warn("New work");
					log_debug("header      = %s", newWork.header.c_str());
					log_debug("target_up64 = %016lx", newWork.target_up64);
					log_debug("special_min = %d", newWork.special_min);

					miningStat.reset();
					last_header = newWork.header;
				}				
			}
			else
			{
				log_debug("Waiting for new work...");
			}
			

			//delay to next getwork
			sleep(5);
					
		}
		catch(const std::exception& e)
		{
			log_error(e.what());			
			sleep(1);
		}
		
	}


	stop_miner_threads();
	delete poolClient;

  return 0;
}