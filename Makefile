.PHONY: all clean

all: ovmf/ovmf-vars-x86_64.fd ovmf/ovmf-code-x86_64.fd
	cd bootloader; \
	make clean; \
	make all; \
	cd ../kernel; \
	make -f Bin_Makefile clean; \
	make -f Bin_Makefile all; \
	make -f ELF_Makefile clean; \
	make -f ELF_Makefile all; \
	cd ..; \
	find . -name "*.img" -type f -delete; \
	sudo ./Make_Image.sh \  
	./Run_Image.sh

ovmf/ovmf-vars-x86_64.fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-vars-x86_64.fd

ovmf/ovmf-code-x86_64.fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-x86_64.fd

clean:
	find . -name "*.d" -type f -delete
	find . -name "*.o" -type f -delete
	find . -name "*.bin" -type f -delete
	find . -name "*.elf" -type f -delete
	find . -name "*.EFI" -type f -delete
	find . -name "*.img" -type f -delete
	find . -name "*.fd" -type f -delete
