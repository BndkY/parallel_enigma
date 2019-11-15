CC=gcc
CFLAGS=-fopenmp -I.
DEPS = p_enigma.h enigma.h
OBJ = p_enigma.o enigma.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

enigma: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


# IDIR =../include
# CC=gcc
# CFLAGS=-I$(IDIR)

# ODIR=obj
# LDIR =../lib

# LIBS=-lm

# _DEPS = hellomake.h
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# _OBJ = hellomake.o hellofunc.o 
# OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


# $(ODIR)/%.o: %.c $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

# hellomake: $(OBJ)
# 	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# .PHONY: clean

# clean:
# 	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 