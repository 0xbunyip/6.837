INCFLAGS  = -I /usr/include/GL
# INCFLAGS += -I mit-vecmath/output/vecmath

LINKFLAGS  = -lglut -lGL -lGLU #-Wl,--trace
# LINKFLAGS += -L mit-vecmath/output -lvecmath
LINKFLAGS += -lvecmath

CFLAGS    = -O2
CC        = g++
SRCS      = main.cpp
OBJS      = $(SRCS:.cpp=.o)
PROG      = a0

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)

