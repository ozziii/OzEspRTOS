***UNDER COSTRUCTION....***


First at all very thanks to Xose Pérez for your public code Espurna (https://github.com/xoseperez/espurna).

## OzEspX 

**OzEspRTOS** is a Firmware for **ESP32** based board. This Firmware provide to interface from hardware to **MQTT** protocol. Hardware is configurable in runtime, with **Plug-In** logic, in a simple **web UI**. Written for **Arduino IDE** and **PlatformIO**.

## Features

* Work on ESP32 base board
    * In ESP32 work with FreeRTOS lib
* Auto search of best Wifi network. if no network was found,it set an Access Point and periodically check for known networks
* Web GUI interface. You can...:
    * See generic board information.
    * Set Parameter (Username, Password, WIFI, IP, MQTT info, Eth ).
    * Set Plugin.
    * See boad LOG.
    * Restart Board.
    * Save and restore all parameter and plugin by json file.
    * Applu OTA update
    * Reset Board 

## Getting started
* Download or clone code to your PlatformIO
 * Compile and Upload 
 * Wait for Esp boot
 * Connect at new WIFI network SSID: "**ESPWIFI**" password: "**ESPWIFIESPWIFIESPWIFI**"  (You can change it in configuration.h)
 * In Your Bowser go to http://192.168.4.1
 * Edit Esp Parameter like **espname** , **User** , **Password** , **WIFI** parameter, **MQTT** parameter. When a paramenter has changed line become bold. When all parameter is ok you can update settings and reboot device.
 * Find your Esp's Ip address in your local network and connet.
 * Configure Plug-in
 * After all configuratin is ok save settings and reboot

         
## Plug-in
Plug-in represent a new element added to ESP. It can be hardware or software. Some example:  sensors, buttons, led, display, bell ...


   
   Initialization String [#]/[NAME]/[delay (ms)]/[Hardware Serial]
   
* **RF_Rx_Switch** ..... 
   
   Initialization String [#]/[NAME]/[sensor pin]/[switch pin]/[relay logic 1:0]/[valid key]
  
* **MORE PLUG-IN COMING SOON....**
