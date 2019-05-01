#ifndef PARAMETERS_READER_H
#define PARAMETERS_READER_H
#include "common.h"

class ParametersReader
{   
    public: 
        static bool HandleParams(int argc, char *argv[], MinerParams& params);
    private:
        static bool ValidateParams(MinerParams params);
        static MinerParams ReadParams(int argc, char *argv[]);
        static void DisplayUsage();

};

#endif