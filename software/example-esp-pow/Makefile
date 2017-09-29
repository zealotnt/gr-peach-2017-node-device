# GR-PEACH 2017 esp-pow makefile

# https://stackoverflow.com/questions/18136918/how-to-get-current-relative-directory-of-your-makefile
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
SKETCH      := example-esp-pow.ino

# Board parameters
BOARD       := esp8285
FLASH_DEF   := 1M64

# OTA parameters
ESP_ADDR    ?= 192.168.1.178
ESP_PORT    ?= 8266
ESP_PWD     ?= iotmaker

# Include the makeEspArduino Makefile
include $(HOME)/workspace_esp/makeEspArduino/makeEspArduino.mk
