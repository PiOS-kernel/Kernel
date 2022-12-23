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