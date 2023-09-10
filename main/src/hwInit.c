#include "hwInit.h"

#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

#define LCD_BPP 2
#define LCD_TARGET_REFRESH_RATE 60
#define LCD_PIXEL_CLOCK_HZ (LCD_H_RES * LCD_V_RES * LCD_BPP * LCD_TARGET_REFRESH_RATE)

#define LCD_PIN_DATA0 39    
#define LCD_PIN_DATA1 40    
#define LCD_PIN_DATA2 41    
#define LCD_PIN_DATA3 42    
#define LCD_PIN_DATA4 45    
#define LCD_PIN_DATA5 46    
#define LCD_PIN_DATA6 47    
#define LCD_PIN_DATA7 48    
#define LCD_PIN_PCLK 8     
#define LCD_PIN_CS 6     
#define LCD_PIN_RD 9         
#define LCD_PIN_DC 7        
#define LCD_PIN_RST 5       
#define LCD_PIN_POWER 15

// Bit number used to represent command and parameter
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

// Supported alignment: 16, 32, 64. A higher alignment can enables higher burst transfer size, thus a higher i80 bus throughput.
#define PSRAM_DATA_ALIGNMENT 32

#define LCD_TOUCH_SDA 18
#define LCD_TOUCH_SCL 17
#define LCD_TOUCH_RST 21
#define LCD_TOUCH_INT 16
#define LCD_TOUCH_I2C_CLK_SPEED_HZ 400000
#define LCD_TOUCH_I2C_NUM 0

static esp_lcd_panel_io_handle_t io_handle = NULL;

esp_lcd_panel_handle_t initLCD()
{
    gpio_config_t pwr_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_PIN_POWER};
    ESP_ERROR_CHECK(gpio_config(&pwr_gpio_config));
    gpio_set_level(LCD_PIN_POWER, LCD_BK_LIGHT_ON_LEVEL);

    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_PIN_BK_LIGHT};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(LCD_PIN_BK_LIGHT, LCD_BK_LIGHT_OFF_LEVEL);

    const gpio_config_t input_conf = {
        .pin_bit_mask =  1ULL << LCD_PIN_RD,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    ESP_ERROR_CHECK(gpio_config(&input_conf));

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .dc_gpio_num = LCD_PIN_DC,
        .wr_gpio_num = LCD_PIN_PCLK,
        .data_gpio_nums = {
            LCD_PIN_DATA0,
            LCD_PIN_DATA1,
            LCD_PIN_DATA2,
            LCD_PIN_DATA3,
            LCD_PIN_DATA4,
            LCD_PIN_DATA5,
            LCD_PIN_DATA6,
            LCD_PIN_DATA7,
        },
        .bus_width = 8,
        .max_transfer_bytes = LCD_H_RES * LCD_V_RES * sizeof(uint16_t),
        .psram_trans_align = PSRAM_DATA_ALIGNMENT,
        .sram_trans_align = 4,
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = LCD_PIN_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 20,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = 0,
        },
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_PIN_RST,
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 35, 0));

    gpio_set_level(LCD_PIN_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);

    return panel_handle;
}

esp_lcd_touch_handle_t initTouch()
{
    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = LCD_TOUCH_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = LCD_TOUCH_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = LCD_TOUCH_I2C_CLK_SPEED_HZ
    };
    ESP_ERROR_CHECK(i2c_param_config(LCD_TOUCH_I2C_NUM, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(LCD_TOUCH_I2C_NUM, i2c_conf.mode, 0, 0, 0));

    esp_lcd_panel_io_i2c_config_t io_config = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)LCD_TOUCH_I2C_NUM, &io_config, &io_handle));

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_H_RES,
        .y_max = LCD_V_RES,
        .rst_gpio_num = LCD_TOUCH_RST,
        .int_gpio_num = LCD_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
        .interrupt_callback = NULL,
    };

    esp_lcd_touch_handle_t tp;
    esp_lcd_touch_new_i2c_cst816s(io_handle, &tp_cfg, &tp);

    return tp;
}