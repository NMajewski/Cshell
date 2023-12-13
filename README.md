# UNIX Shell Clone in C
![UNIx SHEll](https://github.com/NMajewski/Shell/assets/90190080/bcec5e37-5912-42c3-9bd0-3f56eaabdd07)


A UNIX Shell clone written in C, complete with a Makefile and documentation.

The shell supports most Linux commands like "cd", "pwd", "ls", and so on. It also supports I/O piping, conjunction operators ("command_1 && command_2"), and a subshell which runs commands in a separate shell (wrap desired subshell command in parenthesis like so: "(command)").

### Installation Prerequisites

* Access to a shell / terminal

### Setup

In the main project directory, run ```make all```, then run the executable with ```./sh3ll```

```
$ cd shell dir/
$ make all
$ ./sh3ll
```
To rebuild the project, run ```make clean``` to remove the build, then repeat ```make all```

## Usage

A few examples of useful commands and/or tasks after running the sh3ll executable:
The first example creates a backup directory in the parent directory of the current directory

```
$ mkdir ../bkup && tar cf - . | (cd ../bkup && tar xf -)
$ echo hired
$ pwd
```
Thank you for looking
