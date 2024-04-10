#!/bin/bash
#

export OMP_NUM_THREADS=2
export AWS_BATCH_JOB_ID=1
export AWS_BATCH_JOB_MAIN_NODE_PRIVATE_IPV4_ADDRESS=127.0.0.1

./rbatchd &
sleep 5
./rbatch


