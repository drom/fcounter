#include <stddef.h>

#include <metal/cpu.h>
#include <metal/led.h>
// #include <metal/button.h>
#include <metal/spi.h>
#include <metal/gpio.h>


#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C
#define COLMOD 0x3A
#define COLMOD16bit 0x05

int main() {
    struct metal_led *led0_red, *led0_green, *led0_blue;
    led0_red = metal_led_get_rgb("LD0", "red");
    led0_green = metal_led_get_rgb("LD0", "green");
    led0_blue = metal_led_get_rgb("LD0", "blue");

    // Enable each LED
    metal_led_enable(led0_red);
    metal_led_enable(led0_green);
    metal_led_enable(led0_blue);

    metal_led_on(led0_red);
    metal_led_on(led0_green);
    metal_led_off(led0_blue);

    struct metal_gpio *gpio;
    gpio = metal_gpio_get_device(0);

    // backlight, 100% brightness
    metal_gpio_enable_output(gpio, 0);
    metal_gpio_set_pin(gpio, 0, 1);

    // reset = enable
    metal_gpio_enable_output(gpio, 2);
    metal_gpio_set_pin(gpio, 2, 1);

    // data / cmd
    metal_gpio_enable_output(gpio, 1);
    metal_gpio_set_pin(gpio, 1, 0);

    struct metal_spi *spi;
    spi = metal_spi_get_device(1);

    metal_spi_init(spi, 100000);

    // Can't controll GPIOs ???
    metal_led_off(led0_red);
    metal_led_on(led0_green);
    metal_led_on(led0_blue);

    struct metal_spi_config config = {
        .protocol = METAL_SPI_SINGLE,
        .polarity = 0,
        .phase = 0,
        .little_endian = 0,
        .cs_active_high = 0,
        .csid = 0,
    };

    char tx_buf[4] = {0};
    char rx_buf[4] = {0};

    // TODO Wait?

    metal_gpio_set_pin(gpio, 1, 0); // dc = low
    metal_spi_transfer(spi, &config, 1, COLMOD, rx_buf);

    metal_gpio_set_pin(gpio, 1, 1); // dc = high
    metal_spi_transfer(spi, &config, 1, COLMOD16bit, rx_buf);

    //  set window
    //      column set
    metal_gpio_set_pin(gpio, 1, 0); // dc = low
    metal_spi_transfer(spi, &config, 1, CASET, rx_buf);

    metal_gpio_set_pin(gpio, 1, 1); // dc = high
    tx_buf[0] = 100 >> 8;   // x0
    tx_buf[1] = 100 & 0xFF;
    tx_buf[2] = 139 >> 8;   // x1
    tx_buf[3] = 139 & 0xFF;
    metal_spi_transfer(spi, &config, 4, tx_buf, rx_buf);

    //      row set
    metal_gpio_set_pin(gpio, 1, 0); // dc = low
    metal_spi_transfer(spi, &config, 1, RASET, rx_buf);

    metal_gpio_set_pin(gpio, 1, 1); // dc = high
    tx_buf[0] = 100 >> 8;   // y0
    tx_buf[1] = 100 & 0xFF;
    tx_buf[2] = 139 >> 8;   // y1
    tx_buf[3] = 139 & 0xFF;
    metal_spi_transfer(spi, &config, 4, tx_buf, rx_buf);

    //  send cmd
    metal_gpio_set_pin(gpio, 1, 0); // dc = low
    metal_spi_transfer(spi, &config, 1, RAMWR, rx_buf);

    metal_gpio_set_pin(gpio, 1, 1); // dc = high
    tx_buf[0] = 0xFF;   // color
    tx_buf[1] = 0xFF;
    for (int i = 0; i < (40 * 40); i++) {
        // ram write
        metal_spi_transfer(spi, &config, 2, tx_buf, rx_buf);
    }

    return 0;
}
