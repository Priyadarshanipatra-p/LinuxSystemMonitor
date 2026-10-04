obj-m += resource_monitor.o

KDIR := /home/priya/WSL2-Linux-Kernel

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
