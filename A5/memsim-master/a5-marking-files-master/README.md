# a5-marking-files

Marking files for assignment 5 (Spring 2020)

# Tests for Q1 - Memory simulator

| test                             | weight | Correct<br>page requests | Correct<br>free chuks|
| -------------------------------- |:------:|:------------------------:|:--------------------:|
| ``./memsim3 33 < memsim0.txt``   | 5      | 210|29 |
| ``./memsim3 123 < memsim0.txt``  | 5      | 57|110|
| ``./memsim3 3 < memsim1.txt``    | 2      | 47|1|
| ``./memsim3 35 < memsim1.txt``   | 2      |4|0|
| ``./memsim3 1000 < memsim2.txt`` | 2      |7|7000|
| ``./memsim3 1000 < memsim3.txt`` | 3      |199|1000|
| ``./memsim3 1 < memsim3.txt``    | 2      |198866|995|
| ``./memsim3 1000 < memsim4.txt`` | 2      |1891|436930|
| ``./memsim3 1001 < memsim5.txt`` | 7      |1623945|10251
| Total                        | 30     ||


# Tests for Question 2 - FAT simulator

| test                             | weight | Correct<br>largest file | Correct<br>unused blocks|
| -------------------------------- |:------:|:-----------------------:|:--------------------:|
| ``./fatsim < fatsim1.txt``       | 5      | 0|51 |
| ``./fatsim < fatsim2.txt``       | 5      |0|102|
| ``./fatsim < fatsim3.txt``       | 5      |12|15|
| ``./fatsim < fatsim4.txt``       | 5      |3272|19338|
| ``./fatsim < fatsim5.txt``       | 5      |12778|167400|
| ``./fatsim < fatsim6.txt``       | 5      |24047|349902|
| Total                        | 30     ||

