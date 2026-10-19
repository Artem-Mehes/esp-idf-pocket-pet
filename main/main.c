#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_io_i2c(bus_handle, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .bits_per_pixel = 1,
    };

    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, true));

    static const uint8_t clear_screen[128 * 64 / 8] = {0};

    ESP_ERROR_CHECK(
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 128, 64, clear_screen));

    static const uint8_t face[8] = {
        0x00, 0x20, 0x46, 0x40, 0x40, 0x46, 0x20, 0x00,
    };

    static const uint8_t face_closed[8] = {
        0x00, 0x24, 0x44, 0x40, 0x40, 0x44, 0x24, 0x00,
    };

    static uint8_t face_large[64 * 64 / 8] = {0};

    const uint8_t *current_face = face;

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    while (true) {
        for (int row = 0; row < 8; row++) {
            for (int x = 0; x < 64; x++) {
                uint8_t pixel = (current_face[x / 8] >> row) & 1;
                face_large[row * 64 + x] = pixel ? 0xFF : 0x00;
            }
        }

        ESP_ERROR_CHECK(
            esp_lcd_panel_draw_bitmap(panel_handle, 32, 0, 96, 64, face_large));

        vTaskDelay(pdMS_TO_TICKS(2000));

        current_face = (current_face == face) ? face_closed : face;
    }
}
