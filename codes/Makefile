# Simple Makefile

# Your compiler
CC = g++
# Compilation flags
# '-g' turns debugging flags on.
CFLAGS = -g -I /usr/include/opencv -L /usr/lib

# Linker flags
# When you need to add a library
# '-lm' adds the math library
LDFLAGS = -lm -lcv -lhighgui -lcvaux -lserial

# all is a target. main is a 'dependency' or requirement for all to be complete
all: main_in

# The main program depends on the complex number library, and the main file
main_in: main_in.o bg.o in_gate.o iso.o lpsplit.o ocr.o preprocessing.o rot.o split.o yellow.o
	$(CC) $(LDFLAGS) -o $@ $^			# $@ stores the target, 'main' $^ stores all the dependencies 'main.o complex.o'

main_in.o: main_in.c bg.h in_gate.h iso.h lpsplit.h ocr.h preprocessing.h rot.h split.h yellow.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h files)

bg.o: bg.c bg.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

in_gate.o: in_gate.c in_gate.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

iso.o: iso.c iso.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

lpsplit.o: lpsplit.c lpsplit.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

ocr.o: ocr.cpp ocr.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

preprocessing.o: preprocessing.c preprocessing.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

rot.o: rot.c rot.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

split.o: split.c split.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

yellow.o: yellow.c yellow.h
	$(CC) -c $(CFLAGS) -o $@ $<			# $< stores the first target, just the '.c' file (we won't compile the .h file)

# .PHONY tells make that 'all' or 'clean' aren't _actually_ files, and always
# execute the compilation action when 'make all' or 'make clean' are used
.PHONY: all clean							

# Delete all the temporary files we've created so far
clean:
	rm -rf main_in.o bg.o in_gate.o iso.o lpsplit.o ocr.o preprocessing.o rot.o split.o yellow.o
	rm -rf main_in

