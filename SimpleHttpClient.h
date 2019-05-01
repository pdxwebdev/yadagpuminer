#ifndef SIMPLE_HTTP_CLIENT_H
#define SIMPLE_HTTP_CLIENT_H
#include <string>

const int SimpleHttpClientTimeout = 60;

struct MemoryStruct {
  char *memory;
  size_t size;
  
  MemoryStruct()
  {
	memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */ 
	size = 0;    /* no data at this point */   
  }
  
  ~MemoryStruct()
  {
  	free(memory);
  }
  
};


class SimpleHttpClient
{
    public:
        static std::string MakeGetRequest(std::string url);
        static std::string MakePostRequest(std::string url, const std::string& body);

    private:
        static void curl_http_get(const char* url, const MemoryStruct& chunk);
        static void curl_http_post(const char* url, const std::string& json_data, const MemoryStruct& chunk);
        static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
};

#endif