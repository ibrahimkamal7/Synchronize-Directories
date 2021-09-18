# Synchronize Directories

This is a utility program to synchronize two directories. 

Running the program:

For running the program, use make -f makefile.txt synchronize. This will create an executable synchronize. Then you can run the program using ./synchronize directory_a directory_b. After running the program, use make -f makefile.txt clean to remove the executable and the object file created by make.

### Note: This is not a recursive program, so it will not synchronize directories inside of a directory
