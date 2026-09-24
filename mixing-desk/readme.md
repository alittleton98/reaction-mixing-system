# Mixing Desk

The Reaction Mixing Desk is the primary user interface for the the Reaction Mixing System. It is designed for the sound designer and/or engineer to be able to control the sound engine throughout the mixing process. The mixing desk is the heart of the system.&#x20;

## Context

All of the MIDI assignments available and the general setup of the desk are context sensitive. The base context for the Control Surface is to use the PluginMixer from SSL360 and load 512 mono audio channles. Contexts for the Control Surface written in python and loaded by the Mixing Desk to assign commands to any given behavior that can be interpreted by the Desk. Context behavior is written in python and interpreted by the Desk.&#x20;

Scripting the mixing desk is considered Power User level customization and is not required. The Mixing Desk has preinstalled contexts.

### Scripting

Contexts for the Mixing Desk are written in python and loaded by the mixing desk. These are intended to expand behavior of the mixing desk to be more inline with what the user may need. The scripting for Mixing Desk contexts are how all configurations are designed so there are examples included with the package.&#x20;

## Signal Chain

The Mixing Desk's primary signal chain is similar to an analog Mixing Desk. It's base signal chain is

1. Input Source
2. Device Input Buffer
3. Channel Strip Plugin
4. Device Output Buffer

The channel layout of the desk is ultimately up to the user. A single fader on the desk can represent any number of contiguous channels. The Signal chain's general setup is established by the Context Script.

### ASIO

The primary audio device for the mixing desk is the VB-AUDIO 512 CHANNEL Virtual ASIO device from [VB Audio's Matrix Cocount](https://vb-audio.com/Matrix/coconut.htm). The reason the realtime audio matrix is used is because it provides a standardized set of of I/O channels and a ready made routing matrix with more flexibility and customization for the user. The Mixing Desk launches using the 512 channel Virtual ASIO device and defaults to a matrix that outputs downward to the inputs of the smaller channel Virtual ASIO devices.&#x20;

### Channel Strip

The Channel Strip setup runs the audio processing setup for the Mixing Desk. This is the host application for&#x20;

* ASIO 512 Channel Device
* SSL Channel Strip Plugins
* SSL Mastering Plugins

The Channel Strip model of the Mixing Desk is designed to run like an analog studio desk. The Master channels are at the end of the Channel List so the channels available always start at `0` and go up to `512 - Number of Channels for Master Bus` .&#x20;

#### Busses

Any number of busses can be created but they must fit within the 512 channels used by the audio device (minus the Master. It is the default and only required bus). Each Bus that's created will have it's input buffer loaded by its source channels. Busses do not load their own input buffer like regular channels do. Busses are similar to channels except the input source is any set of channels.

## Control Surface

The Control Surface for RMD is built around the Mackie Control Framework and allows for mapping standard MCU commands to the Mixing Desk's control scheme. The Control Surface configuration is designed to link and control software that that can benefit from an expanded control scheme from MCU commands. DAWs may not directly benefit from this expansion as they usually have their own built in mappings than be refined as needed. The most immediate use case are interactive experience authoring tools or other pieces of custom software.&#x20;
