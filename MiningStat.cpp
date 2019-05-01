#include "MiningStat.h"
#include <stdio.h>

MiningStat::MiningStat()
{
    reset();
}

void MiningStat::reset()
{
    for(int i=0; i<MiningStatMaxDevices; i++)
        hashesTab[i] = 0; 

	clock_gettime(CLOCK_MONOTONIC, &start);
}


void MiningStat::addHashes(int device, unsigned long int hashes )
{
    hashesTab[device] += hashes;
}

double MiningStat::elapsedTime()
{    
    timespec finish;

	double elapsed;

	clock_gettime(CLOCK_MONOTONIC, &finish);

    


	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    return elapsed;
}

double MiningStat::getTotalHashPerSec()
{
    double total = 0.0;
    for(int i=0; i<MiningStatMaxDevices; i++)
        total += hashesTab[i];

    return total / elapsedTime();

}

double MiningStat::getDeviceHashPerSec(int device)
{
    return hashesTab[device] / elapsedTime();
}
