# cpplisp

A small Lisp dialect interpreter written in C++

## About

I believe that all programmers should know how a programming language is implemented, so I'm doing that with Lisp. This
is just an interpreter, although I might implement a VM or compiled version at some point.

Needless to say that **cpplisp** is not production ready, but I don't know who would even try that.

## Features

- if-else statements
  ```lisp
  (if <cond> <true> <false>)
  ```
- conditional statements
  ```lisp
  (cond ((<cond> <action>) (<cond> <action>) (else <action>)))
  ```
- Comments
  ```lisp
  (println "print this")
  ; this is a comment
  (println "print that too")
  ```
- Lists
  ```lisp
  (list 1 2 3)
  (first (list 1 2 3)) ; -> 1
  (rest (list (1 2 3)) ; -> (list 2 3)
  ```
- Arithmetic
  ```lisp
  (+ 1 2 (/ 3 4) (- 6 3) (* 3 4 2))
  ```
- Print statement
  ```lisp
  (println 1)
  ```
- Strings handling
  ```lisp
  (println "Hello")
  ```
- Lambda functions
  ```lisp
  (lambda (x y) (+ x y))
  ```
- Variable assignments
  ```lisp
  (define x 1)
   ```
- Comparison operators
  ```lisp
  (if (= 1 2) "equal" "not equal")
  ```
- `Let` blocks for local scoping
  ```lisp
  (let (x 1 y 2) (+ x y))
  ```
- `Do` blocks for multi-expression
  ```lisp
  (do (define x 1) (define y 2) (+ x y))
  ```
- Recursion  
   ```lisp
  (define factorial (lambda (n)
      (if (= n 0)
          1 
          (* n (factorial (- n 1))))))
  (factorial 10) ; -> 3628800
  ```

## How to build and run

Use common CMake build steps:

```bash
mkdir build && cd build
cmake ..
make
```

For the moment, **cpplisp** does not have an interpreter or a script reader, it only has tests to confirm that the
language is correctly implemented.

## Author

Damien Firmenich - [dfirmenich](https://twitter.com/dfirmenich)

## License

cpplisp is licensed under a MIT license.