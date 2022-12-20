LDFILE=src/linker.ld

KERNELFILES_C=$(subst .c,.o,$(wildcard kernel/*/*.c kernel/*.c))
KERNELFILES_S=$(subst .s,.o,$(wildcard kernel/port/gcc/*.s))

BUILDFILES_C=$(subst kernel/,build/,$(KERNELFILES_C))
BUILDFILES_S=$(subst kernel/port/gcc/,build/,$(KERNELFILES_S))

build/libpios.a: $(BUILDFILES_C) $(BUILDFILES_S)
	@echo building libpios...
	@ar rcs $@ $(addprefix build/,$(notdir $^))
	@rm build/*.o

# Rules for C files

build/%.o: kernel/%.c
	@[ -d build ] || mkdir build
	@arm-none-eabi-gcc -Wall -O0 -mcpu=cortex-m4 -mthumb --freestanding -nostartfiles -ggdb -c $^ -o build/$(notdir $@)

# Rules for S files
build/%.o: kernel/port/gcc/%.s
	@[ -d build ] || mkdir build
	@arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m4 -ggdb $^ -o $@

# Rule for cleaning the build directory

clean:
	@rm build/*.a
