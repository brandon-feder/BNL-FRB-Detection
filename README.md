# BNL-FRB-Detection

There are two files: `genTestData.py` and `detect.ipynb` as well as the old code in `old`.

## genTestData.py
This file generates the test data. It takes in any permutation of the following arguments:
* `-d`/`-display` to display the test data as its created. This is defauleted to false.
* `-width <width>`, and `-height <height>` to determine the dimensions of the data. This is defaulted to 500 and 500.
* `-n <some positive integer>` to determine the number of test datas to create. Defaulted to 1

## detect.ipynb
This notebook has two setting variables at the top: `FILE_PATH` and `ANGLE_ACCURACY`. `FILE_PATH` should hold the path to the 
data file eg. `./test-data/data1.json` while `ANGLE_ACCURACY` determines the accuracy that a line is detected width. Note that 
there is a cutoff where higher values of this variable lead to demnisishing returns.
