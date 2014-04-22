linux-benchmarks
================

Collection of benchmaks for unix/linux platform, mainly in C language.

Drystones

	$ gcc -o dhry -O dhry.c
	$ ./dhry
	Dhrystone (version 1.1) time for 100000000 passes = 18
	This machine benchmarks at 5555555 dhrystones/second

Wetstones

	$ gcc -o whets -DUNIX_Old -O whets.c -lm
	$ ./whets
	##########################################
	Single Precision C/C++ Whetstone Benchmark

	Calibrate
	       0.01 Seconds          1   Passes (x 100)
	       0.05 Seconds          5   Passes (x 100)
	       0.21 Seconds         25   Passes (x 100)
	       1.01 Seconds        125   Passes (x 100)
	       5.43 Seconds        625   Passes (x 100)

	Use 11505  passes (x 100)

          Single Precision C/C++ Whetstone Benchmark

	Loop content                  Result              MFLOPS      MOPS   Seconds

	N1 floating point     -1.12441420555114746       556.413              0.397
	N2 floating point     -1.12241148948669434       375.309              4.120
	N3 if then else        1.00000000000000000                 692.307    1.720
	N4 fixed point        12.00000000000000000               30200.682    0.120
	N5 sin,cos etc.        0.49904659390449524                  41.928   22.830
	N6 floating point      0.99999988079071045       164.916             37.630
	N7 assignments         3.00000000000000000                 381.026    5.580

	....

Fstones

	$ gcc -o fstones -O fstones.c
	$ ./fstones BULK 10000
	Phase 1 ** Sequential file creation
		   Creating a 10000 blocks file (131072 byte blocks)
		   Elapsed time 22 - 56M/sec
		   Average write service time 2.200000000 ms.
	Phase 2 ** Sequential file reading
		   Elapsed time 20 - 62M/sec
		   Average read service time 2.000000000 ms.
	Phase 3 ** Random file read
		   Elapsed time 1 - 1250M/sec
		   Average seek-read time 10.000000000 ms.
	Phase 4 ** Random file write
		   Elapsed time 1 - 1250M/sec
		   Average seek-write time 10.000000000 ms.
	Phase 5 ** Directory test
		   Elapsed time 3 - index 33333/sec
	...
	...

Ack - Ackerman function (http://en.wikipedia.org/wiki/Ackermann_function)

	$ gcc -o ack -O ack.c
	$ time ./ack


Enjoy
