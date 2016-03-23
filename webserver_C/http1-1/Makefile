.PHONY:clean clean_obj
#
# Define WorkingPath &  PORT_USE
#
custom_workingPath = \"./www\"
custom_port_use = 8080
custom_http_version = \"http/1.1\"

# Declear sector
__DIRNAME=$(shell pwd)
__SRCNAME=$(__DIRNAME)/src

common-h =  $(__SRCNAME)/common.h
server-src-c = $(__SRCNAME)/server.c
services-src-c = $(__SRCNAME)/services.c
services-src-h = $(__SRCNAME)/services.h

server-build-flag = -lpthread -Wall -DHTTP_VERSION=$(custom_http_version)
server-build-src = $(server-src-c) services.o
services-build-flag = -Wall
services-build-src = $(services-src-c)

server-dependence = services.o $(server-src-c) $(common-h)
services-dependence = $(services-src-c) $(services-src-h) $(common-h)

ifdef custom_workingPath
	server-build-flag += -DWorkingPath=$(custom_workingPath)
endif

ifdef custom_port_use
	server-build-flag += -DPORT_USE=$(custom_port_use)
endif

# Build sector
all:  server clean_obj

server: $(server-dependence)
	$(CC) $(server-build-src) -o server $(server-build-flag)

services.o: $(services-dependence)
	$(CC) -c $(services-build-src) $(services-build-flag)

clean:
	-@rm server

clean_obj:
	-@rm *.o
