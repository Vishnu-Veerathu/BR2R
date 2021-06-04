This folder contains the c++ codes used for evaluation of BR2R vs other ranking algorithms which utilize OptSpace.
synth_data_test.cpp contains the code to create synthetic data and evaluate algorithms based on them. Use 'make synthtest' to compile.
Running ./synthtest will result in data files and result files being created. Use the data files as input to rank centrality in order to run both algos on the same data.

real_data_test.cpp contains the code to evaluate real world datasets.
Use 'make realtest' to compile.
./realtest < /path/to/reformatted/blade_chest/data is the usage. Reformatting datafile code is present in ../real_data_extract folder.
