#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

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

void benchmark_mmap() {
	size_t filesize = MiB1;
	__uint64_t start, end;
	__uint64_t sum = 0;
	__uint64_t  n_iters = filesize/4096;
	const __uint64_t file_sum = ((n_iters*1024 - 1)*(n_iters*1024))/2;

	asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(start));
	for (int i = 0; i < 100; i++) {
		int fd = open("/root/tmpfs/test2", O_RDONLY);

		/* Prevent prefetching to make it more fair */
		//madvise((int *) ADDR, filesize, MADV_SEQUENTIAL);
		
		mmap(ADDR, filesize, PROT_READ, MAP_PRIVATE, fd, 0);

		for (int *curr= (int *) ADDR; curr < ADDR + filesize;
		     curr++) {

			sum += *curr;
		}
		
		munmap(ADDR, filesize);

		close(fd);

	}
	asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(end));
	//printf("%lu %lu\n", sum, 100 * file_sum);
	assert(sum == 100 * file_sum);
	printf("%ld\n", end - start);
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

	printf("hi\n");
	benchmark_mmap();
}
