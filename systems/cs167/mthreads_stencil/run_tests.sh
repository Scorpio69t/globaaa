#!/usr/bin/env bash

# run a test numerous times
for i in `seq 1 100`;
do
        ./test  > out
done
