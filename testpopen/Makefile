GTESTDIR=../gtest
CPPFLAGS=-I$(GTESTDIR)
CXXFLAGS=-lpthread

CXX=g++

all: popen

%.o:%.c
	$(CXX) $(CPPFLAGS) -c $< -o $@

popen: popen.o $(GTESTDIR)/gtest-all.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

.PHONY:clean
clean:
	@rm popen popen.o

