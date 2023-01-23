# PHSP

This repository implements a deadlock-free solution to the famous Synchronization problem called the Dining Philosophores.

## Definitons

`num_phsp`: The number of philosophers sitting at the table. It can be maximum 27

`min_think`: The minimum time value that a philosopher thinking takes. Its unit is milliseconds. 

`max_think`: The maximum time value that a philosopher thinking takes. Its unit is milliseconds. 

`min_dine`: The minimum time value that a philosopher dining takes. Its unit is milliseconds. 

`max_dine`: The maximum time value that a philosopher dining takes. Its unit is milliseconds. 

`dst`: The distribution type. It can be either uniform or exponential.

`num`:  The number of repetitions of dining state. Eeach philosopher must dine num times.

### Input Format

    ./phsp <num_phsp> <min_think> <max_think> <min_dine> <max_dine> <dst> <num>

## Execution

Run the following command to compile the C code

    make

Then you can enter your input, e.g.,

    ./phsp 5 500 1000 50 100 exponential 100

