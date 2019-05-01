#include "ParametersReader.h"
#include <getopt.h>

bool ParametersReader::HandleParams(int argc, char *argv[], MinerParams& params)
{
    params = ReadParams(argc, argv);
    if (params.address.length() == 0)
    {
        DisplayUsage();
        return false;
    }        
    return ValidateParams(params);    
}

MinerParams ParametersReader::ReadParams(int argc, char *argv[])
{
    MinerParams params;
    params.address = "";
    params.batch_size = 100000000;
    params.block_size = 64;
    params.gpu_devices = 1;
    params.pool_url = "";
    params.share_difficulty = 1;


    int opt= 0;
    int area = -1, perimeter = -1, breadth = -1, length =-1;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"address",      required_argument,       0,  'a' },
        {"batch-size", required_argument,       0,  'b' },
        {"block-size",    required_argument, 0,  'w' },
        {"pool-url",   required_argument, 0,  'p' },
        {"share-difficulty",   required_argument, 0,  'd' },
        {"gpu-devices",   required_argument, 0,  'g' },     

        {0,           0,                 0,  0   }
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"a:b:w:p:d:g:", 
                   long_options, &long_index )) != -1) {

        switch (opt) {
             case 'a' : params.address = std::string(optarg);
                 break;
             case 'b' : params.batch_size = atoi(optarg);
                 break;
             case 'w' : params.block_size = atoi(optarg); 
                 break;
             case 'p' : params.pool_url = std::string(optarg);
                 break;
             case 'd' : params.share_difficulty = atoi(optarg); 
                 break;
             case 'g' : params.gpu_devices = atoi(optarg); 
                 break;                 
        }
    }

    return params;

}

void ParametersReader::DisplayUsage()
{
    printf("Parameters:\n");
    printf("-a, --address             Wallet address (or \"benchmark\")\n");
    printf("-b, --batch-size          Batch size (default 100000000)\n");
    printf("-w, --block-size          Block size (default 64)\n");
    printf("-p, --pool-url            Pool url (optional if address is \"benchmark\")\n");
    printf("-d, --share-difficulty    Share difficulty (default 1 - find only full blocks)\n");
    printf("-g, --gpu-devices         Number of gpu devices to use (default 1)\n");
    printf("\n");
}

bool ParametersReader::ValidateParams(MinerParams params)
{
    bool res = true;

    if ((params.address.length() <= 30) && (params.address != "benchmark"))
    {
        printf("address is too short (minimum 30 characters)\n");
        res = false;
    }
        
    
    if ((params.batch_size < 1000))
    {
        printf("batch-size is too low (minimum 1000)\n");
        res = false;
    }

    if ((params.batch_size > 100000000))
    {
        printf("batch-size is too high (maximum 100000000)\n");
        res = false;
    }

    if ((params.block_size < 8))
    {
        printf("block-size is too low (minimum 8)\n");
        res = false;
    }

    if ((params.block_size > 4096))
    {
        printf("block-size is too high (maximum 4096)\n");
        res = false;
    }


    if (params.address != "benchmark")
    {        
        if ((params.pool_url.find("http") != 0 ))
        {
            printf("invalid pool-url (must start with http)\n");
            res = false;
        }
    }

    if ((params.gpu_devices < 1))
    {
        printf("gpu-devices is too low (minimum 1)\n");
        res = false;
    }

    if ((params.share_difficulty < 1))
    {
        printf("share-difficulty is too low (minimum 1)\n");
        res = false;
    }


    return res;
}


