obj-m := 01.o
K_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all :
	$(MAKE) -C $(K_DIR) M=$(PWD) modules

clean :
	$(MAKE) -C $(K_DIR) M=$(PWD) clean