.PHONY: all clean

all:
	cd kernel; \
	make; \
	cd ../bootloader; \
	make; \
	cd ..; \
	find . -name "*.img" -type f -delete; \
	sudo ./Make_Image.sh  

clean:
	find . -name "*.d" -type f -delete
	find . -name "*.o" -type f -delete
	find . -name "*.bin" -type f -delete
	find . -name "*.EFI" -type f -delete
	find . -name "*.img" -type f -delete
