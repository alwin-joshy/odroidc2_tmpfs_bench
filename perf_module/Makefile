
obj-m += perfcnt_enable.o

all:
	make -C /lib/modules/6.1.42-meson64/build M=$(PWD) modules

clean:
	make -C /lib/modules/6.1.42-meson64/build M=$(PWD) clean
