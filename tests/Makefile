CXX=g++
RM=rm -f
CXXFLAGS := -std=c++20
SRCS=main.cc
OBJS=$(subst .cc,.o,$(SRCS))


test: main
	./main
	@$(RM) main

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(CXXFlAGS) $(LDLIBS)

main.o: main.cc ../skibidi.hh

clean:
	$(RM) $(OBJS)

distclean: clean
	@$(RM) main
