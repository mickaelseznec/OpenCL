CPPFLAGS := -std=c++11 -Wall -Wextra -Werror
LDFLAGS := -framework opencl

PHONY := all clean
TARGETS := viewer

all: $(TARGETS)

clean:
	$(RM) $(TARGETS)
