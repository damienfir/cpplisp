# cpplisp

A small Lisp dialect interpreter written in C++

## About

I believe that all programmers should know how a programming language is implemented, so I'm doing that with Lisp. This is just an interpreter, although I might implement a VM or compiled version at some point.

Needless to say that **cpplisp** is not production ready, but I don't know who would even try that.

## How to build and run

Use common CMake build steps:

```bash
mkdir build && cd build
cmake ..
make
```

For the moment, **cpplisp** does not have an interpreter or a script reader, it only has tests to confirm that the language is correctly implemented.

## Author

Damien Firmenich - [dfirmenich](https://twitter.com/dfirmenich)

## License

cpplisp is licensed under a MIT license.