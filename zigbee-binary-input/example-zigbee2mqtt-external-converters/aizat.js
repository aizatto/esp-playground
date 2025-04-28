// aizat.js
const m = require('zigbee-herdsman-converters/lib/modernExtend');

const definition = {
  zigbeeModel: ['esp32c6'],
  model: 'esp32c6',
  vendor: 'AIZAT',
  description: 'Automatically generated definition',
  extend: [
    m.binary({
      "name": "binary_input_10_10",
      "cluster": "genBinaryInput",
      "attribute": "presentValue",
      "reporting": { "attribute": "presentValue", "min": "MIN", "max": "MAX", "change": 1 },
      "valueOn": ["ON", 1],
      "valueOff": ["OFF", 0],
      "description": "Button 1",
      "access": "STATE_GET",
      // "endpointName": "10"
    })
  ],
};

module.exports = definition;