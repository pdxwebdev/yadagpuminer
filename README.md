# yadagpuminer

Experimental Yadacoin cuda / gpu miner

[GPL V3 License](LICENSE.txt)

Uses some important code from:
- https://github.com/tpruvot/ccminer 
- https://github.com/rxi/log.c



## Compilation

1. Install Ubuntu 18.04 LTS (http://releases.ubuntu.com/18.04/)

2. Install nvidia drivers and CUDA toolkit
```
sudo apt update
sudo apt install nvidia-driver-390
sudo apt install nvidia-cuda-toolkit
sudo reboot
```

3. Check if its correctly installed
```
nvcc --version
nvidia-smi
```

3. Install dependencies
```
sudo apt install libcurl4-openssl-dev
sudo apt install rapidjson-dev
sudo apt install libgmp-dev
sudo apt install g++-5
```

4. Clone and compile
```
git clone https://github.com/marcinot/yadagpuminer.git
cd yadagpuminer
./compile.sh
```

5. Test
```
./yadagpuminer --address=benchmark
```

## Use

### Parameters:
```
-a, --address             Wallet address (or "benchmark")
-b, --batch-size          Batch size (default 100000000)
-w, --block-size          Block size (default 64)
-p, --pool-url            Pool url (optional if address is "benchmark")
-d, --share-difficulty    Share difficulty (default 1 - find only full blocks)
-g, --gpu-devices         Number of gpu devices to use (default 1)
```

### Mining to your own node/pool (tested)

1. You have to create your own pool (check official instructions https://yadacoin.io/guide ). Make sure your node is on peer list https://yadacoin.io/peers .

2. Start the miner with options for finding only full blocks. Bellow example:

```
./yadagpuminer --address=YourYadacoinWalletAddress --gpu-devices=6 --pool-url=http://11.22.33.44
```

Normally your rewards are collected by pool/node address so YourYadacoinWalletAddress isn't very important in this case.


### Mining to yadacoin.io pool (not tested very well)

1. Start the miner with sending also shares with minimum difficulty 1/64 current target (you can experiment what is the best value).

```
./yadagpuminer --address=YourYadacoinWalletAddress --gpu-devices=6 --pool-url=https://yadacoin.io --share-difficulty=64
```


### Example output

```
20:04:03 INFO  Found: 108/176, Hashrate: 1536.21 MH/s, Current block time 159 s, Time to block: 1124 s
20:04:03 DEBUG 393.01 MH/s, 321.33 MH/s, 208.77 MH/s, 211.28 MH/s, 209.40 MH/s, 192.42 MH/s,
20:04:08 INFO  Found: 108/176, Hashrate: 1535.88 MH/s, Current block time 164 s, Time to block: 1124 s
20:04:08 DEBUG 392.96 MH/s, 321.07 MH/s, 208.36 MH/s, 211.40 MH/s, 209.58 MH/s, 192.52 MH/s,
20:04:13 INFO  Found: 108/176, Hashrate: 1535.48 MH/s, Current block time 170 s, Time to block: 1124 s
20:04:13 DEBUG 393.00 MH/s, 321.12 MH/s, 208.58 MH/s, 210.94 MH/s, 209.76 MH/s, 192.08 MH/s,
20:04:19 INFO  Found: 108/176, Hashrate: 1536.28 MH/s, Current block time 175 s, Time to block: 1124 s
20:04:19 DEBUG 392.94 MH/s, 321.44 MH/s, 208.76 MH/s, 211.05 MH/s, 209.91 MH/s, 192.18 MH/s,
20:04:19 WARN  FOUND BLOCK!
20:04:19 DEBUG nonce       = 113034199090789877
20:04:19 DEBUG hash        = 00000000004852cacb122a3f70f62794d3f76e6e435eed141f6452478cb13bc2
20:04:19 DEBUG target_up64 = 0000000000a30c29
20:04:19 DEBUG { "nonce": 113034199090789877, "hash": "00000000004852cacb122a3f70f62794d3f76e6e435eed141f6452478cb13bc2", "address": "18fgHW55CaSvP2LrQBixmmBc3DANUJqGAr" }
20:04:24 INFO  Found: 108/176, Hashrate: 1530.71 MH/s, Current block time 180 s, Time to block: 1128 s
20:04:24 DEBUG 392.82 MH/s, 321.14 MH/s, 208.91 MH/s, 206.13 MH/s, 209.47 MH/s, 192.24 MH/s,
```

target_up64 is first 64-bits from current target (in hex format) - your goal is to find hash smaller than target.

## Benchmarks / hashrates

- 1070Ti - 393 MH/s
- 1070 - 321 MH/s
- 1060 6GB - 210 MH/s
- 1060 3GB - 192 MH/s
- GTX 750 - 65 MH/s

You can experiment with batch-size and block-size parameters.

## Limitations

- This miner ignores nonce ranges sent by pool and generates custom random nonces. This is neccesary because pool ranges are too small to efficient GPU mining. Normaly it's not a problem.

- This miner ignore special_min flag (some exceptions for reporting) because I don't fully understand how it works. Still you can find some special_min blocks (I found many).

- Currently you can't choose specific GPU cards.

- Currently works only on Linux (I don't have plans to Windows port but you can Fork this code :-)














