# Recursive wildcard make function; recursively searches for files matching 
# given wildcard pattern.
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Source files (*.cpp)
shared_srcs     = $(call rwildcard,../shared,*.cpp)
bootloader_srcs = $(call rwildcard,../bootloader,*.cpp)

# Object files (*.o)
shared_objs     = $(patsubst %.cpp,%.o,$(shared_srcs))
bootloader_objs = $(patsubst %.cpp,%.o,$(bootloader_srcs))

# Dependency information files (*.d)
shared_depends     = $(patsubst %.cpp,%.d,$(shared_srcs))
bootloader_depends = $(patsubst %.cpp,%.d,$(bootloader_srcs))

# Project targets
bootloader_target = BOOTX64.EFI

###############################################################################
# Compiler and Compiler Flags                                                 #
###############################################################################
# -Wl,--subsystem,10         = Flag passed to the linker (x86_64-w64-mingw32-ld)  
#                              to produce an EFI application.
# -nostdlib                  = Prevent linking to standard libraries.
# -03                        = Optimization level 3 (highest)
# -MMD -MP                   = Dump dependency information for each compiled 
#                              .cpp for future makes.
# -Wall, -Wextra, -Wpedantic = Verbose warnings.
# -mno-red-zone              = Compile and generate machine code such that 
#                              there is no assumed red zone below the stack.
# -fpie -pie                 = Generate code independent of where the machine 
#                              code is placed in memory.                      
# -ffreestanding             = Generate code independent of the host platform 
#                              and assume standard libraries don't exist.     
###############################################################################
CXX      = x86_64-w64-mingw32-g++
CXXFLAGS = \
	       -Wl,--subsystem,10 \
	       -nostdlib \
	       -O3 \
	       -MMD \
	       -MP \
	       -Wall \
	       -Wextra \
	       -Wpedantic \
	       -mno-red-zone \
	       -fpie \
		   -pie \
	       -ffreestanding

.PHONY: all clean

all: $(bootloader_target)

# Clean compile outputs from last make.
clean:
	find .. -name "*.d" -type f -delete
	find .. -name "*.o" -type f -delete
	find .. -name "*.EFI" -type f -delete

# Bootloader target depends on the bootloader objects and the shared objects.
$(bootloader_target) : $(bootloader_objs) $(shared_objs)
	$(CXX) $(CXXFLAGS) -e uefi_main $^ -o $@ 

# An object file depends on the respective cpp file.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -c

# Include dependency rules output from previous make.
-include $(shared_depends)
-include $(bootloader_depends)
