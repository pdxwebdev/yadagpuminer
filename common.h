#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <stdio.h>

struct MinerParams {
	int batch_size;
	int block_size;		
	std::string address;
	int share_difficulty;
    std::string pool_url;
    int gpu_devices;    
};

#endif