#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#define ADDR 0xB000000
/*
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__)) __rdtsc(void) {
	return __
}
*/

#define KiB4 4096
#define KiB16 16384
#define KiB64 65536
#define KiB256 262144
#define MiB1 1048576
#define MiB4 4194304

#define NUM_TESTS 6
#define NUM_SAMPLES 100

#define BENCH_OPEN 0
#define BENCH_MMAP 1
#define BENCH_ITERATE 2
#define BENCH_MUNMAP 3
#define BENCH_CLOSE 4
#define BENCH_FULL 5

#define READ_CCNT(x) asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(x));

/* Change this for different file sizes */
#define FILE_SIZE MIB1
#define N_ITERATION (FILESZ / 4096)
#define FILE_SUM (((N_ITERATIONS * 1024 - 1)*(N_ITERATIONS * 1024))/2)

const char *tests[NUM_TESTS] = {"open", "mmap", "iterate", "munmap", "close", "complete"};
__uint64_t samples[NUM_TESTS][NUM_SAMPLES] = {0};

__uint64_t cc_overhead(){
	__uint64_t start, end = 0;
	asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(start));
	asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(end));
	return end - start;
}


void benchmark_complete() {
	__uint64_t start, end;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		READ_CCNT(start);
		int fd = open("/root/tmpfs/test2", O_RDONLY);

		/* Prevent prefetching to make it more fair */
		mmap(ADDR, FILE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
		
		//posix_madvise((int *) ADDR, filesize, POSIX_MADV_RANDOM);
		for (int *curr= (int *) ADDR; curr < ADDR + FILE_SIZE;
		     curr++) {

			sum += *curr;
		}
		
		munmap(ADDR, FILE_SIZE);

		close(fd);

		READ_CCNT(end);
		samples[BENCH_COMPLETE][i] = end - start;
	}
	assert(sum == 100 * FILE_SUM);

}

void benchmark_component() {
	__uint64_t start_inner, end_inner;
	__uint64_t sum = 0;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		READ_CCNT(start_inner);
		int fd = open("/root/tmpfs/test2", O_RDONLY);
		READ_CCNT(end_inner);
		samples[BENCH_OPEN][i] = end_inner - start_inner;

		/* Prevent prefetching to make it more fair */
		
		READ_CCNT(start_inner);
		mmap(ADDR, FILE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
		READ_CCNT(end_inner);
		samples[BENCH_MMAP][i] = end_inner - start_inner;
		
		//posix_madvise((int *) ADDR, filesize, POSIX_MADV_RANDOM);
		READ_CCNT(start_inner);
		for (int *curr= (int *) ADDR; curr < ADDR + FILE_SIZE;
		     curr++) {

			sum += *curr;
		}
		READ_CCNT(end_inner);
		samples[BENCH_ITERATE][i] = end_inner - start_inner;
		
		READ_CCNT(start_inner);
		munmap(ADDR, FILE_SIZE);
		READ_CCNT(end_inner);
		samples[BENCH_MUNMAP][i] = end_inner - start_inner;

		READ_CCNT(start_inner);
		close(fd);
		READ_CCNT(end_inner);
		samples[BENCH_MUNMAP][i] = end_inner - start_inner;
	}
	assert(sum == 100 * FILE_SUM);
}


__uint32_t read_pmcr() {
	__uint32_t val;
	asm volatile("mrs %0, PMCR_EL0" :  "=r"(val));
	return val;
}

int main(void) {
	printf("Beginning the SMOS ramfs benchmark\n");

	/* Make sure all the counters are stopped or something */
	__uint32_t value = -1;
	printf("%x\n", value);
	asm volatile ("msr PMCNTENCLR_EL0, %0" :: "r"(value));

	/* Make sure div64 is cleared or something */
	value = read_pmcr() & ~(1UL << 3);
	printf("%x\n", value);
	asm volatile ("msr PMCR_EL0, %0" :: "r"(value));
	
	/* Reset all the counters */
	value = read_pmcr() | (1UL << 1) | (1UL << 2);
	printf("%x\n", value);
	asm volatile ("msr PMCR_EL0, %0" :: "r"(value));

	/* Enable counters globally? */	
	value = read_pmcr() | (1UL << 0);
	printf("%x\n", value);
	asm volatile ("msr PMCR_EL0, %0" :: "r"(value));
	
	/* Turn something else on*/
	value = (1UL << 31);
	printf("%x\n", value);
	asm volatile ("msr PMCNTENSET_EL0, %0" :: "r"(value));

	/* Do the benchmark */
	benchmark_component();
	benchmark_full();

	/* Output the results */
	printf("## BEGIN benchmark results ##\n");
	
	for (int i = 0; i < NUM_TESTS; i++) {
		printf("Benchmark: %s\n", benchmarks[i]);
		for (int j = 0; j < NUM_SAMPLES; j++) {
			printf("%lu,", samples[i][j]);
		}
		printf("\n");
	}

	printf("## END benchmark results ##\n");

}
