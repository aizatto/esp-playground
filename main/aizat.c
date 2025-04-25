#include <esp_zigbee_type.h>
#include "aizat.h"
#include "esp_log.h"
#include <nwk/esp_zigbee_nwk.h>

void aizat_print_lqi(void) {
  // uint8_t lqi = 0;
  // esp_zb_ieee_addr_t parent_addr;
  // esp_zb_get_parent_info(&parent_addr, &lqi);

  // esp_zb_ieee_addr_t 
  uint8_t ieee_addr[8];
  esp_zb_get_long_address(ieee_addr);
  // ESP_LOGI(TAG, "Long address: 0x%llx", ieee_addr);
  ESP_LOGI("ZIGBEE", "IEEE Address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
    ieee_addr[0], ieee_addr[1], ieee_addr[2], ieee_addr[3],
    ieee_addr[4], ieee_addr[5], ieee_addr[6], ieee_addr[7]);

  uint16_t short_address = esp_zb_get_short_address();
  ESP_LOGI(TAG, "Short address: %d", short_address);

  uint8_t current_channel = esp_zb_get_current_channel();
  ESP_LOGI(TAG, "Current Channel: %d", current_channel);
  // ESP_LOGI(TAG, "My IEEE short address: 0x%llx", ieee_addr);
}

// static void add_binary_input_endpoint(esp_zb_ep_list_t *ep_list) {
//   esp_zb_endpoint_config_t endpoint_config = {
//     .endpoint = HA_ESP_BINARY_INPUT_ENDPOINT,                          // endpoint id
//     .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,  // home automation profile
//     .app_device_id = ESP_ZB_HA_TEST_DEVICE_ID
//   };

//   esp_zb_cluster_list_t *esp_zb_cluster_list = esp_zb_zcl_cluster_list_create();
//   esp_zb_attribute_list_t *binary_input_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT);
//   uint8_t binary_input_value = 1;
//   esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_ACTIVE_TEXT_ID, "active");
//   esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_DESCRIPTION_ID, "binary input description");
//   esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_INACTIVE_TEXT_ID, "inactive");
//   esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID, &binary_input_value);
//   esp_zb_cluster_list_add_binary_input_cluster(esp_zb_cluster_list, binary_input_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

//   esp_zb_ep_list_add_ep(ep_list, esp_zb_cluster_list, endpoint_config);
// }