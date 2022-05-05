# Boa

Boa is a high level functional programming language that compiles to a single
binary. Boa supports both dynamic and fully static linking, and can be used to
build an easily distributable and fully static binary program, much like Go.

## Build

To compile boa, simply run

```
$ make -j$(nproc)
```

A `boa` executable program can then be found in the `bin/` directory.

Then, install the standard library file:

```
$ mkdir ~/.boa
$ cp lib/libboaruntime.a ~/.boa/.
```

Optionally, install the `boa` program to a directory in your `$PATH`, for
example

```
$ sudo cp bin/boa /bin/.
```

Doing so will allow you to execute boa programs as if they were scripts, however
all boa programs are compiled to native machine code.

## Usage

Boa supports the follow command line options:

```
$ boa -h

boa: [-b] [-h] [-o outfile] infile

        -b    Build only, don't run the boa program

        -h    Display this help menu, then exit

        -o outfile
              Instead of the default output filename,
              write binary to outfile
```

## Examples

Many examples can be found in the `example/` directory. For example, an
implementation of the collatz conjecture can be found below:

```
#!/usr/bin/env boa
#
#   This is an exemple of a boa program
#   Collatz Conjecture, see: https://en.wikipedia.org/wiki/Collatz_conjecture
#

rec_collatz = acc, n => {
    if n == 1:
        -> acc
    else if n % 2 == 0:
        new_n = n / 2
        new_acc = append(acc, new_n)
        -> rec_collatz(new_acc, new_n)
    else
        new_n = 3 * n + 1
        new_acc = append(acc, new_n)
        -> rec_collatz(new_acc, new_n)
}

collatz = n => {
    -> rec_collatz([n], n)
}

println(collatz(10))
```

When executed, produces the output below:

```
$ ./bin/boa example/collatz.boa
[10, 5, 16, 8, 4, 2, 1]
```

To build a binary with boa, simply call it with the `-b` option:

```
$ ./bin/boa -b example/collatz.boa
$ ls -l collatz
-rwxrwxr-x 1 kyle kyle 18568 May  4 20:29 collatz
```

One can also build a fully static executable file for easy distribution. This
however utilizes [musl libc](https://musl.libc.org/) and it requires `musl-gcc`
to be installed in the PATH of the host system. After installing `musl-gcc`,
if you set the `BOACC` environment variable to `musl-gcc`, then boa will
automatically produce a fully static build of your program.

```
$ BOACC=musl-gcc ./bin/boa -b example/collatz.boa
$ ls -l collatz
-rwxrwxr-x 1 kyle kyle 42688 May  4 20:31 collatz
$ ldd collatz
	not a dynamic executable
$ ./collatz
[10, 5, 16, 8, 4, 2, 1]
```
