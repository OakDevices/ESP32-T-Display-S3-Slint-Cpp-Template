#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch_cst816s.h"

#define LCD_H_RES 170
#define LCD_V_RES 320

#define LCD_PIN_BK_LIGHT 38 
#define LCD_BK_LIGHT_ON_LEVEL 1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL

esp_lcd_panel_handle_t initLCD();
esp_lcd_touch_handle_t initTouch();

#ifdef __cplusplus
}
#endif