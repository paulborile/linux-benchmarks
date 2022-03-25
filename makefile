all: 
	gcc -o dhry -O3 dhry.c
	gcc -o whets -DUNIX_Old -O whets.c -lm
	gcc -o fstones -O3 fstones.c
	gcc -o ack -O3 ack.c

