CCFLAGS:=$(CFLAGS) -I..

all: gtest.a

gtest.a: gtest-all.o gtest_main.o
	ar rcs gtest.a gtest-all.o gtest_main.o

gtest-all.o: gtest-all.cc
	g++ $(CCFLAGS) -c gtest-all.cc -o gtest-all.o

gtest_main.o: gtest_main.cc
	g++ $(CCFLAGS) -c gtest_main.cc -o gtest_main.o

.PHONY: clean
clean:
	rm -f gtest.a gtest_main.o gtest-all.o
