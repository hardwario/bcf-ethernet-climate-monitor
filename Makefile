SDK_DIR ?= sdk
VERSION ?= vdev

CFLAGS += -D'VERSION="${VERSION}"'

INC_DIR += ioLibrary_Driver/Ethernet
INC_DIR += ioLibrary_Driver/Internet/httpServer

SRC_DIR += ioLibrary_Driver
SRC_DIR += ioLibrary_Driver/Ethernet
SRC_DIR += ioLibrary_Driver/Ethernet/W5500
SRC_DIR += ioLibrary_Driver/Internet/httpServer

-include sdk/Makefile.mk

.PHONY: all
all: debug

.PHONY: sdk
sdk: sdk/Makefile.mk

.PHONY: update
update:
	@git submodule update --remote --merge sdk
	@git submodule update --remote --merge .vscode

sdk/Makefile.mk:
	@git submodule update --init sdk
	@git submodule update --init .vscode
