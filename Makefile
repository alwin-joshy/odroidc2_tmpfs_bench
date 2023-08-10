makerule: file_gen.c benchmark.c
	gcc -o benchmark benchmark.c -lm
	gcc -o file_gen file_gen.c
