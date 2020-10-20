# EMCA 
## Air quality monitoring system
![](https://github.com/ivanmorenoj/emca/workflows/check-build/badge.svg)

***This program was developed by Ivan Moreno as part of his thesis project.***
The projet get the gas concentration of alphasense (CO,NO2,SO2) and aeroqual (O3), process and save in mariadb database and finally send these values over a LoRaWAN Network.

## Required hardware
 - Raspberry pi
 - BME280 sensor
 - Catwan USB Stick from electronic cats
 - Alphasense type B4 gas sensors (CO,NO2,SO2)
 - Aeroqual SM50 (O3)
 - ADC Differential MCP3424 module from ABE Electronics
 - Signal conditioner
 - DC - DC step down
 - 12V 24W power suply

## Installation
The process of installation is the next:
 - Go to release folder and choose de latest version
 - Execute the next command
```{r, engine='bash', count_lines}
sudo sh intall.sh 
```
 - Check if the program is working with the next command

```{r, engine='bash', count_lines}
systemctl status emca 
```
## Compile from source
Dependencies for raspbian (buster)
 - libconfig++-dev
 - libmysqlcppconn-dev
 - wiringpi
 - i2c-tools
 - libi2c-dev
 - build-essential
 - git

Build command
```{r, engine='bash', count_lines}
make 
```
The destination folder is `build/bin`

Install
```{r, engine='bash', count_lines}
sudo make install 
```

Also support remote copy, first you need to configure the rapberry pi host in `/etc/hosts` as `rpi` and configure ssh keys
```{r, engine='bash', count_lines}
make remote_copy 
```

## Build with QEMU in amd64 for arm32v7
If you prefer compile from amd64 to arm32v7 you can use docker and QEMU
```{r, engine='bash', count_lines}
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
```
Build command 
```{r, engine='bash', count_lines}
docker run --rm -it -v $(pwd):/workdir ivan28823/emcaworkspace
```

## Configuration file
The config file is saved in `/etc/emca/config.cfg`
### How to use
 - **bme280Adrres:** *Address of BME280 sensor*
 - **timeCfg**
    - **sampling:** *time of sampling in seg*
    - **restore:** *time of wait to begin measure*
    - **latestMeasure:** *time of latest measure (unix time)* ***Don't modify this***
 - **sqlDataBase**
    - **user:** *user of database*
    - **password:** *password of user*
    - **host:** *host of database*
    - **schema:** *Schema of database*

 - **gasSensors**
    - **alphasenseSensor**
        - **Sensor**
            - **SensorType:** *type of sensor (CO,NO2,SO2, etc), see include/commonConfig.h*
            - **Compensate:** *choose the specific sensor (B4 or A4), see include/commonConfig.h*
            - **Algorithm:** *choose the algorithm acording to sensor [1 - 4]* 
            - **we_electronic:** *working electrode electronic offset in vols, see delivery bag*
            - **we_total:** *working electrode total offset in volts, see delivery bag*
            - **ae_electronic:** *auxiliar electrode electronic offset in volts, see delivery bag*
            - **ae_total:** *auxiliar electrode total offset in volts, see delivery bag*
            - **sensitivity:** *sensitivity in volts, see delivery bag*
        - **Adc:** *2 objets per sensor, for WE and AE*
            - **address:** *address of mcp3424 chip*
            - **bitrate:** *resolution in bits 12, 14, 16 or 18*
            - **pga:** *programmable gain 1, 2, 4 or 8*
            - **convmode:** *conversion mode, continuos(1) or one shot(0)*
            - **channel:** *channel of integrated circuit 1 - 4*
        - **Operation**
            - **lifetime:** *sensor life in days*
            - **startuptime:** *first connection of the sensor in day/month/year*
            - **temperature:** *[min ,max] float array in °C* 
            - **pressure:** *[min ,max] float array in hPa*
            - **humidity:** *[min ,max] float array in %*
            - **ranges:** *[min ,max] float array in ppb*
    - **aeroqual** 
        - **Adc       :** *the same as alphasense sensor config* 
        - **Operation :** *the same as alphasense sensor config*
 - **LoRa**
    - **port:** *serial port of usb stick*
    - **txPower:** *transmission power 0 - 15*
    - **actMethod:** *Activation method ABP - OTAA*
    - **dataRate:** *Datarate SF[10 - 7]BW125*
    - **channel:** *Cahnnel CH[0 - 7] or MULTI*
    - **DevAddr:** *Device address [8 digits]*
    - **NwkSKey:** *Network Session Key [32 HEX digits order in msb]*
    - **AppSKey:** *Application Session Key [32 HEX digits order in msb]*
    - **frameCounter:** *Current frame conter,* ***Don't modify this***

## Logs
The logs are saved in `/var/log/emca/emca.log`, type the next command to see logs
```{r, engine='bash', count_lines}
cat /var/log/emca/emca.log | tail -n 20
```

## Images of prototype
![TST01](img/img01.jpg?raw=true)
![TST02](img/img02.jpg?raw=true)
![TST03](img/img03.jpg?raw=true)

MIT License

Copyright (c) 2019 Ivan Moreno

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
