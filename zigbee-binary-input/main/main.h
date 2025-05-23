#include "esp_zigbee_core.h"
#include "zcl_utility.h"

#include "esp_zigbee_endpoint.h"
#include "zcl/esp_zigbee_zcl_multistate_value.h"


/* Zigbee configuration */
#define MAX_CHILDREN 10                 /* the max amount of connected devices */
#define INSTALLCODE_POLICY_ENABLE false /* enable the install code policy for security */

#define ED_AGING_TIMEOUT ESP_ZB_ED_AGING_TIMEOUT_64MIN /* aging timeout of device */
#define ED_KEEP_ALIVE 3000                             /* 3000 millisecond */

// #define ESP_ZB_PRIMARY_CHANNEL_MASK (1l << 13) /* Zigbee primary channel mask use in the example */
#define ESP_ZB_PRIMARY_CHANNEL_MASK ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK /* Zigbee primary channel mask use in the example */

// Hex has to match the number of characters in your manufacturer name
#define ESP_MANUFACTURER_NAME "\x05""AIZAT"

// CONFIG_IDF_TARGET is taken from `sdkconfig` which can be configured via
// 1. idf.py set-target esp32c6
// 2. idf.py menuconfig
#define ESP_MODEL_IDENTIFIER "\x07"CONFIG_IDF_TARGET

#define HA_ESP_MULTISTATE_ENDPOINT 10
#define HA_ESP_BINARY_INPUT_ENDPOINT 11

#define ESP_ZB_ZC_CONFIG()                                \
{                                                               \
    .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,                       \
    .install_code_policy = INSTALLCODE_POLICY_ENABLE,           \
    .nwk_cfg.zed_cfg = {                                        \
        .ed_timeout = ED_AGING_TIMEOUT,                         \
        .keep_alive = ED_KEEP_ALIVE,                            \
    },                                                          \
}

#define ESP_ZB_DEFAULT_RADIO_CONFIG()                           \
{                                                           \
    .radio_mode = ZB_RADIO_MODE_NATIVE,                     \
}

#define ESP_ZB_DEFAULT_HOST_CONFIG()                            \
{                                                           \
    .host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE,   \
}
