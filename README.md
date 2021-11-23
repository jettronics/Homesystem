# Homesystem

## Introduction
The Homesystem project has been created to provide web radio stream for different rooms. Which can be controlled by Alexa, by Smart Phone and by a Remote controller centrally.
Further more the Homesystem acts as an Alarm center to monitor different sensors like water sensor to protect for water damage. In case of an Alarm the Homesystem can send Push Messages to a Smart Phone.

## Overview
The Homesystem centeral unit is realized by a Raspberry PI 3B+ controller module. 
The Sensor Alarm message connection is realized by a FM (433MHz) link between Sensor and Central unit.
Radio streaming can be controlled as mentioned above by different ways
* Alexa - The speech command for radio streaming is routed over Alexa to the Central unit. 
* Smart Phone - A browser interface is provided by NodeRED
* Remote Controller - A FM based controller to control the radio player on Central unit.

With the MPD (Music Player Daemon) player running on Central unit a list of radio streaming provides can be selected.  
For further technical details see Wiki page.
## Operation
The User can interact with the Homesystem over Alexa Speech Commands, Smart Phone Interface and Remote Controller.
### Alexa Speech Commands
Node RED uses amazon-echo-device-nodes to receive speech commands from Alexa. For Alexa those nodes are visible as a Light Bulb and can be speech controlled accordingly.
Following Speech Commands are available:
* Radio Küche
* Radio Bad
* Lieblingssender
* Was anderes
* Radio lauter
* Radio leiser

To activate webradio in kitchen say e.g. "Alexa, schalte Radio Küche ein!"
### Smart Phone Interface
There are two tabs as Interface to Homesystem.  
The first one is to control Webradio with different stations.  
  
![Webradio](/Images/WebradioScreenShot.png)  
  
The next tab shows the status of the different Watersensors.  
  
![Wateralarm](/Images/WateralarmScreenShot.png)  
  
Green LED means no water detected. Red LED means water detected. In case of Water detection a Push message will be sent to Smart Phone.
### Remote Controller
The Remote Controller is a small device to control Webradio by 4 buttons.
* On/Off Webradio
* Next Webradio station
* Volume up
* Volume down
  
### Water Detection
