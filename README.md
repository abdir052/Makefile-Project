# Makefile-Project
/* CSci4061 Fall2018 Assignment 1
* login: alixx577
* date: 10.04.18
* name: Ali Adam, Abdirahman Abdirahman, Hassan Ali
* id: 5330610, 5330419, 4570634 */


Make is a useful utility which builds executable programs or libraries from source files based on an input makefile which includes information on how to build targets. Our make4061 will be responsible for analyzing dependencies of targets, which determines which targets are eligible to be built (each target may depend on other targets). Those other targets must be built first. As targets in makefile are built, our make4061 program will determine which targets in the makefile have become eligible to be compiled.


HOW TO USE:

Copy the make4061 executible to the folder you want to use it in. Then run ./make4061 [options].
make4061 supports the following options:

1)	-f [filename] : filename is the name of the makefile. if this option is not used, the default name 		"Makefile" is used.

2)	[SpecificTarget]

3)	-h : To print the usage info 

Options can be combined together e.g.

 ./make4061 -f ownmakefile specificTarget


Abdirahman worked on the build function with the help of Ali and Hassan. Ali worked on the timestamp implementation with the assistance of Abdi and Hassan. Hassan did the warm-up, cleaned up the code, error checked, fixed bugs and wrote the README file with the assistance of Abdi and Ali. 
