# esp32 zigbee device

Follow me as I figure out how to get zigbee working on an esp32-c6.

Disclaimers:
1. This is really bad code. It is frakenstein's spaghetti.
   1. There is a lot of commented code.
   2. Magic numbers should be constants
   3. Variables could be better named
2. Do not use this for production.
3. I'm not a C coder, and I'm buliding this based on the limited [documentation](https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/index.html) and [examples](https://github.com/espressif/esp-zigbee-sdk/tree/main/examples) available.
4. I hope this can be useful to others out there.

Goal:
To develop a macropad to be used with Home Assistant (HA) via [Zigbee2MQTT](http://zigbee2mqtt.io/)

This exposes the button as a binary input so that Home Assistant can detect changes.

Learnings:
1. Zigbee HA means Home Automation (and not Home Assistant)
2. The Zigbee clusters you set up in code, are not automagically configured by Zigbee2MQTT. You have to define your own [external converters](https://www.zigbee2mqtt.io/advanced/more/external_converters.html).
   1. See my `example-zigbee2mqtt-external-converters/aizat.js`