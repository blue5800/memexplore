CONFIG_MODULE_SIG=n
obj-m += kernrw.o
kernrw-objs := read_write.o main.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
