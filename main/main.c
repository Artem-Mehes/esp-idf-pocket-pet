#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_log.h"

#define DEVICE_ADDRESS 0x3C

i2c_master_bus_config_t bus_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = GPIO_NUM_8,
    .scl_io_num = GPIO_NUM_9,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

void app_main (void) {
    i2c_master_bus_handle_t bus_handle = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    esp_err_t result = i2c_master_probe(bus_handle, DEVICE_ADDRESS, 100);

    ESP_LOGI("pocket_pet", "I2C probe: %s", esp_err_to_name(result));

    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = DEVICE_ADDRESS,
        .scl_speed_hz = 100000,
        .control_phase_bytes = 1,
        .dc_bit_offset = 6,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
}
