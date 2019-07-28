#CFLAGS='-DDEBUG'
gcc $CFLAGS -c -g stack.c
gcc $CFLAGS -c -g main.c 
gcc $CFLAGS -o filewatch main.o stack.o 
