#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define KiB4 1
#define KiB16 4
#define KiB64 16
#define KiB256 64
#define MiB1 256
#define MiB4 1024

int main() {
        int buf[1024];
        int fd = open("/root/tmpfs/test2", O_RDWR | O_CREAT | O_TRUNC);
        for (int i =0; i < MiB1; i++) {
                for (int j = 0; j < 1024; j++) {
                        buf[j] = i * 1024 + j;
                }
                write(fd, buf, 1024 * sizeof(int));
        }
}

