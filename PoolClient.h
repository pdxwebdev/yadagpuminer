#ifndef POOLCLIENT_H
#define POOLCLIENT_H

#include "SimpleHttpClient.h"
#include "uint256.h"
#include <string>

struct Work {
	std::string header;
	uint64_t target_up64;
	uint64_t special_target_up64;
	bool special_min;
	bool active;
	bool special_active;
};

struct WorkResult {
	uint256 hash;
	uint64_t nonce;
	bool found;
	uint64_t hash_up64;
    std::string address;
	bool share_found;
};


class PoolClient
{
    public:
        std::string poolAddress;

        PoolClient(std::string poolAddress);
        Work getWork();
        void sendResult(const WorkResult& result);

};

#endif