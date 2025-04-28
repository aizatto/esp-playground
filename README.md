# esp playground 

Follow me as I learn how to code for the ESP32.

Disclaimers:
1. This is really bad code. It is frakenstein's spaghetti.
   1. There is a lot of commented code.
   2. Magic numbers should be constants
   3. Variables could be better named
2. Do not use this for production.
3. I'm not a C coder.
4. I hope this can be useful to others out there.

## Projects

I try to create a working prototype in `Arduino` first, and then move on to `esp-idf`.

### arduino-keypad-advanced

A more advanced keypad example where a bit more feedback is given. LED colors rotate when idle, and change brightness when a button is pressed.

### [esp-keyboard-button](./esp-keyboard-button/)

Figuring out how to wire keyboard / keypad / button matrix.

### [zigbee-binary-input](./zigbee-binary-input/)

Basic example of getting zigbee working with esp32.

## References

1. [ESP Component Registry](https://components.espressif.com/)
1. [esp-idf](https://docs.espressif.com/projects/esp-idf)
1. [esp-iot-solution](https://docs.espressif.com/projects/esp-iot-solution/)
1. [esp-zigbee-sdk](https://docs.espressif.com/projects/esp-zigbee-sdk)