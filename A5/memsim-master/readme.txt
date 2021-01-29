--compile

$ make

-- tests

s./memsim 33 < memsim0.txt
------------ Results -----------
pages requested:    57
largest free chunk: 110
elapsed time:       0.000

./memsim 3 < memsim1.txt

------------ Results -----------
pages requested:    47
largest free chunk: 1
elapsed time:       0.000
./memsim 35 < memsim1.txt

------------ Results -----------
pages requested:    4
largest free chunk: 0
elapsed time:       0.000

./memsim 3 < memsim2.txt

------------ Results -----------
pages requested:    2092
largest free chunk: 6276
elapsed time:       0.000
./memsim 1000 < memsim2.txt

------------ Results -----------
pages requested:    7
largest free chunk: 7000
elapsed time:       0.000

./memsim 1000 < memsim3.txt

------------ Results -----------
pages requested:    199
largest free chunk: 1000
elapsed time:       0.008

./memsim 1 < memsim3.txt

------------ Results -----------
pages requested:    198866
largest free chunk: 995
elapsed time:       0.009

./memsim 1000 < memsim4.txt

------------ Results -----------
pages requested:    1891
largest free chunk: 436930
elapsed time:       0.278

./memsim 1001 < memsim5.txt

------------ Results -----------
pages requested:    1623945
largest free chunk: 10251
elapsed time:       1.196

./memsim 123 < test1.txt

------------ Results -----------
pages requested:    57
largest free chunk: 110
elapsed time:       0.000

./memsim 321 < test2.txt

------------ Results -----------
pages requested:    16
largest free chunk: 136
elapsed time:       0.000

./memsim 111 < test3.txt

------------ Results -----------
pages requested:    0
largest free chunk: 0
elapsed time:       0.000

./memsim 222 < test4.txt

------------ Results -----------
pages requested:    896
largest free chunk: 1000
elapsed time:       0.007

./memsim 333 < test5.txt

------------ Results -----------
pages requested:    121597
largest free chunk: 9445200
elapsed time:       0.554

./memsim 606 < test6.txt

------------ Results -----------
pages requested:    2682392
largest free chunk: 9996
elapsed time:       1.232
