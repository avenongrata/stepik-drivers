obj-m += solution_number.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/moudles/$(shell uname -r)/build M=$(PWD) clean
