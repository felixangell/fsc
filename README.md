# javajava
java but twice as good

this is the logo the code can come later
![](/logo.png)

just a stupid project to ease me back in2 programming

## goals
i want it to be easy to build and execute
the code should compile fast


## notes
its written in c
the virtual machine is called "jjvm" and is not really documented
jjvm steals a lot from the jvm's architecture
the code is pretty messy
its a work in progress
to install you just need make and a compiler, i'm using clang

	git clone http://github.com/felixangell/javajava
	cd javajava
	make
	./a.out tests/some_test.yava

Pass a file in as an argument to "compile it"
as of writing this the compiler just lexes things

## siphash
the hashtable implementation uses SipHash https://github.com/veorq/SipHash