COMPILER=x86_64-w64-mingw32-g++
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
shared_srcs=$(call rwildcard,./shared,*.cpp)
kernel_srcs=$(call rwildcard,./kernel,*.cpp)
bootloader_srcs=$(call rwildcard,./bootloader,*.cpp)
shared_objs=$(patsubst %.cpp,%.o,$(shared_srcs))
kernel_objs=$(patsubst %.cpp,%.o,$(kernel_srcs))
bootloader_objs=$(patsubst %.cpp,%.o,$(bootloader_srcs))
shared_depends=$(patsubst %.cpp,%.d,$(shared_srcs))
kernel_depends=$(patsubst %.cpp,%.d,$(kernel_srcs))
bootloader_depends=$(patsubst %.cpp,%.d,$(bootloader_srcs))
bootloader_target=BOOTX64.EFI
kernel_target=kernel.elf
COMPILER_FLAGS = \
	      -Wl,--subsystem,10 \
	      -nostdlib \
	      -O3 \
	      -MMD \
	      -MP \
	      -Wall \
	      -Wextra \
	      -Wpedantic \
	      -mno-red-zone \
	      -fpic \
	      -ffreestanding \
	      -fno-stack-protector \
	      -shared

.PHONY: all clean

all: $(bootloader_target)

# Bootloader depends on the bootloader objects and the shared objects.
$(bootloader_target) : $(bootloader_srcs) $(shared_srcs)
	$(COMPILER) $(COMPILER_FLAGS) -e uefi_main $^ -o $@ 

clean:
	find . -name "*.d" -type f -delete
	find . -name "*.o" -type f -delete
	find . -name "*.EFI" -type f -delete

-include BOOTX64.d

