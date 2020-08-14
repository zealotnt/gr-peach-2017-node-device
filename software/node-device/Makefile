# GR-PEACH 2017 esp-pow makefile

# https://stackoverflow.com/questions/18136918/how-to-get-current-relative-directory-of-your-makefile
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
SKETCH      := node-device.ino

# Board parameters
BOARD       := esp8285
FLASH_DEF   := 1M64

########################################
# OTA parameters
########################################
# Red wire
ESP_ADDR    ?= esp8266-8109ea.local
# Or
# White wire
# ESP_ADDR    ?= esp8266-810ab8.local
ESP_PORT    ?= 8266
ESP_PWD     ?= admin
########################################

# Include the makeEspArduino Makefile
include $(HOME)/workspace_mine/makeEspArduino/makeEspArduino.mk
