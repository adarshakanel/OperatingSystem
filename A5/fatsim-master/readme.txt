
--to compile

	$ make

-- tests 

for i in {0,1,2,3,4,5,6}; do ./fatsim < fatsim$i.txt; done

Result expected

blocks in largest file: 5
blocks not in any file: 10
elapsed time:           0.000
blocks in largest file: 0
blocks not in any file: 51
elapsed time:           0.000
blocks in largest file: 0
blocks not in any file: 102
elapsed time:           0.000
blocks in largest file: 12
blocks not in any file: 25
elapsed time:           0.000
blocks in largest file: 3272
blocks not in any file: 19338
elapsed time:           0.018
blocks in largest file: 12778
blocks not in any file: 167400
elapsed time:           0.335
blocks in largest file: 24047
blocks not in any file: 349902
elapsed time:           0.935


 for i in {1,2,3,4,5,6,7}; do ./fatsim < test$i.txt; done

Result expected 

blocks in largest file: 5
blocks not in any file: 10
elapsed time:           0.000
blocks in largest file: 9
blocks not in any file: 0
elapsed time:           0.000
blocks in largest file: 0
blocks not in any file: 1000000
elapsed time:           0.011
blocks in largest file: 17
blocks not in any file: 27
elapsed time:           0.000
blocks in largest file: 21
blocks not in any file: 60
elapsed time:           0.000
blocks in largest file: 272
blocks not in any file: 4928
elapsed time:           0.000
blocks in largest file: 26499
blocks not in any file: 196674
elapsed time:           1.061
