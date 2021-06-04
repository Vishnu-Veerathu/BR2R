This upload is meant to provide a way to test the BR2R ranking algorithm against other popular ranking algorithms.

The cpp_code directory contains 3 OptSpace based ranking algorithms. The framework randomly generates data and tests the algorithms upon it. Each run produces a data file and a results file. The data file can be used as input to the python codes in the python/ directory to maintain consistency across experiments. Multiple languages were used to simplify the implementation as we only found OptSpace implementations in C and MATLAB, and the Rank Centrality algorithm is easily implemented in Python.

The blade_chest directory is taken from https://github.com/csinpi/blade_chest and modifications have been made to it for the purpose of running ranking algorithms after using the blade chest model for completion of tournaments.

Individual files/folders have additional data about their purpose and usage.
