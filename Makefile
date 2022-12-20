LDFILE=src/linker.ld

KERNELFILES_C=$(subst .c,.o,$(wildcard kernel/*.c))
KERNELFILES_S=$(subst .s,.o,$(wildcard kernel/port/gcc/*.s))
HEAPFILES_C=$(subst .c,.o,$(wildcard kernel/heap/*.c))
SYSCALLSFILES_C=$(subst .c,.o,$(wildcard kernel/syscalls/*.c))
TASKFILES_C=$(subst .c,.o,$(wildcard kernel/task/*.c))

BUILDKERNELFILES_C=$(subst kernel/,build/,$(KERNELFILES_C))
BUILDKERNELFILES_S=$(subst kernel/port/gcc/,build/,$(KERNELFILES_S))
BUILDHEAPFILES_C=$(subst kernel/heap/,build/,$(HEAPFILES_C))
BUILDSYSCALLSFILES_C=$(subst kernel/syscalls/,build/,$(SYSCALLSFILES_C))
BUILDTASKFILES_C=$(subst kernel/task/,build/,$(TASKFILES_C))
BUILDFILES_C=$(BUILDKERNELFILES_C) $(BUILDHEAPFILES_C) $(BUILDSYSCALLSFILES_C) $(BUILDTASKFILES_C)
BUILDFILES_S=$(BUILDKERNELFILES_S)

kernel.a: $(BUILDFILES_C) $(BUILDFILES_S)
	@echo building libpios...
	@ar rcs build/libpios.a $(BUILDFILES_C) $(BUILDFILES_S)
	@rm build/*.o

# Rules for C files

build/%.o: kernel/%.c
	@echo building $<...
	@[ -d build ] || mkdir build
	@arm-none-eabi-gcc -Wall -O0 --freestanding -mcpu=cortex-m4 -mthumb -nostartfiles -ggdb -c $^ -o $@

build/%.o: kernel/heap/%.c
	@echo building $<...
	@[ -d build ] || mkdir build
	@arm-none-eabi-gcc -Wall -O0 --freestanding -mcpu=cortex-m4 -mthumb -nostartfiles -ggdb -c $^ -o $@

build/%.o: kernel/syscalls/%.c
	@echo building $<...
	@[ -d build ] || mkdir build
	@arm-none-eabi-gcc -Wall -O0 --freestanding -mcpu=cortex-m4 -mthumb -nostartfiles -ggdb -c $^ -o $@

build/%.o: kernel/task/%.c
	@echo building $<...
	@[ -d build ] || mkdir build
	@arm-none-eabi-gcc -Wall -O0 --freestanding -mcpu=cortex-m4 -mthumb -nostartfiles -ggdb -c $^ -o $@

# Rules for S files
build/%.o: kernel/port/gcc/%.s
	@echo building $<...
	@[ -d build ] || mkdir build
	@arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m4 -ggdb $^ -o $@

# Rule for cleaning the build directory

clean:
	@rm build/*.a
