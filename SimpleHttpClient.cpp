#include "SimpleHttpClient.h"
#include <curl/curl.h>
#include <string.h>
#include <exception>
#include <stdexcept>

std::string SimpleHttpClient::MakeGetRequest(std::string url)
{
    MemoryStruct mem;
    curl_http_get(url.c_str(), mem);    
    return std::string(mem.memory);    
}

std::string SimpleHttpClient::MakePostRequest(std::string url, const std::string& body)
{
    MemoryStruct mem;
    curl_http_post(url.c_str(), body.c_str(), mem);    
    return std::string(mem.memory);    
}


size_t SimpleHttpClient::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    throw std::runtime_error ("not enough memory (realloc returned NULL)");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}




void SimpleHttpClient::curl_http_get(const char* url, const MemoryStruct& chunk)
{
	CURL *curl_handle;
	  CURLcode res;
	 		 
	  curl_global_init(CURL_GLOBAL_ALL);
	 
	  /* init the curl session */ 
	  curl_handle = curl_easy_init();
	 
	  /* specify URL to get */ 
	  curl_easy_setopt(curl_handle, CURLOPT_URL, url);

      curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, SimpleHttpClientTimeout);

	  /* send all data to this function  */ 
	  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	 
	  /* we pass our 'chunk' struct to the callback function */ 
	  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	 
	  /* some servers don't like requests that are made without a user-agent
	     field, so we provide one */ 
	  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	 
	  /* get it! */ 
	  res = curl_easy_perform(curl_handle);
	 
	  /* cleanup curl stuff */ 
	  curl_easy_cleanup(curl_handle);
	 	  	 
	  /* we're done with libcurl, so clean it up */ 
	  curl_global_cleanup();	

	  /* check for errors */ 
	  if(res != CURLE_OK) {
          throw std::runtime_error ("Request to " + std::string(url) + " failed: " + std::string( curl_easy_strerror(res)) );
	  }
	 
 
}



void SimpleHttpClient::curl_http_post(const char* url, const std::string& json_data, const MemoryStruct& chunk)
{
  CURL *curl;
  CURLcode res;
  
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
//Succeeded
	  /* send all data to this function  */ 
	  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	 
	  /* we pass our 'chunk' struct to the callback function */ 
	  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	  
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, SimpleHttpClientTimeout);

			struct curl_slist *headers = NULL;
  		headers = curl_slist_append(headers, "Content-Type: application/json");
			headers = curl_slist_append(headers, "Connection: close");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
  
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);

    /* always cleanup */ 
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);


    /* Check for errors */ 
	  /* check for errors */ 
	  if(res != CURLE_OK) {
          throw std::runtime_error ("Request to " + std::string(url) + " failed: " + std::string( curl_easy_strerror(res)) );
	  }
 


  }
}
