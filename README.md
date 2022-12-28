# πOS

πOS (you should read it as `pì·os`) is an embedded Operating System for the `Cortex-M4` processor. It fully supports the `msp432` launchpad.

To have your application link against πOS, you can either directly compile the source code, or you can choose to compile the kernel as a static library. We reccomend the first option as it is more stable.

To compile πOS as a static library just run `make` in the root directory.

## Module organization

The source code is in the [kernel](kernel) directory, divided into the different modules.
* The [heap](kernel/heap) directory contains the code that manages dynamic memory   allocation.
* The [task](kernel/task) directory contains the code for the scheduler.
* In the [syscalls](kernel/syscalls) directory, you'll find the system calls supported by πOS.
* The [utils](kernel/utils) directory contains some utility code.
* Finally, in the [port](kernel/port) directory you can find source code that is
specific to the compiler used to build an application that links against πOS. Much of that code is written in assembly.


## Unit tests

To run unit tests, you have two options:
* ```$ make test``` to run tests on qemu directly.
* ```$ make gdb``` to run tests with GDB. Have a look at the README at
[this link](https://github.com/PiOS-kernel/testing) to learn more about testing with GDB.


# Transpiling Assembly code

In the root directory you can find a [transpiler](transpiler.py) that allows to translate gcc assembly code to ccs assembly code, and vice versa. 

The first time you run the transpiler you need to make sure it has execution permissions:

```
$ chmod +x transpiler.py
```

The transpiler acceppts as arguments:
1. The input assembly file path.
2. The output assembly file path.
3. The identifier of the target compiler (`gcc` or `ccs`)

You can run `./transpiler.py` without arguments to check the arguments format.