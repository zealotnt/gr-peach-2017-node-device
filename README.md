# Hardware

![Top view](./hardware/esp-pow-asem-top.jpg)  
![Board schematic](./hardware/esp-power-schematic.png)  
![Smart node pinout](./hardware/smart-note-pinout.png)  
[Smart node](./hardware/smart-node.md)  

# Software

[Example code](./software/example-esp-pow.ino)  

- To be able to build the example
```bash
# Install arduino IDE
https://arduino.esp8266.vn/basic/install.html

# Download all the dependencies libraries
git clone https://bitbucket.org/xoseperez/hlw8012.git
git clone git@github.com:blynkkk/blynk-library.git
git clone git@github.com:tzapu/WiFiManager.git

# and link it to arduino library
cd ~/.arduino15/packages/esp8266/hardware/esp8266/2.3.0/libraries
ln -s <absolute-path-to-hlw8012> .
ln -s <absolute-path-to-blynk-library> .
ln -s <absolute-path-to-WiFiManager> .

# Now the example should be able to build with arduino IDE
```

[HLW8012](https://bitbucket.org/xoseperez/hlw8012)  

[Wifi Manager](https://github.com/tzapu/WiFiManager)  

# Distributor

[esp-pow](https://iotmaker.vn/esp-pow-thiet-bi-do-dien.html)  
[smart-node](https://iotmaker.vn/smartnode.html)

# Demo

[Link1](https://www.facebook.com/tuanpm.net/videos/1458801410810199/)
