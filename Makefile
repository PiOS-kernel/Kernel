LDFILE=testing/linker.ld
LIBPATH=build/libpios.a
TESTSPATH=build/tests.elf

KERNELFILES_C=$(subst .c,.o,$(wildcard kernel/*/*.c kernel/*.c))
KERNELFILES_S=$(subst .s,.o,$(wildcard kernel/port/gcc/*.s))
TESTSFILES_C=$(subst .c,.o,$(wildcard testing/*/*.c testing/*.c))
TESTSFILES_S=$(subst .s,.o,$(wildcard testing/*.s))

BUILDFILES_C=$(subst kernel/,build/,$(KERNELFILES_C))
BUILDFILES_S=$(subst kernel/port/gcc/,build/,$(KERNELFILES_S))

BUILDTESTING_C=$(subst testing/,build/,$(TESTSFILES_C)) $(BUILDFILES_C)
BUILDTESTING_S=$(subst testing/,build/,$(TESTSFILES_S)) $(BUILDFILES_S)

# Rule for building the kernel as a static library

libpios: $(BUILDFILES_C) $(BUILDFILES_S)
	@echo building libpios...
	@ar rcs $(LIBPATH) $(addprefix build/,$(notdir $^))

# Rule for building tests
testsexec: $(BUILDTESTING_C) $(BUILDTESTING_S)
	@echo building tests...
	@arm-none-eabi-gcc -Wall -O0 -mcpu=cortex-m4 -mthumb --freestanding -nostartfiles -ggdb -T $(LDFILE) -o $(TESTSPATH) $(addprefix build/,$(notdir $^))

# Rules for C files

build/%.o: kernel/%.c
	@[ -d build ] || mkdir build
	@arm-none-eabi-gcc -Wall -O0 -mcpu=cortex-m4 -mthumb --freestanding -nostartfiles -ggdb -c $^ -o build/$(notdir $@)

build/%.o: testing/%.c
	@[ -d build ] || mkdir build
	@arm-none-eabi-gcc -Wall -O0 -mcpu=cortex-m4 -mthumb --freestanding -nostartfiles -ggdb -c $^ -o build/$(notdir $@)

# Rules for S files
build/%.o: kernel/port/gcc/%.s
	@[ -d build ] || mkdir build
	@arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m4 -ggdb $^ -o $@

build/%.o: testing/%.s
	@[ -d build ] || mkdir build
	@arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m4 -ggdb $^ -o $@

# Rule for cleaning the build directory

clean:
	@rm build/*.a build/*.o build/*.elf 2>/dev/null || true

# Rule for running the executable in qemu with gdb

gdb:
	@make clean && make testsexec
	@qemu-system-arm -cpu cortex-m4 -machine lm3s6965evb -nographic -gdb tcp::3333 -S -kernel $(TESTSPATH)

# Rule for running the executable in qemu without gdb

test:
	@make clean && make testsexec
	@qemu-system-arm -cpu cortex-m4 -machine lm3s6965evb -nographic -kernel $(TESTSPATH)


