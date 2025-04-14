#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "main.h"

static const char *TAG = "ZIGBEE_MULTISTATE";

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
  // uint32_t *p_sg_p       = signal_struct->p_app_signal;
  // esp_err_t err_status = signal_struct->esp_err_status;
  // esp_zb_app_signal_type_t sig_type = *p_sg_p;
  // esp_zb_zdo_signal_device_annce_params_t *dev_annce_params = NULL;
  // switch (sig_type) {
  // }
}

static void esp_zb_task(void *pvParameters)
{
    /* initialize Zigbee stack */
    esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZC_CONFIG();
    esp_zb_init(&zb_nwk_cfg);
    // esp_zb_on_off_switch_cfg_t switch_cfg = ESP_ZB_DEFAULT_ON_OFF_SWITCH_CONFIG();
    // esp_zb_ep_list_t *esp_zb_on_off_switch_ep = esp_zb_on_off_switch_ep_create(HA_ONOFF_SWITCH_ENDPOINT, &switch_cfg);
    zcl_basic_manufacturer_info_t info = {
        .manufacturer_name = ESP_MANUFACTURER_NAME,
        .model_identifier = ESP_MODEL_IDENTIFIER,
    };

    // esp_zcl_utility_add_ep_basic_manufacturer_info(esp_zb_on_off_switch_ep, HA_ONOFF_SWITCH_ENDPOINT, &info);
    // esp_zb_device_register(esp_zb_on_off_switch_ep);
    esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);
    // ESP_ERROR_CHECK(esp_zb_start(false));
  ESP_LOGI(TAG, "esp zigbee start");
  esp_zb_stack_main_loop();
}

void app_main(void)
{
  esp_log_level_set("*", ESP_LOG_DEBUG);  
  ESP_LOGI(TAG, "Hello World!");

  esp_zb_platform_config_t config = {
    .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
    .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
  };
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_zb_platform_config(&config));

  xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
}
