Copyright 2022 Matthew Thornton

# Description
The firmware for a cloud-enabled reverse-osmosis pump controller board and the services required to power
the online monitoring and configuration system. Primarily used for residential, this could easily be adapted
to a commercial system. The WebApp portion of this project currently runs successfully on a Raspberry Pi 3.

## Structure
* Reversis-Osmosis-Controller
** The firmware
* WebApp
** Collection of services and microservices facilitating the cloud-like monitoring and configuration for the controller

# Reverse-Osmosis-Controller
## Overview
The firmware is designed to be ran on a Particle Photon board, which is an ARM based Wi-Fi enabled microcontroller
with Arduino compatibility. The primary reason this was used was due to familiarity with the system, the embedded Wi-Fi
board, and out-of-the-box OTA Update features for firmware changes.
## Features
* Supports controlling the following through relay switches
** RO Pump -- Pumps water through an RO filter or back flushing the filter
** Inlet Valve -- Solonoid valve for pump and flush inlet
** Flush Valve -- Solonoid valve for reverse flushing the RO filter
* Supports modular plug-ins such as water level sensors when using a resovoir
** Float-Switch -- An on/off switch typically reserved for fail-safe level detection
** Ultra-Sonic -- Bounce off the top of the water level to read low levels within the resovoir
** Float-Meter -- Another sensor for reading water levels in the resovoir

# WebApp
** Overview
Dockerized group of services enabling the remote monitoring and configuration of the system.
## Features
* Custom configuration settings driven by firmware
* Feature-List of currently enabled features on firmware
* Wi-Fi signal of controller
* State change history
* Average time to fill resovoir
* Event log history
* Time of last Flush routine
* Time and reason of last system restart
* Version tracking
* Supports multiple controllers
