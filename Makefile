# © 2017 Aaron Taylor <ataylor at subgeniuskitty dot com>
# See LICENSE file for copyright and license details.

####################################################################################################
# Executables

CC              = gcc

####################################################################################################
# Flags

CC_FLAGS        += -Wall -std=c99

####################################################################################################
# Targets

all: 
	$(CC) $(CC_FLAGS) -o pdp11-serial-loader server.c

install: all
	mkdir -p /home/ataylor/bin/pdp/bin
	cp pdp11-serial-loader /home/ataylor/bin/pdp/bin/pdp11-serial-loader

uninstall:
	rm /home/ataylor/bin/pdp/bin/pdp11-serial-loader

clean:
	rm pdp11-serial-loader
