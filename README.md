# πOS

## Hardware Requirements

πOS (you should read it as `pì·os`) is an embedded Real Time Operating System developed for the `Cortex-M4` processor. It fully supports the `msp432` launchpad.

## Getting started

To have your application link against πOS you should compile the source code in order to import it on your project. 
Once you've done that you can flash your app on the microcontroller and run it.
Running πOS on a diffeerent platform would require expanding the port module accordingly.

## Module organization

The source code is in the [kernel](kernel) directory, divided into the different modules.
* The [heap](kernel/heap) directory contains the code that manages dynamic memory allocation.
* The [task](kernel/task) directory contains the code for the scheduler.
* In the [syscalls](kernel/syscalls) directory, you'll find the system calls supported by πOS.
* The [synch](kernel/synch) directory contains the code for tasks synchronization.
* The [utils](kernel/utils) directory contains some utility code.
* Finally, in the [port](kernel/port) directory you can find source code that is
specific to the compiler used to build an application that links against πOS. Much of that code is written in assembly.

## Unit tests

To run unit tests, you have two options:
* ```$ make test``` to run tests on qemu directly.
* ```$ make gdb``` to run tests with GDB. 
Have a look at the README at
[this link](https://github.com/PiOS-kernel/testing) to learn more about testing with GDB.


## Transpiling Assembly code

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

## Presentation

Here you can find a brief [presentiation](https://docs.google.com/presentation/d/1UwrHFOR5TECv7jCFEYzsqXVi26k9xcZ9c9n7NmlnuEQ/edit#slide=id.gc6f9e470d_0_0) going through some details of πOS 

And a short presentation [video](https://drive.google.com/file/d/1wdovbkCU4LN8i_uqJ_YX4u1BSG1Roq7t/view?usp=sharing) 

##Demo App

A [demo app](https://github.com/PiOS-kernel/demo-app-msp432) for the TI MSP432 board based on πOS is available as an example of a use case of our embedded Operating System

## Team members

πOS has been developed by Barbieri Marco, Maglie Francesco and Zaupa Alberto. Main contribution:

Barbieri Marco - Scheduling functions, syscall and synch development 

Maglie Francesco -  Start-up routine, porting, synch development, Demo APP

Zaupa Alberto - General code organization, memory allocation and syscall development, testing
