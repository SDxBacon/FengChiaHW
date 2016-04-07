.PHONY: clean all

__DIRNAME = $(shell pwd)
__SRCDIR := $(__DIRNAME)/src

all: portProbe
	
portProbe: $(__SRCDIR)/portProbe.c $(__SRCDIR)/portProbe.h
	$(CC) $(__SRCDIR)/portProbe.c -o $(__DIRNAME)/portProbe -lcprops -lpthread
	
clean:
	-@rm portProbe