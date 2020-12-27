This code will not compile as its been stipped down. Its purpose is
serve as an example for your own developement of a simple multi-tasking
kernel. It was originally written for a z180 using an embedded c
compiler. The register requirements for that compiler dictated which
registers were saved in the stack image. You most likely will need to
modify which registers are saved. For a completely general purpose
kernel, you will have to save all registers but the result will be a
slower context switch.
