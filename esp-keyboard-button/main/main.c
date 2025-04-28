#include "keyboard_button.h"
#include "esp_log.h"

#define TAG "kbd"

static int row_gpios[] = {GPIO_NUM_10, GPIO_NUM_11};
static int col_gpios[] = {GPIO_NUM_2, GPIO_NUM_3};
static keyboard_btn_handle_t kbd_handle = NULL;

static void keyboard_single_key_cb(keyboard_btn_handle_t kbd_handle, keyboard_btn_report_t kbd_report, void *user_data)
{
    for (int i = 0; i < kbd_report.key_pressed_num; i++)
    {
        uint8_t row = kbd_report.key_data[i].output_index;
        uint8_t col = kbd_report.key_data[i].input_index;
        ESP_LOGI(TAG, "Key pressed: output_idx=%d, input_idx=%d", row, col);
    }
}

void app_main(void)
{
    keyboard_btn_config_t cfg = {
        .output_gpios = row_gpios,
        .output_gpio_num = sizeof(row_gpios) / sizeof(row_gpios[0]),
        .input_gpios = col_gpios,
        .input_gpio_num = sizeof(col_gpios) / sizeof(col_gpios[0]),
        .active_level = 0,
        .debounce_ticks = 2,
        .ticks_interval = 500,
        .enable_power_save = false,
    };

    keyboard_button_create(&cfg, &kbd_handle);

    keyboard_btn_cb_config_t cb_cfg = {
        .event = KBD_EVENT_PRESSED,
        .callback = keyboard_single_key_cb,
    };
    keyboard_button_register_cb(kbd_handle, cb_cfg, NULL);
}
