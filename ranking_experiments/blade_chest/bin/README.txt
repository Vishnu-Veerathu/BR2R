This folder contains the experiments done on real world datasets provided at https://github.com/csinpi/blade_chest. See ../README.md for more information. This folder contains scripts and additional codes in order to run block rank 2 ranking algorithm on the datasets after blade chest is used for completing the tournament.

Usage: 
1) run make in the ../src directory to update BC, the blade chest binary.
2) Execute run_expts.sh. Dataset name and path should be given here.
3) Files of the format {$dataset_name$}_result{$r$}.txt should appear, which contain results of individual runs. $r$ represents the rank in this case. Each row represents an individual run, the 4 columns represent the upsets based on copeland, borda, br2, and br2+copeland ranking methods. br2+copeland basically considers using a copeland ranking instead of br2 ranking in every recursive call if number of upsets with copeland is smaller.
4) Use stats.cpp and give the above generated files as input if aggregated stats are needed. This output also consists of 4 columns similar to the above file, except mean+-standard deviation is the format for each numerical value.