# fsc
felix's stupid (c) compiler 

# notes
there are a few memory leaks, mostly one time allocations that are
never cleaned up. this is somewhat lazyness, and somewhat a design decision
as compilers are (hopefully) short lived programs and the memory will be freed up anyway 
by the operating system

this compiler is not compliant to any c standard, nor will it be. its mostly written
with referral to the c99 and c11 specs. 

the main goal here is to produce a compiler that can compile itself

the compiler does not compile natively into machine code, it will compile into
bytecode for a virtual machine (jjvm), this is mostly to keep things cross platform
so i can get the same result on windows, linux, and macOS. perhaps one day i 
will implement a backend to gen x64 asm?

there are little to no semantic checks, the compiler mostly assumes that the code
given to it is error free. this may change in the future, but first the frontend and the codegen
is all i care about.

there are no optimisations, though this is something i want to experiment with

the compiler uses an existing pre-processor via `cc -E`. this aspect of the
compiler is very hacky! perhaps ill try implement my own pre-processor in the future
but this isn't my interest right now

# building
to build fsc you need:

* C compiler (clang, gcc, ...)
* GNU Make
* [collections-c](https://github.com/srdja/Collections-C#building-and-installation)

# references
[c99 spec](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
[c99 spec](http://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf)
[c99? BNF](https://cs.wmich.edu/~gupta/teaching/cs4850/sumII06/The%20syntax%20of%20C%20in%20Backus-Naur%20form.htm)
[c11? ANTLR grammar](https://raw.githubusercontent.com/antlr/grammars-v4/master/c/C.g4)