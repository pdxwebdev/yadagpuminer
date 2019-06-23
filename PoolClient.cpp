#include "PoolClient.h"
#include <rapidjson/document.h>
#include <gmpxx.h>
#include <stdexcept>
#include <unistd.h>
#include "log.h"

using namespace rapidjson;

PoolClient::PoolClient(std::string poolAddress)
{
    this->poolAddress = poolAddress;
}


Work PoolClient::getWork()
{
	Work work;
	std::string url = poolAddress + "/pool";


	std::string response = SimpleHttpClient::MakeGetRequest(poolAddress + "/pool");

	Document document;
	document.Parse(response.c_str());
	
  	work.header = document["header"].GetString();

	mpz_class tmp;	
	tmp.set_str(document["target"].GetString(), 16);
	tmp >>= 192;
	work.target_up64 = tmp.get_ui();
	mpz_class tmp2;	
	tmp2.set_str(document["special_target"].GetString(), 16);
	tmp2 >>= 192;
	work.special_target_up64 = tmp2.get_ui();

	work.special_min = document["special_min"].GetBool();
	work.active = (work.target_up64 < 0xffffffffffffffffUL);
	work.special_active = (work.special_target_up64 < 0xffffffffffffffffUL);

	return work;
}

void PoolClient::sendResult(const WorkResult& result)
{
    

	std::string data("{ ");

	data.append("\"nonce\": ");
	data.append(std::to_string(result.nonce));
	data.append(", ");


	data.append("\"hash\": \"");
	data.append(result.hash.GetHex());
	data.append("\", ");


	data.append("\"address\": \"");
	data.append(result.address);
	data.append("\"");

  	data.append(" }");
	
	log_debug(data.c_str());


    int tries = 0;
    while (tries  < 3) 
    {
        tries++;
        try
        {
            std::string response = SimpleHttpClient::MakePostRequest(this->poolAddress + "/pool-submit", data);
        }
        catch(const std::exception& e)
        {
			log_error(e.what());            
            sleep(5);
        }
    } 

	
}