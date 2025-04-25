idf_component_register(
	SRC_DIRS "." "third_party" "${IDF_PATH}/examples/zigbee/common/zcl_utility/src"
  INCLUDE_DIRS "." "${IDF_PATH}/examples/zigbee/common/zcl_utility/include"
  PRIV_REQUIRES nvs_flash esp_driver_gpio esp_driver_uart ieee802154
)
