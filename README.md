# CoffeePot
Code for a ESP8266 based coffee machine. Schematics and pictures of the final result will follow soon.

## Project directory-structure

### coffeepot
Contains the PlatformIO project and the platformio.ini file. The platformio.ini should be adjusted for the board you are compiling for. The coffeepot folder contains the following folders:

#### data: 
Contains the minified and gzipped HTML, CSS and Javascript files. These are uploaded to the SPIFFS memory of the ESP8266.
#### lib: 
Empty folder which could be used for including libraries.
#### src: 
Contains the C++ logic for the ESP8266 microcontroller.

### html
Contains the original not minified and gzipped HTML, CSS and Javascript files.

### images
Contains the images used in the project. Which I uploaded to Onedrive for everyone to use and are included through the main.js Javascript file. Feel free to

## Getting started
1. Open the coffeepot folder in Atom with PlatformIO installed
2. Edit the platformio.ini to suit your needs
3. Update the IN and OUT pinvariables, DomoticzIP, SSID, WPA-KEY and the IPAddress
4. Build and Upload to the ESP8266
5. Upload the SPIFFS image through the button in the left corner of PlatformIO
6. Visit the IP address of the ESP8266 and enjoy :)


## Credits
This project was developed in the PlatformIO IDE which can be downloaded here:
http://platformio.org/platformio-ide

Most of the icons are property of https://www.iconsmind.com/
