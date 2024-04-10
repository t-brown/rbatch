# rbatch
Client and server to create a MNP hostfile in AWS Batch

## Introduction
When using [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface) to run multi-node parallel jobs on [AWS Batch](https://docs.aws.amazon.com/batch/latest/userguide/multi-node-parallel-jobs.html) one needs to generate a host file.

There are two programs within this repository, `rbatchd` the server
program and `rbatch` the client program. The server program should
be run on the node that will be executing the MPI program (e.g. the node
calling `mpiexec`). While the client program should be executed on every
node that will be participating in the job, including the server node.

## Prerequisites

+  [C compiler](https://gcc.gnu.org/)
+  [make](https://www.gnu.org/software/make/)

## Building / Installation

To install `rbatch` and `rbatchd` with the default options into `/usr/local`:

    make
    make install


