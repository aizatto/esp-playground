# esp32 zigbee device

Goal:
Develop a macropad to be used with Home Assistant (HA) via [Zigbee2MQTT](http://zigbee2mqtt.io/)

This exposes the button as a binary input so that Home Assistant can detect changes.

References:

1. [Documentation](https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/index.html) 
2. [Examples](https://github.com/espressif/esp-zigbee-sdk/tree/main/examples)

Learnings:
1. Zigbee HA means Home Automation (and not Home Assistant)
2. The Zigbee clusters you set up in code, are not automagically configured by Zigbee2MQTT. You have to define your own [external converters](https://www.zigbee2mqtt.io/advanced/more/external_converters.html).
   1. See my `example-zigbee2mqtt-external-converters/aizat.js`