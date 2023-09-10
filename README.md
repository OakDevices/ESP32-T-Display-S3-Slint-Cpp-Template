# ESP32 T-Display-S3 Slint C++ Template

A template for a MCU ESP32 T-Display-S3 application that's using [ESP-IDF](https://github.com/espressif/esp-idf) and [Slint](https://slint-ui.com) for the user interface.

## About

This template helps you get started developing a MCU C++ application with Slint as toolkit for the user interface.
It implements basics HW initialization code using ESP-IDF drivers for LCD (ST7789) and Touch (CST816) which is allowing to quickly start creating and displaying a `.slint` design on the screen.

## Prerequisites

 - Download and install [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) or use [Eclipse](https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md) or [VSCode](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md) plugin.
 - Download and install [Rust](https://esp-rs.github.io/book/installation/rust.html) and configure [ESP-RS](https://esp-rs.github.io/book/installation/riscv-and-xtensa.html) toolchain.

## Usage

Project is fully configured so simply clone this repo and run following command:
```
idf.py build
```

Note: Make sure that you have idf.py in your path, if not then follow [this](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html#step-4-set-up-the-environment-variables) steps.

To flash the board run:
```
idf.py flash -p <serial_port>
```

