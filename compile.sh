#!/bin/bash

nvcc -O3 -arch=sm_35  -o yadagpuminer yadagpuminer.cpp MiningStat.cpp SimpleHttpClient.cpp PoolClient.cpp ParametersReader.cpp log.c cuda_sha256d.cu -lcurl -lgmpxx -lgmp 



