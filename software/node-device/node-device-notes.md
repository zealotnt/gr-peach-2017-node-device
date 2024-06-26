# First time flashing
- Wiring like this picture [Wiring_FTDI_esp-pow](../../hardware/serial-wiring-1.jpg).
- The `Green` bus will connect to GND, this will make ESP8285 go to firmware upgrade mode.
    + Reference for ESP8266/ESP8285 boot mode: [link-1](https://github.com/esp8266/esp8266-wiki/wiki/Boot-Process#esp-boot-modes), [link-2](https://github.com/esp8266/esp8266-wiki/wiki/Uploading)
    + The Espressif code can boot in different modes, selected on power-up based on GPIO pin levels. (MTDO is equivalent to GPIO15).

    | MTDO | GPIO0 | GPIO2 | Mode  | Description
    |:----:|:-----:|:-----:| ----- | -----------
    |   L  |   L   |   H   | UART  | Download code from UART
    |   L  |   H   |   H   | Flash | Boot from SPI Flash
    |   H  |   x   |   x   | SDIO  | Boot from SD-card

    + To upload to the module, configure the following pins:

    |  Pin   | Level | Description
    | ------ | ----- | -----------
    | CH_PD  | High  | Enables the chip
    | GPIO0  | Low   | Selects UART download
    | GPIO2  | High  | Selects UART download
    | GPIO15 | Low   | If availble. Selects UART download

- Open arduino `esp-pow` project. Change the settings like above (may be need to double check the settings, cause arduino project will remember last settings).
- Click `Upload` button to flash to board.

# mDNS
- To scan any node device using avahi in Ubuntu:

```bash
$ avahi-browse --all -v -r
```

- Result look like this

```
Server version: avahi 0.6.32-rc; Host name: Probook.local
E Ifce Prot Name                                          Type                 Domain
+   wlo1 IPv4 esp8266_8456682                               _arduino._tcp        local
=   wlo1 IPv4 esp8266_8456682                               _arduino._tcp        local
   hostname = [esp8266-8109ea.local]
   address = [192.168.1.122]
   port = [8266]
   txt = ["auth_upload=yes" "board=ESP8266_ESP01" "ssh_upload=no" "tcp_check=no"]
: All for now
: Cache exhausted
```

# Build and Flash using Makefile
- Helpers

```
make help # to show all options, incase we need other function, eg:
make erase_flash # erase all flash
make flash # flash using tty session
```

- To build the firmware

```
make
```

- To flash over OTA
    + The device must be same LAN as the Host machine
    + The settings in [Makefile](./Makefile) should be correct, especially: `ESP_ADDR` field.

```
make ota
```

# Behind the scene

### Build the firmware using `Arduino IDE`
- The firmware should be config like this in ArduinoIDE:
```
Tools -> Boards: "Generic ESP8285 Module"
Flash Size: 1M (64K SPIFFS)
CPU Frequency: "80 MHz"
Upload speed: "115200" # I found only this baudrate is stable for esp-pow device
```

### OTA using `espota` tool

```bash
# Get espota tool
wget https://raw.githubusercontent.com/esp8266/Arduino/master/tools/espota.py
# Esp ota command to load firmware to esp-pow
python espota.py -d -i <ip-of-esp-pow> -p 8266 -P 8266 -a admin -f <path-to-ino-bin>
```

# Debug exception of ESP8266 Arduino code
- Story: When I was working with wifiManager, and try to erase the wifi config of esp8266 (to force it goes to config mode when restart), it always cause exception, and the wifi config still there
- There is a easy way to debug exception, the ingredient:
    + Arduino IDE
    + [Exception debugger plugin](https://github.com/me-no-dev/EspExceptionDecoder)
    + espota.py
- Step by step:
    + Install stack exception debugger plugin (I'm using Arduino 1.6.13, older version may not work)
    + Open and build sketch.
    + If not using serial to download firmware, may use the espota to ugprade the firmware, with the syntax described above. The firmware is showed in console build log of Arduino IDE, like: `/tmp/arduino_build_849924/node-device.ino.bin`
        * May need enable: `File` -> `Preferences` -> `Show verbose output when compile`
