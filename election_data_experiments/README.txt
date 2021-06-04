This directory contains the code and data used to conduct experiments on election data found here (https://www.preflib.org/data/packs/tog.zip). This data was used to verify whether the block rank 2 model appears in real datasets.

Usage: ./run_all.sh in the code/election_data_test/ directory. An output of the format

Total = x
y z

will appear. x represents the total number datasets which are present after certain filters are applied in br2finder.cpp. y represents the number of datasets out of the 'x' which have a total order. z represents the number which follow the block rank 2 model.