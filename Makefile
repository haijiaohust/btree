objects=btree.o test.o

all:$(objects)
	cc -o test $(objects)

.PHONY:clean
clean:
	rm -rf *.o test