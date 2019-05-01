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


	std::string response = SimpleHttpClient::MakeGetRequest(poolAddress + "/pool");

	Document document;
	document.Parse<kParseNumbersAsStringsFlag>(response.c_str());
	
  work.header = document["header"].GetString();
	work.nonces[0] = atoll(document["nonces"][0].GetString());
	work.nonces[1] = atoll(document["nonces"][1].GetString());

	mpz_class tmp;	
	tmp.set_str(document["target"].GetString(), 10);
	tmp >>= 192;
	work.target_up64 = tmp.get_ui();

	work.special_min = document["special_min"].GetBool();
	work.active = (work.target_up64 < 0xffffffffffffffffUL);

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