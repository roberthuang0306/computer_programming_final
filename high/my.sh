do#!/bin/bash
make
for i in {1..100};
do
	Echo //---------//;
	Echo // number$i //;	
	Echo //---------//;
        ./fraig -f do
done
Say 好了啦