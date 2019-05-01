#ifndef MINING_STAT_H
#define MINING_STAT_H
#include <time.h>

const int MiningStatMaxDevices = 8;

class MiningStat {
    public:
        MiningStat();
        void reset();        
        void addHashes(int device, unsigned long int hashes );    
        double getTotalHashPerSec();
        double getDeviceHashPerSec(int device);
        double elapsedTime();
    private:
        timespec start;
    
        unsigned long int hashesTab[MiningStatMaxDevices];        
};

#endif