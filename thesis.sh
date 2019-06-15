#!/bin/bash
g++-8 -O -o lock.out LockStack.cpp
for i in 1 2 3 4 5 6 7 8 9 10 20 30 40 50 100 
do 
for (( j=1; j<11; j++ ))
do
./lock.out $i >> lock_data.csv
done
done

g++-8 -O -o opt.out OptStack.cpp
for i in 1 2 3 4 5 6 7 8 9 10 20 30 40 50 100
do
for (( j=1; j<11; j++ ))
do
./opt.out $i >> opt_data.csv
done
done

g++-8 -O -o hp.out -I/usr/local/include/ -L/usr/local/lib/ -lcds HPStack.cpp
for i in 1 2 3 4 5 6 7 8 9 10 20 30 40 50 100
do
for (( j=1; j<11; j++ ))
do
./hp.out $i >> hp_data.csv
done
done

g++-8 -O -mrtm -o rtm.out RTMStack.cpp
for i in 1 2 3 4 5 6 7 8 9 10 20 30 40 50 100
do
for (( j=1; j<11; j++ ))
do
./rtm.out $i >> rtm_data.csv
done
done

g++-8 -O -o lock_treap.out LockTreap.cpp
for i in 1 2 3 4 5 6 7 8 9 10 20 30 40 50 100
do
for (( j=1; j<11; j++ ))
do
./lock_treap.out $i >> lock_treap.csv
done
done

g++-8 -O -mrtm -o rtm_treap.out RTMTreap.cpp
for i in 1 2 3 4 5 6 7 8 9 10 20 30 40 50 100
do
for (( j=1; j<11; j++ ))
do
./rtm_treap.out $i >> rtm_treap.csv
done
done

exit 0
