# To investigate esp8285 firmware

## To view text, bss size
```bash
~/bin/arduino-1.6.9/hardware/esp8266com/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-size esp-pow.ino.elf

# Result should be like this
   text    data     bss     dec     hex filename
 238741    3388   29808  271937   42641 esp-pow.ino.elf
```

## To create firmware map file
1. first, find the line of arduino IDE that link the object together, for example:

```bash
"~/bin/arduino-1.6.9/hardware/esp8266com/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc" -g -w -Os -nostdlib -Wl,--no-check-sections -u call_user_start -u _printf_float -u _scanf_float -Wl,-static "-L~/bin/arduino-1.6.9/hardware/esp8266com/esp8266/tools/sdk/lib" "-L~/bin/arduino-1.6.9/hardware/esp8266com/esp8266/tools/sdk/ld" "-L~/bin/arduino-1.6.9/hardware/esp8266com/esp8266/tools/sdk/libc/xtensa-lx106-elf/lib" "-Teagle.flash.1m512.ld" -Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,spi_flash_read  -o "/tmp/build063cfba14cc8c0bc5f46fc9c882a7a57.tmp/esp-pow.ino.elf" -Wl,--start-group "/tmp/build063cfba14cc8c0bc5f46fc9c882a7a57.tmp/sketch/esp-pow.ino.cpp.o" "/tmp/build063cfba14cc8c0bc5f46fc9c882a7a57.tmp/arduino.ar" -lhal -lphy -lpp -lnet80211 -llwip_gcc -lwpa -lcrypto -lmain -lwps -laxtls -lespnow -lsmartconfig -lmesh -lwpa2 -lstdc++ -lm -lc -lgcc -Wl,--end-group  "-L/tmp/build063cfba14cc8c0bc5f46fc9c882a7a57.tmp"
```

2. then add the ` -Wl,-Map=esp-pow.map` at the end of the link process, the map file will be created at the current folder

3. using the [amap tools](http://www.sikorskiy.net/prj/amap/) to quickly investigate the map file
