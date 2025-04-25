#include <esp_zigbee_type.h>
#include "aizat.h"
#include "esp_log.h"
#include <nwk/esp_zigbee_nwk.h>

void aizat_print_lqi(void) {
  uint8_t ieee_addr[8];
  esp_zb_get_long_address(ieee_addr);
  ESP_LOGI("ZIGBEE", "IEEE Address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
    ieee_addr[0], ieee_addr[1], ieee_addr[2], ieee_addr[3],
    ieee_addr[4], ieee_addr[5], ieee_addr[6], ieee_addr[7]);

  uint16_t short_address = esp_zb_get_short_address();
  ESP_LOGI(TAG, "Short address: %d", short_address);

  uint8_t current_channel = esp_zb_get_current_channel();
  ESP_LOGI(TAG, "Current Channel: %d", current_channel);
}
