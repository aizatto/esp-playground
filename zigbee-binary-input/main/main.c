#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "main.h"
#include "aizat.h"

#include "binary_input.h"
#include "light_driver.h"
#include "third_party/switch_driver.h"

#include <esp_zigbee_type.h>
#include <esp_zigbee_cluster.h>
#include <zcl/esp_zigbee_zcl_basic.h>
#include <zcl/esp_zigbee_zcl_binary_input.h>
#include <zcl/esp_zigbee_zcl_ias_zone.h>
#include <zcl/zb_zcl_basic.h>

#include <zboss_api.h>

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE in idf.py menuconfig to compile light (End Device) source code.
#endif

static esp_err_t deferred_driver_init(void)
{
  static bool is_inited = false;
  if (!is_inited)
  {
    light_driver_init(true);
    ESP_RETURN_ON_FALSE(switch_driver_init(button_func_pair, PAIR_SIZE(button_func_pair), zb_buttons_handler), ESP_FAIL, TAG,
                        "Failed to initialize switch driver");
    is_inited = true;
  }

  return is_inited ? ESP_OK : ESP_FAIL;
}

static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask)
{
  ESP_RETURN_ON_FALSE(esp_zb_bdb_start_top_level_commissioning(mode_mask) == ESP_OK, , TAG, "Failed to start Zigbee commissioning");
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
  uint32_t *p_sg_p = signal_struct->p_app_signal;
  esp_err_t err_status = signal_struct->esp_err_status;
  esp_zb_app_signal_type_t sig_type = *p_sg_p;
  // esp_zb_zdo_signal_device_annce_params_t *dev_annce_params = NULL;

  ESP_LOGI(TAG, "Signal %d / 0x%x %s", sig_type, sig_type, esp_zb_zdo_signal_to_string(sig_type));

  switch (sig_type)
  {
  case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
    ESP_LOGI(TAG, "Initialize Zigbee stack");
    esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
    break;

  case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
  case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
    configure_binary_input_reporting();
    if (err_status == ESP_OK)
    {
      ESP_LOGI(TAG, "Deferred driver initialization %s", deferred_driver_init() ? "failed" : "successful");
      ESP_LOGI(TAG, "Device started up in %s factory-reset mode", esp_zb_bdb_is_factory_new() ? "" : "non");
      if (esp_zb_bdb_is_factory_new())
      {
        ESP_LOGI(TAG, "Start network steering");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
      }
      else
      {
        ESP_LOGI(TAG, "Device rebooted");

        light_blue();
        aizat_print_lqi();
      }
    }
    else
    {
      ESP_LOGE(TAG, "Failed to initialize Zigbee stack (status: %s)", esp_err_to_name(err_status));
    }
    break;

  case ESP_ZB_BDB_SIGNAL_STEERING:
    if (err_status == ESP_OK)
    {
      esp_zb_ieee_addr_t extended_pan_id;
      esp_zb_get_extended_pan_id(extended_pan_id);
      ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
               extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
               extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
               esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());

      light_green();
      aizat_print_lqi();
    }
    else
    {
      ESP_LOGI(TAG, "Network steering was not successful (status: %s)", esp_err_to_name(err_status));
      esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb, ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);

      // light_red();
      // vTaskDelay(pdMS_TO_TICKS(100));
      // light_driver_set_power(false);
    }
    break;

  default:
    ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
             esp_err_to_name(err_status));
    break;
  }
}

static esp_err_t zb_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message)
{
  esp_err_t ret = ESP_OK;
  // bool light_state = 0;

  ESP_RETURN_ON_FALSE(message, ESP_FAIL, TAG, "Empty message");
  ESP_RETURN_ON_FALSE(message->info.status == ESP_ZB_ZCL_STATUS_SUCCESS, ESP_ERR_INVALID_ARG, TAG, "Received message: error status(%d)",
                      message->info.status);
  ESP_LOGI(TAG, "Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), data size(%d)", message->info.dst_endpoint, message->info.cluster,
           message->attribute.id, message->attribute.data.size);
  // if (message->info.dst_endpoint == HA_ESP_LIGHT_ENDPOINT) {
  //     if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) {
  //         if (message->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) {
  //             light_state = message->attribute.data.value ? *(bool *)message->attribute.data.value : light_state;
  //             ESP_LOGI(TAG, "Light sets to %s", light_state ? "On" : "Off");
  //             light_driver_set_power(light_state);
  //         }
  //     }
  // }
  return ret;
}

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message)
{
  esp_err_t ret = ESP_OK;
  ESP_LOGW(TAG, "Receive Zigbee action(0x%x) callback", callback_id);

  switch (callback_id)
  {
  case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:
    ret = zb_attribute_handler((esp_zb_zcl_set_attr_value_message_t *)message);
    break;
  default:
    break;
  }
  return ret;
}

static void esp_zb_task(void *pvparameters)
{

  // https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/developing.html#enable-debug-mode-and-trace-logging
#if CONFIG_ESP_ZB_TRACE_ENABLE
  esp_zb_set_trace_level_mask(esp_zb_trace_level_critical, esp_zb_trace_subsystem_mac | esp_zb_trace_subsystem_app);
#endif

  // esp_err_t err_status;
  /* initialize zigbee stack */
  esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZC_CONFIG();
  esp_zb_init(&zb_nwk_cfg);

  // https://github.com/espressif/esp-zigbee-sdk/issues/363#issuecomment-2408832840
  // esp_zb_secur_network_min_join_lqi_set(0);

  // esp_zb_on_off_switch_cfg_t switch_cfg = esp_zb_default_on_off_switch_config();
  // esp_zb_ep_list_t *esp_zb_on_off_switch_ep = esp_zb_on_off_switch_ep_create(ha_onoff_switch_endpoint, &switch_cfg);
  // zcl_basic_manufacturer_info_t info = {
  //     .manufacturer_name = ESP_MANUFACTURER_NAME,
  //     .model_identifier = ESP_MODEL_IDENTIFIER,
  // };

  // esp_zcl_utility_add_ep_basic_manufacturer_info(esp_zb_on_off_switch_ep, ha_onoff_switch_endpoint, &info);
  // esp_zb_device_register(esp_zb_on_off_switch_ep);

  // configure the cluster
  esp_zb_cluster_list_t *esp_zb_cluster_list = esp_zb_zcl_cluster_list_create();

  // configure basic
  // esp_zb_basic_cluster_cfg_t *basic_cfg = {0};

  // esp_zb_zcl_cluster_t *basic_cluster = esp_zb_basic_cluster_create(basic_cfg);
  uint8_t zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
  uint8_t power_source_id = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE; // from zboss-lib

  // uint8_t power_source_id = 0x04;
  esp_zb_attribute_list_t *esp_zb_basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, ESP_MANUFACTURER_NAME);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, ESP_MODEL_IDENTIFIER);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &zcl_version);
  esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, &power_source_id);

  ESP_LOGI(TAG, "Add basic cluster");
  esp_zb_cluster_list_add_basic_cluster(esp_zb_cluster_list, esp_zb_basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  // analog input
  // https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/api-reference/zcl/esp_zigbee_zcl_analog_input.html
  // esp_zb_attribute_list_t *analog_input_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT);
  // esp_zb_cluster_list_add_analog_input_cluster(esp_zb_cluster_list, analog_input_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  // on off cluster
  // esp_zb_attribute_list_t *on_off_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);
  // esp_zb_cluster_list_add_on_off_cluster(esp_zb_cluster_list, on_off_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  // binary input cluster
  // https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/api-reference/zcl/esp_zigbee_zcl_binary_input.html
  esp_zb_attribute_list_t *binary_input_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT);
  esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_ACTIVE_TEXT_ID, "active");
  esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_DESCRIPTION_ID, "binary input description");
  esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_INACTIVE_TEXT_ID, "inactive");
  zb_bool_t binary_input_cluster_default_value = ZB_FALSE; // ZB_TRUE or ZB_FALSE
  esp_zb_binary_input_cluster_add_attr(binary_input_cluster, ESP_ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID, &binary_input_cluster_default_value);
  esp_zb_cluster_list_add_binary_input_cluster(esp_zb_cluster_list, binary_input_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  // ias zone
  // https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32/api-reference/zcl/esp_zigbee_zcl_ias_zone.html
  // compalins cannot read zone state
  // int ias_zone_state = ESP_ZB_ZCL_IAS_ZONE_ZONESTATE_ENROLLED;
  // uint16_t zone_type = ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_KEYPAD;

  // esp_zb_attribute_list_t *ias_zone_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE);
  // esp_zb_ias_zone_cluster_add_attr(ias_zone_cluster, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID, &ias_zone_state);
  // esp_zb_ias_zone_cluster_add_attr(ias_zone_cluster, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, &zone_type);
  // esp_zb_cluster_list_add_ias_zone_cluster(esp_zb_cluster_list, ias_zone_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  // esp_zb_attribute_list_t *on_off_switch_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG);
  // esp_zb_cluster_list_add_on_off_switch_config_cluster(esp_zb_cluster_list, on_off_switch_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  // configure multistate
  // esp_zb_multistate_value_cluster_cfg_t *multistate_config = {
  //   .present_value = 0,                 // Initial value (e.g., state 0)
  //   .status_flags = 0,                  // Optional, use 0 if you don’t need status flags
  //   // .description = "Mode Selector",     // Optional string descriptor
  // };

  // esp_zb_attribute_list_t *multistate_attr_list = esp_zb_multistate_value_cluster_create(multistate_config);
  // esp_zb_attribute_list_t *multistate_attr_list = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT);

  // static char multistate_desc[] = "multistate";
  // static char multistate_value[] = ""; // initial state as string
  // esp_zb_multistate_value_cluster_add_attr(multistate_attr_list, ESP_ZB_ZCL_ATTR_MULTI_VALUE_DESCRIPTION_ID , &multistate_desc);
  // esp_zb_multistate_value_cluster_add_attr(multistate_attr_list, ESP_ZB_ZCL_ATTR_MULTI_VALUE_PRESENT_VALUE_ID, &multistate_value);

  // err_status = esp_zb_cluster_add_manufacturer_attr(
  //   multistate_attr_list,
  //   esp_zb_cluster_list->cluster->cluster_id,
  //   1,
  //   EZP_ZB_ZCL_CLUSTER_NON_MANUFACTURER_SPECIFIC,
  //   ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING,
  //   ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE,
  //   &multistate_value
  // );

  // ESP_LOGI(TAG, "Add multistate cluster");
  // esp_zb_cluster_list_add_multistate_value_cluster(esp_zb_cluster_list, multistate_attr_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  // esp_zb_zcl_attr_location_info_t attr_info = {
  //   .endpoint_id = HA_ESP_MULTISTATE_ENDPOINT,
  //   .cluster_id = ESP_ZB_ZCL_CLUSTER_ID_MULTI_VALUE,
  //   .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
  //   .attr_id = ESP_ZB_ZCL_ATTR_MULTI_VALUE_PRESENT_VALUE_ID,
  // };
  // esp_zb_zcl_start_attr_reporting(attr_info);

  // esp_zb_cluster_list_add_multistate_cluster(esp_zb_cluster_list, esp_zb_multistate_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  ESP_LOGI(TAG, "Add endpoint");
  // configure the endpoint
  esp_zb_ep_list_t *esp_zb_ep = esp_zb_ep_list_create();

  esp_zb_endpoint_config_t endpoint_config = {
      .endpoint = HA_ESP_MULTISTATE_ENDPOINT,    // endpoint id
      .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID, // home automation profile
      .app_device_id = ESP_ZB_HA_TEST_DEVICE_ID};
  esp_zb_ep_list_add_ep(esp_zb_ep, esp_zb_cluster_list, endpoint_config);

  // add_binary_input_endpoint(esp_zb_ep);

  ESP_LOGI(TAG, "Add basic manufacturere info");
  // esp_zcl_utility_add_ep_basic_manufacturer_info(esp_zb_ep, HA_ESP_MULTISTATE_ENDPOINT, &info);
  esp_zb_device_register(esp_zb_ep);

  esp_zb_core_action_handler_register(zb_action_handler);
  esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);
  ESP_LOGI(TAG, "esp zigbee start 1");
  ESP_ERROR_CHECK(esp_zb_start(false));
  ESP_LOGI("ZB", "ESP Zigbee stack version: %s", esp_zb_get_version_string());
  ESP_LOGI(TAG, "esp zigbee start 2");
  esp_zb_stack_main_loop();
}

void app_main(void)
{
  esp_log_level_set("*", ESP_LOG_DEBUG);
  esp_log_level_set("esp_zb", ESP_LOG_DEBUG);
  esp_log_level_set("esp_zcl", ESP_LOG_DEBUG);
  esp_log_level_set("esp_ha", ESP_LOG_DEBUG);

  // esp_zb_set_trace_level_mask(ESP_ZB_TRACE_LEVEL_DEBUG,);
  // esp_zb_set_trace_mask(ESP_ZB_TRACE_MASK_STACK);

  ESP_LOGI(TAG, "Hello World!");

  esp_zb_platform_config_t config = {
      .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
      .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
  };
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_zb_platform_config(&config));

  xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
}
