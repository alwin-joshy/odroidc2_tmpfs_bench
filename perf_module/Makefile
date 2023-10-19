
obj-m += perfcnt_enable.o

TEST := "6.1.50-current-meson64"

all:
	make -C /lib/modules/$(TEST)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(TEST)/build M=$(PWD) clean
