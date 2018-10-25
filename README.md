LoRa ChisteraPi
=============

LoRa ChisteraPi is the source code for LoRa transmissions with ChisteraPi.

Products
-------
ChisteraPi + accessories LoRa : https://snootlab.com/shields-snootlab/1152-.html

Forums
-------
Visit our specific forum at :
http://forum.snootlab.com/viewforum.php?f=59

Topic
-------
To get more informations visit our topic at :
http://forum.snootlab.com/viewtopic.php?f=59&t=1512

Links
-------
The original Radiohead library is under GPLv2 license.

http://www.airspayce.com/mikem/arduino/RadioHead/

Modifications and examples come from Snootlab with GPLv2 license.

https://snootlab.com/72-03-iot-et-sans-fil

Howto start
------------------
Plug the ChisteraPi on your RaspberryPi.

Power on and connect into your RaspberryPi with SSH.

Activation of the SPI :

```bash
    sudo raspi-config
```

Then select "5 Interfacing Options" then "P4 SPI" and <Yes> (Tested on version 20180518).

The lora_chisterapi library need the external library : wiringpi.
If you don't have the library do :
```bash
    sudo apt-get update
    sudo apt-get install wiringpi
```

Clone the repository :

```bash
    git clone https://github.com/Snootlab/lora_chisterapi
```

Get into the repository and select the example :

```bash
    cd lora_chisterapi
    cp examples/sender.cpp src/main.cpp
            OR
    cp examples/receiver.cpp src/main.cpp 
```

Compile and execute the example :

```bash
    make
    sudo ./chisterapi
```

