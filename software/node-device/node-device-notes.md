# Notes
- The firmware should be config like this in ArduinoIDE:
```
Tools -> Boards: "Generic ESP8285 Module"
Flash Size: 1M (64K SPIFFS)
CPU Frequency: "80 MHz"
Upload speed: "115200" # I found only this baudrate is stable for esp-pow device
```

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

# OTA

```bash
# Get espota tool
wget https://raw.githubusercontent.com/esp8266/Arduino/master/tools/espota.py
# Esp ota command to load firmware to esp-pow
python espota.py -d -i <ip-of-esp-pow> -p 8266 -P 8266 -a admin -f <path-to-ino-bin>
```
