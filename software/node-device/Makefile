# GR-PEACH 2017 esp-pow makefile

# system dependancies
ESP_ROOT    := /home/zealot/Media-Sharing/bin/arduino-esp8266

# https://stackoverflow.com/questions/18136918/how-to-get-current-relative-directory-of-your-makefile
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
SKETCH      := node-device.ino

# Board parameters
BOARD       := esp8285
FLASH_DEF   := 1M64

########################################
# OTA parameters
########################################
# Red wire // room light
ESP_ADDR    ?= 192.168.31.232
OTA_ADDR    ?= 192.168.31.232
# Or
# White wire // balcony light
# ESP_ADDR    ?= 192.168.31.159
# OTA_ADDR    ?= 192.168.31.159

# OTA credential
ESP_PORT    ?= 8266
OTA_PORT    ?= 8266
ESP_PWD     ?= admin
OTA_PWD     ?= admin
########################################

# Include the makeEspArduino Makefile
include $(HOME)/workspace_mine/makeEspArduino/makeEspArduino.mk
