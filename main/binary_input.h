// https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/api-reference/zcl/esp_zigbee_zcl_binary_input.html
// https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/api-reference/zcl/esp_zigbee_zcl_command.html

#include "aizat.h"

#include "esp_log.h"

#include <esp_zigbee_type.h>
#include <esp_zigbee_cluster.h>
#include <esp_zigbee_type.h>
#include "third_party/switch_driver.h"

#include <zboss_api.h>
#include <zcl/zb_zcl_basic.h>
#include <esp_zigbee_attribute.h>
#include <zcl/esp_zigbee_zcl_binary_input.h>
#include <zcl/esp_zigbee_zcl_command.h>

static switch_func_pair_t button_func_pair[] = {
    {GPIO_INPUT_IO_TOGGLE_SWITCH, SWITCH_ONOFF_TOGGLE_CONTROL, ZB_FALSE}};

static void zb_buttons_handler(switch_func_pair_t *button_func_pair)
{
  bool value = gpio_get_level(button_func_pair->pin) ? false : true;
  // bool level = gpio_get_level(button_func_pair->pin);
  // zb_bool_t zb_value = level ? ZB_TRUE : ZB_FALSE;
  // ESP_LOGI(TAG, "Button Press %d: %s", button_func_pair->pin, level ? "ON" : "OFf");
  if (button_func_pair->func == SWITCH_ONOFF_TOGGLE_CONTROL)
  {
    esp_err_t err;

    // 1. Set the attribute value
    esp_zb_lock_acquire(portMAX_DELAY);
    err = esp_zb_zcl_set_attribute_val(
        10,
        ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID,
        &value,
        false // Don't override ZCL internal flags
    );
    esp_zb_lock_release();
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "Failed to report attribute: %s", esp_err_to_name(err));
    }

    // 2. Report it to the coordinator
    esp_zb_zcl_report_attr_cmd_t report = {
        .zcl_basic_cmd.src_endpoint = 10,
        .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
        .attributeID = ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID,
        .clusterID = ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT,
        .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI,
        // .cluster_role = ZB_ZCL_CLUSTER_SERVER_ROLE,
        // .dst_endpoint = 1,
    };

    // esp_err_t err = esp_zb_zcl_report_attr_cmd_req(&report);
    esp_zb_lock_acquire(portMAX_DELAY);
    err = esp_zb_zcl_report_attr_cmd_req(&report);
    esp_zb_lock_release();
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "Failed to report attribute: %s", esp_err_to_name(err));
    }

    // esp_err_t err = esp_zb_cluster_update_attr(
    //     // 10,  // Endpoint ID
    //     // ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT,
    //     // attr_list
    //     ESP_ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID,
    //     &zb_value
    // );

    // TODO report updated attribute value

    /* implemented light switch toggle functionality */
    // esp_zb_zcl_on_off_cmd_t cmd_req;
    // cmd_req.zcl_basic_cmd.src_endpoint = HA_ESP_MULTISTATE_ENDPOINT;
    // cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
    // cmd_req.on_off_cmd_id = ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID;
    // esp_zb_lock_acquire(portMAX_DELAY);
    // esp_zb_zcl_on_off_cmd_req(&cmd_req);
    // esp_zb_lock_release();
    // ESP_EARLY_LOGI(TAG, "Send 'on_off toggle' command");
  }
}

void configure_binary_input_reporting(void)
{
  esp_zb_zcl_reporting_info_t reporting_info = {
      .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,     // Report to coordinator
      .ep = 10,                                         // Your endpoint (change if needed)
      .cluster_id = ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT, // Binary Input cluster
      .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,   // Device is server
      .dst.profile_id = ESP_ZB_AF_HA_PROFILE_ID,        // HA profile

      .u.send_info.min_interval = 0, // Immediate report on change
      .u.send_info.max_interval = 0, // Don't report periodically
      .u.send_info.delta.u8 = 1,     // Report when 0→1 or 1→0

      .attr_id = ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID,    // Attribute: presentValue
      .manuf_code = ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC, // Standard attr
  };

  esp_zb_zcl_update_reporting_info(&reporting_info);
}