#include <stdio.h>
#include <esp_err.h>
#include <slint-esp.h>
#include <optional>

#include "appwindow.h"
#include "hwInit.h"

extern "C" void app_main(void)
{
    esp_lcd_panel_handle_t panel_handle = initLCD();
    esp_lcd_touch_handle_t touch_handle = initTouch();

    /* Allocate a drawing buffer */
    static std::vector<slint::platform::Rgb565Pixel> buffer(LCD_H_RES * LCD_V_RES);

    /* Initialize Slint's ESP platform support*/
    slint_esp_init(slint::PhysicalSize({ LCD_H_RES, LCD_V_RES }), panel_handle, touch_handle, buffer);
    /* Instantiate the UI */
    auto ui = AppWindow::create();
    /* Show it on the screen and run the event loop */
    ui->run();
    while(1);
}
