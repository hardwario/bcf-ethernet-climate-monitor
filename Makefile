SDK_DIR ?= sdk
VERSION ?= vdev

CFLAGS += -D'VERSION="${VERSION}"'
CFLAGS += -D'_WIZCHIP_=5500'

INC_DIR += ioLibrary_Driver/Ethernet
INC_DIR += ioLibrary_Driver/Internet/httpServer

INC_DIR += ioLibrary_Driver/Internet/MQTT
INC_DIR += ioLibrary_Driver/Internet/MQTT/MQTTPacket/src

SRC_DIR += ioLibrary_Driver
SRC_DIR += ioLibrary_Driver/Ethernet
SRC_DIR += ioLibrary_Driver/Ethernet/W5500
SRC_DIR += ioLibrary_Driver/Internet/httpServer

SRC_DIR += ioLibrary_Driver/Internet/MQTT
SRC_DIR += ioLibrary_Driver/Internet/MQTT/MQTTPacket/src

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
