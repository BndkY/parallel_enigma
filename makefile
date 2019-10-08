CC=gcc
CFLAGS=-I.
DEPS = p_enigma.h enigma.h
OBJ = p_enigma.o enigma.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

enigma: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)