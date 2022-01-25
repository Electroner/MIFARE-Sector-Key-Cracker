# MIFARE-Sector-Key-Cracker ![Generic badge](https://img.shields.io/badge/Version-1.0-brightgreen.svg) ![Generic badge](https://img.shields.io/github/last-commit/Electroner/MIFARE-Sector-Key-Cracker) [![](https://tokei.rs/b1/github/XAMPPRocky/tokei?category=code)](https://github.com/Electroner/IG)
Small Arduino Program to crack by brute force a sector key of a MIRAFE/RFID tag.

There are 3 version of the program.

## Cracker Data Logger & Serial
This version outputs the data on the serial and saves it on an SD card (Following the Wire.png Scheme). The downside of this is that it does not start working until the serial is started from a computer. 

## Cracker only SD
This version follows the same setup as the previous one but does not need a computer to function, it also includes the possibility of putting two LEDs on ping 5 and 6 that indicate the current status of the program. 
  
## Cracker only Serial
This version is only connecting the part of the NFC reader without SD. So it has to be connected and with the serial set on a PC necessarily. 

## Recursos Usados

-   [Codigo de Ejemplo y conexiones](https://programarfacil.com/blog/arduino-blog/lector-rfid-rc522-con-arduino/)
