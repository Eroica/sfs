all: sfs.so

clean:
	rm *.o *.so

sfs.so: sfs.o mkpath.o
	cc -bundle -undefined dynamic_lookup -o $@ $^

sfs.o: sfs.c
	cc -fPIC -o $@ -c $<

mkpath.o: mkpath.c
	cc -fPIC -o $@ -c $<
