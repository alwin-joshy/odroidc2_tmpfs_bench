#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
        int buf[1024];
        int fd = open("/root/tmpfs/test2", O_RDWR | O_CREAT | O_TRUNC);
        for (int i =0; i < 256; i++) {
                for (int j = 0; j < 1024; j++) {
                        buf[j] = i * 1024 + j;
                }
                write(fd, buf, 1024 * sizeof(int));
        }
}

