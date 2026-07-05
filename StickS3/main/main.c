/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "driver/ledc.h"

#include "drv_ili9341.h"
#include "drv_pcf8574a.h"
#if HAS_ST7735 == 1u
#include "drv_st7735.h"
#endif
#if HAS_ST7789 == 1u
#include "drv_st7789.h"
#endif
#if HAS_ES8311 == 1u
#include "drv_es8311.h"
#endif
#include "clock_hal.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_hal.h"
#include "hw_config.h"
#include "i2c_hal.h"
#if HAS_LED_STRIP == 1
#include "led_strip.h"
#endif // HAS_LED_STRIP
#include "drv_button.h"
#include "drv_led.h"
#include "sdkconfig.h"
#include "spi_hal.h"
#include <driver/gpio.h>
#include <driver/i2c_master.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <spi_hal.h>
#include <stdio.h>
#include <timer_hal.h>

// Direct ESP-IDF display test (bypasses all HAL layers)
static void lcd_cmd(spi_device_handle_t spi, uint8_t cmd)
{
    gpio_set_level(45, 0); // DC=LOW for command
    spi_transaction_t t = {.length = 8, .tx_buffer = &cmd};
    spi_device_polling_transmit(spi, &t);
}

static void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    if (len == 0)
        return;
    gpio_set_level(45, 1); // DC=HIGH for data
    spi_transaction_t t = {.length = len * 8, .tx_buffer = data};
    spi_device_polling_transmit(spi, &t);
}

static void test_display_direct(void)
{
    ESP_LOGI("LCD_DIRECT", "Starting direct display test...");

    // 1. Enable M5PM1 L3B power rail via direct I2C
    i2c_master_bus_config_t bus_cfg = {
        .sda_io_num        = 47,
        .scl_io_num        = 48,
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .i2c_port          = 1, // Use I2C_NUM_1 (same as M5GFX)
        .glitch_ignore_cnt = 7,
    };
    bus_cfg.flags.enable_internal_pullup = true;
    i2c_master_bus_handle_t i2c_bus      = NULL;
    esp_err_t               err          = i2c_new_master_bus(&bus_cfg, &i2c_bus);
    if (err != ESP_OK)
    {
        ESP_LOGE("LCD_DIRECT", "I2C bus failed: %s", esp_err_to_name(err));
        return;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7, .device_address = 0x6E, .scl_speed_hz = 100000};
    i2c_master_dev_handle_t m5pm1 = NULL;
    i2c_master_bus_add_device(i2c_bus, &dev_cfg, &m5pm1);

    // Set M5PM1 GPIO2 = L3B_EN = HIGH (display power on)
    uint8_t reg_val = 0;
    uint8_t tx[2];
    // Reg 0x09: disable I2C idle sleep
    tx[0] = 0x09;
    tx[1] = 0x00;
    i2c_master_transmit(m5pm1, tx, 2, -1);
    // Reg 0x16: set GPIO2 as GPIO function (clear bit 2)
    i2c_master_transmit_receive(m5pm1, (uint8_t[]){0x16}, 1, &reg_val, 1, -1);
    tx[0] = 0x16;
    tx[1] = reg_val & ~0x04;
    i2c_master_transmit(m5pm1, tx, 2, -1);
    // Reg 0x10: set GPIO2 as output (set bit 2)
    i2c_master_transmit_receive(m5pm1, (uint8_t[]){0x10}, 1, &reg_val, 1, -1);
    tx[0] = 0x10;
    tx[1] = reg_val | 0x04;
    i2c_master_transmit(m5pm1, tx, 2, -1);
    // Reg 0x13: set GPIO2 push-pull (clear bit 2)
    i2c_master_transmit_receive(m5pm1, (uint8_t[]){0x13}, 1, &reg_val, 1, -1);
    tx[0] = 0x13;
    tx[1] = reg_val & ~0x04;
    i2c_master_transmit(m5pm1, tx, 2, -1);
    // Reg 0x11: drive GPIO2 HIGH (set bit 2)
    i2c_master_transmit_receive(m5pm1, (uint8_t[]){0x11}, 1, &reg_val, 1, -1);
    tx[0] = 0x11;
    tx[1] = reg_val | 0x04;
    i2c_master_transmit(m5pm1, tx, 2, -1);
    ESP_LOGI("LCD_DIRECT", "M5PM1 L3B enabled");
    vTaskDelay(pdMS_TO_TICKS(100));

    // 2. Configure GPIO pins
    gpio_config_t io = {.pin_bit_mask = (1ULL << 45) | (1ULL << 41) | (1ULL << 21) | (1ULL << 38),
                        .mode         = GPIO_MODE_OUTPUT};
    gpio_config(&io);
    gpio_set_level(38, 1); // Backlight on
    gpio_set_level(41, 1); // CS deselect
    gpio_set_level(45, 0); // DC low
    // Hardware reset
    gpio_set_level(21, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(21, 1);
    vTaskDelay(pdMS_TO_TICKS(150));

    // 3. Init SPI3 with MOSI=39, SCK=40
    spi_bus_config_t spi_bus = {.mosi_io_num     = 39,
                                .miso_io_num     = -1,
                                .sclk_io_num     = 40,
                                .quadwp_io_num   = -1,
                                .quadhd_io_num   = -1,
                                .max_transfer_sz = 65535};
    spi_bus_initialize(SPI2_HOST, &spi_bus, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t spi_dev = {
        .mode = 0, .clock_speed_hz = 10000000, .spics_io_num = -1, .queue_size = 7};
    spi_device_handle_t spi;
    spi_bus_add_device(SPI2_HOST, &spi_dev, &spi);
    ESP_LOGI("LCD_DIRECT", "SPI2 initialized at 40MHz");

    // 4. CS low, send init commands
    gpio_set_level(41, 0); // CS select

    lcd_cmd(spi, 0x01);
    vTaskDelay(pdMS_TO_TICKS(150)); // SWRESET
    lcd_cmd(spi, 0x11);
    vTaskDelay(pdMS_TO_TICKS(255)); // SLPOUT
    lcd_cmd(spi, 0x3A);
    lcd_data(spi, (uint8_t[]){0x55}, 1); // COLMOD 16bit
    lcd_cmd(spi, 0x36);
    lcd_data(spi, (uint8_t[]){0x00}, 1); // MADCTL
    lcd_cmd(spi, 0x21);                  // INVON
    lcd_cmd(spi, 0x13);                  // NORON
    lcd_cmd(spi, 0x29);
    vTaskDelay(pdMS_TO_TICKS(100)); // DISPON
    ESP_LOGI("LCD_DIRECT", "Init commands sent");

    // 5. Fill screen BLUE (RGB565: 0x001F)
    // CASET: cols 52..186
    lcd_cmd(spi, 0x2A);
    lcd_data(spi, (uint8_t[]){0x00, 0x34, 0x00, 0xBA}, 4);
    // RASET: rows 40..279
    lcd_cmd(spi, 0x2B);
    lcd_data(spi, (uint8_t[]){0x00, 0x28, 0x01, 0x17}, 4);
    // RAMWR + fill
    lcd_cmd(spi, 0x2C);
    gpio_set_level(45, 1); // DC=HIGH for pixel data
    uint8_t blue[2] = {0x00, 0x1F};
    // Send all 135*240 = 32400 pixels as a single DMA transfer
    static uint8_t DRAM_ATTR pixel_buf[135 * 240 * 2];
    for (int i = 0; i < 135 * 240 * 2; i += 2)
    {
        pixel_buf[i]     = 0x00;
        pixel_buf[i + 1] = 0x1F;
    }
    spi_transaction_t bulk = {.length = sizeof(pixel_buf) * 8, .tx_buffer = pixel_buf};
    spi_device_polling_transmit(spi, &bulk);
    (void)blue;

    gpio_set_level(41, 1); // CS deselect
    ESP_LOGI("LCD_DIRECT", "Blue fill done. Screen should be BLUE now.");
}
// static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
// #define BLINK_GPIO CONFIG_BLINK_GPIO
#if USE_LED_GPIO == 1U
static p_led_handle_t led_handle = NULL;
#endif
static p_timer_hal_t timer_handle = NULL;

void button_callback(button_state_t button_state)
{
    // button_handle_t p_button_handle = (button_handle_t)p_button;
    if (button_state == BUTTON_PRESSED)
    {
#if USE_LED_GPIO == 1U
        drv_led_toggle(led_handle);
#endif
    }
}

#if HAS_LED_STRIP == 1
#include "led_strip.h"
#define BLINK_GPIO CONFIG_BLINK_GPIO;

static led_strip_handle_t led_strip;
#endif // HAS_LED_STRIP

#define RED_LED_GPIO 50
#define GREEN_LED_GPIO 51
#define BLUE_LED_GPIO 52

#if HW_CONFIG_SPI == 1U
// for TFT display

static p_gpio_hal_t cs_gpio_handle    = NULL;
static p_gpio_hal_t reset_gpio_handle = NULL;
static p_gpio_hal_t dc_gpio_handle    = NULL;
static p_gpio_hal_t bckl_gpio_handle  = NULL;

#if HAS_ILI9341 == 1U
#define PIN_NUM_RST 16

///*********************************************************** */
#define LCD_HOST SPI2_HOST

#if IS_AIPI
#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK 12
#define PIN_NUM_CS 15

#define PIN_NUM_DC 2
// #define PIN_NUM_RST 16
#define PIN_NUM_BCKL 5
#define PARALLEL_LINES 16

#define LCD_BK_LIGHT_ON_LEVEL 0

typedef enum
{
    LCD_TYPE_ILI = 1,
    LCD_TYPE_ST,
    LCD_TYPE_MAX,
} type_lcd_t;

// This function is called (in irq context!) just before a transmission starts.
// It will set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

void lcd_cmd(spi_device_handle_t spi, const uint8_t cmd, bool keep_cs_active)
{
    esp_err_t         ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // Zero out the transaction
    t.length    = 8;          // Command is 8 bits
    t.tx_buffer = &cmd;       // The data is the cmd itself
    t.user      = (void *)0;  // D/C needs to be set to 0
    if (keep_cs_active)
    {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE; // Keep CS active after data transfer
    }
    ret = spi_device_polling_transmit(spi, &t); // Transmit!
    assert(ret == ESP_OK);                      // Should have had no issues.
}

uint32_t lcd_get_id(spi_device_handle_t spi)
{
    // When using SPI_TRANS_CS_KEEP_ACTIVE, bus must be locked/acquired
    spi_device_acquire_bus(spi, portMAX_DELAY);

    // get_id cmd
    lcd_cmd(spi, 0x04, true);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8 * 3;
    t.flags  = SPI_TRANS_USE_RXDATA;
    t.user   = (void *)1;

    esp_err_t ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);

    // Release bus
    spi_device_release_bus(spi);

    return *(uint32_t *)t.rx_data;
}

// Initialize the display
void lcd_init(spi_device_handle_t spi)
{
    // int cmd = 0;
    // const lcd_init_cmd_t *lcd_init_cmds;

    // Initialize non-SPI GPIOs
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask  = ((1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST) | (1ULL << PIN_NUM_BCKL));
    io_conf.mode          = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en    = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // detect LCD type
    uint32_t lcd_id            = lcd_get_id(spi);
    int      lcd_detected_type = 0;

    printf("LCD ID: %08" PRIx32 "\n", lcd_id);
    if (lcd_id == 0)
    {
        // zero, ili
        lcd_detected_type = LCD_TYPE_ILI;
        printf("ILI9341 detected.\n");
    }
    else
    {
        // none-zero, ST
        lcd_detected_type = LCD_TYPE_ST;
        printf("ST7789V detected.\n");
    }

    /*
        // Send all the commands
        while (lcd_init_cmds[cmd].databytes != 0xff) {
          lcd_cmd(spi, lcd_init_cmds[cmd].cmd, false);
          lcd_data(spi, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes &
      0x1F); if (lcd_init_cmds[cmd].databytes & 0x80) { vTaskDelay(100 /
      portTICK_PERIOD_MS);
          }
          cmd++;
        }
      */
    /// Enable backlight
    gpio_set_level(PIN_NUM_BCKL, LCD_BK_LIGHT_ON_LEVEL);
}
#else

// for TFT display
static p_gpio_hal_t cs_gpio_handle    = NULL;
static p_gpio_hal_t reset_gpio_handle = NULL;
static p_gpio_hal_t dc_gpio_handle    = NULL;
static p_gpio_hal_t bckl_gpio_handle  = NULL;

#endif // IS_AIPI

#endif // HAS_ILI9341
#endif // HW_CONFIG_SPI
///*********************************************************** */

#if HAS_LED_STRIP == 1
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO (3) // Define the output GPIO
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY (4096)                // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY (4000)

static void example_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {.speed_mode      = LEDC_MODE,
                                      .duty_resolution = LEDC_DUTY_RES,
                                      .timer_num       = LEDC_TIMER,
                                      .freq_hz         = LEDC_FREQUENCY, // Set output frequency at 4 kHz
                                      .clk_cfg         = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {.speed_mode = LEDC_MODE,
                                          .channel    = LEDC_CHANNEL,
                                          .timer_sel  = LEDC_TIMER,
                                          .intr_type  = LEDC_INTR_DISABLE,
                                          .gpio_num   = LEDC_OUTPUT_IO,
                                          .duty       = 0, // Set duty to 0%
                                          .hpoint     = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}
#endif // HAS_LED_STRIP
#if HAS_ES8311_I2S == 1u
codec_config_t codec_cfg = {
    //.input_device  = INPUT_DEVICE_ADC_MIC,
    //.output_device = OUTPUT_DEVICE_DAC_HEADPHONE,
    .i2s = {
        .mode = MODE_MASTER, .bits = BIT_LENGTH_16BITS, .channels = CHANNELS2, .rate = RATE_24K, .fmt = I2S_NORMAL
        //.signal_type = SIGNAL_TYPE_PCM,
    }};

extern const uint8_t music_pcm_start[] asm("_binary_canon_pcm_start");
extern const uint8_t music_pcm_end[] asm("_binary_canon_pcm_end");

static void i2s_music(void *args)
{
    esp_err_t ret         = ESP_OK;
    size_t    bytes_write = 0;
    uint8_t  *data_ptr    = (uint8_t *)music_pcm_start;

    /* (Optional) Disable TX channel and preload the data before enabling the TX
     * channel, so that the valid data can be transmitted immediately */
    ESP_ERROR_CHECK(i2s_channel_disable(tx_handle));
    ESP_ERROR_CHECK(i2s_channel_preload_data(tx_handle, data_ptr, music_pcm_end - data_ptr, &bytes_write));
    data_ptr += bytes_write; // Move forward the data pointer

    /* Enable the TX channel */
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
    while (1)
    {
        /* Write music to earphone */
        ret = i2s_channel_write(tx_handle, data_ptr, music_pcm_end - data_ptr, &bytes_write, portMAX_DELAY);
        if (ret != ESP_OK)
        {
            /* Since we set timeout to 'portMAX_DELAY' in 'i2s_channel_write'
               so you won't reach here unless you set other timeout value,
               if timeout detected, it means write operation failed. */
            ESP_LOGE(TAG, "[music] i2s write failed, %s", err_reason[ret == ESP_ERR_TIMEOUT]);
            abort();
        }
        if (bytes_write > 0)
        {
            ESP_LOGI(TAG, "[music] i2s music played, %d bytes are written.", bytes_write);
        }
        else
        {
            ESP_LOGE(TAG, "[music] i2s music play failed.");
            abort();
        }
        data_ptr = (uint8_t *)music_pcm_start;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
#endif // HAS_ES8311_I2S

//******************* APP MAIN ***********************************/

#if 0
void app_main(void)
{
#if HW_CONFIG_GPIO == 1
    /* Configure the peripheral according to the LED type */
    // configure_led();

    p_gpio_hal_t gpio_handle = gpio_hal_create(0);
    gpio_handle->init(gpio_handle);
#endif

#if HAS_LED_STRIP == 1
    led_strip_config_t strip_config = {
        .strip_gpio_num         = STRIP_GPIO_NUM,
        .max_leds               = 1,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .led_model              = LED_MODEL_WS2812,

    };
    /*
    led_strip_spi_config_t spi_config = {
        .spi_bus        = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip)); */
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);

    led_strip_clear(led_strip);
#endif // HAS_LED_STRIP

#if HW_CONFIG_SPI == 1

    void             *p_spi_bus    = spi_hal_initialize(SPI_DEVICE_1);
    spi_hal_device_t *p_spi_device = spi_hal_create_device(SPI_DEVICE_1, p_spi_bus, NULL);
#if HAS_ST7735 == 1
#if AUTO_CS == 1u
    drv_st7735_init(p_spi_device, NULL, gpio_handle, gpio_handle, gpio_handle);
#else
    drv_st7735_init(p_spi_device, gpio_handle, gpio_handle, gpio_handle, gpio_handle);
#endif // AUTO_CS
#if IS_AIPI == 0u
    drv_st7735_invert_colors(true);
#endif
#endif // HAS_ST7735

#endif // HW_CONFIG_SPI

#if HW_CONFIG_I2C == 1
    void    *p_i2c_handle   = i2c_hal_create_device(1u, NULL);
    uint32_t pcf8574a_state = 0xFFu;
    i2c_hal_initialize(100000u);
#if HAS_PCF8574A == 1u
    void *pcf8574a_device = drv_pcf8574a_init();
#endif // HAS_PCF8574A
#if HAS_ES8311 == 1u

    void *p_es8311_i2c_device = drv_es8311_codec_init(&codec_cfg, 0);
    gpio_handle->set(gpio_handle, ES8311_PA_ENABLE_GPIO, 1);
#endif // HAS_ES8311
#endif // HW_CONFIG_I2C

#if HW_CONFIG_SPI == 1U
#if HAS_ILI9341 == 1U
    // void             *p_spi_bus    = spi_hal_initialize(SPI_BUS_1);
    // spi_hal_device_t *p_spi_device = spi_hal_create_device(SPI_DEVICE_1, p_spi_bus, HW_CONFIG_SPI1_CS_PORT, NULL);

    // gpio_handle->set(gpio_handle, ILI9341_SPI_CS_PIN, 1);
    //  Reset the display
    gpio_handle->set(gpio_handle, ILI9341_SPI_RESET_PIN, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_handle->set(gpio_handle, ILI9341_SPI_RESET_PIN, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    uint8_t buffer[3] = {0x04};
    // gpio_handle->set(gpio_handle, ILI9341_SPI_CS_PIN, 1);
    p_spi_device->write(p_spi_device, buffer, 1);
    // gpio_handle->set(gpio_handle, ILI9341_SPI_CS_PIN, 1);

    // vTaskDelay(100 / portTICK_PERIOD_MS);
    // p_spi_device->write(p_spi_device, buffer, sizeof(buffer));
    p_spi_device->read(p_spi_device, buffer, sizeof(buffer));

    printf("LCD ID: %02X %02X %02X\n", buffer[0], buffer[1], buffer[2]);
    drv_ili9341_init(p_spi_device, gpio_handle, gpio_handle, gpio_handle, gpio_handle);
    // spi_hal_add_device(SPI_BUS_1, SPI_DEVICE_1, SPI1_BAUD_RATE, SPI1_QUEUE_SIZE);
    drv_ili9341_fill_screen(ILI9341_RED);

    clock_hal_delay(5u);

    drv_ili9341_fill_screen(ILI9341_GREEN);
    clock_hal_delay(5u);
    drv_ili9341_fill_screen(ILI9341_BLUE);
    clock_hal_delay(5u);
    drv_ili9341_set_rotation(3);

    // Refresh the screen to black background
    drv_ili9341_fill_screen(ILI9341_BLACK);
    clock_hal_delay(5);

    // Counting through all the bytes of those icons
    // uint64_t k = 0;
    // Draw border for the menu
    drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 10, 30, 310, 230);
    drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 310, 230, 10, 30);
    // Write something
    drv_ili9341_draw_string(10, 10, ILI9341_WHITE, ILI9341_BLACK, "Welcome!", 2);

    // Battery Icon in the top right corner
#endif // HAS_ILI9341
#endif // HW_CONFIG_SPI
#if HAS_LED_STRIP == 1
    example_ledc_init();
    //   Set duty to 50%
    // ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 2048));
    //   Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    led_strip_set_pixel(led_strip, 0, 16, 0, 0);
    led_strip_refresh(led_strip);
    // gpio_handle->set(gpio_handle, 53, true);
#endif // HAS_LED_STRIP
#if HAS_ST7735 == 1
    for (uint16_t x = 0; x < ST7735_WIDTH; x++)
    {
        for (uint16_t y = 0; y < ST7735_HEIGHT; y++)
        {
            uint16_t color = 0x001F; // pretty_effect_get_pixel(x, y);
            drv_st7735_draw_pixel(x, y, color);
        }
    }

    for (int x = 0; x < ST7735_WIDTH; x++)
    {
        drv_st7735_draw_pixel(x, 0, ST7735_BLUE);
        drv_st7735_draw_pixel(x, ST7735_HEIGHT - 1, ST7735_BLUE);
    }

#endif // HAS_ST7735
    // vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    while (1)
    {
#if HW_CONFIG_SPI == 1
#if HAS_ST7735 == 1
        drv_st7735_write_string(20, 20, "Hello World!", drv_font_7x10, ST7735_WHITE, ST7735_BLACK);
        clock_hal_delay(1500u);
        drv_st7735_fill_screen_fast(ST7735_GREEN);
        clock_hal_delay(500u);
        drv_st7735_fill_screen_fast(ST7735_BLUE);
        clock_hal_delay(500u);
        drv_st7735_fill_screen_fast(ST7735_BLACK);

#endif // HAS_ST7735
#endif //

#if HW_CONFIG_GPIO == 1
#if HAS_LED_STRIP == 1
        clock_hal_delay(500u);
        led_strip_set_pixel(led_strip, 0, 0, 16, 0);
        led_strip_refresh(led_strip);
        clock_hal_delay(500u);
        led_strip_set_pixel(led_strip, 0, 0, 0, 16);
        led_strip_refresh(led_strip);
        clock_hal_delay(500u);
        led_strip_set_pixel(led_strip, 0, 16, 0, 0);
        led_strip_refresh(led_strip);
#else
        gpio_handle->set(gpio_handle, 46, false);
#endif
#endif
#if HW_CONFIG_I2C == 1

#if HAS_PCF8574A == 1u
        pcf8574a_state ^= 0xFFu;
        uint32_t data = drv_pcf8574a_read(pcf8574a_device);
        ESP_LOGI(TAG, "PCF8574A read data: 0x%02X", data);
        // drv_pcf8574a_write(pcf8574a_device,
#endif // HAS_PCF8574A

#endif // HW_CONFIG_I2C
       //        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
       //     blink_led();
        /* Toggle the LED state */
        // s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
#else
void app_main(void)
{
    // Direct display test - bypass all HAL layers to confirm hardware works
    // test_display_direct();

    // Fetch total and free memory inside the dedicated PSRAM pool
    size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t free_psram  = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    printf("\n==================================\n");
    printf("   StickS3 PSRAM Verification\n");
    printf("==================================\n");
    printf("Total PSRAM detected: %d bytes (~%d MB)\n", total_psram, total_psram / (1024 * 1024));
    printf("Free PSRAM available: %d bytes\n", free_psram);
    printf("==================================\n\n");

    if (total_psram > 0)
    {
        printf(" SUCCESS: PSRAM is configured and active!\n");
    }
    else
    {
        printf(" FAILURE: PSRAM is not initialized or mapped.\n");
    }

    clock_hal_init();
    timer_handle = timer_hal_create(1000U, false, NULL);
#if HW_CONFIG_GPIO == 1 && USE_LED_GPIO == 1
    led_handle = drv_led_create(LED_PORT, LED_PIN);
    if (led_handle != NULL)
    {
        drv_led_init(led_handle);
    }
#endif // HW_CONFIG_GPIO AND USE_LED_GPIO
#if HW_CONFIG_GPIO == 1 && USE_BUTTON_GPIO == 1
    //    p_gpio_hal_t gpioc_handle = gpio_hal_create(GPIOC_PORT);
    //    if (gpioc_handle != NULL)
    //    {
    //        gpio_hal_init(gpioc_handle);
    //        gpio_hal_pin_direction(gpioc_handle, BUTTON_PIN, INPUT);
    //    }
    p_button_handle_t p_button_handle = drv_button_create(BUTTON_PORT, BUTTON_PIN, button_callback);
    if (p_button_handle != NULL)
    {
        drv_button_init(p_button_handle);
    }

#endif // HW_CONFIG_GPIO AND USE_BUTTON_GPIO
#if HW_CONFIG_SPI == 1
#if HAS_ILI9341 == 1u
    cs_gpio_handle = gpio_hal_create(ILI9341_SPI_CS_PORT);
    if (cs_gpio_handle != NULL)
    {
        ESP_LOGI("TAG", "Initializing CS GPIO");
        gpio_hal_init(cs_gpio_handle);
        gpio_hal_pin_direction(cs_gpio_handle, ILI9341_SPI_CS_PIN, PIN_DIRECTION_OUTPUT);
    }
    else
    {
        ESP_LOGE("TAG", "Failed to create CS GPIO");
    }
    reset_gpio_handle = gpio_hal_create(ILI9341_SPI_RESET_PORT);
    if (reset_gpio_handle != NULL)
    {
        ESP_LOGI("TAG", "Initializing RESET GPIO");
        gpio_hal_init(reset_gpio_handle);
        gpio_hal_pin_direction(reset_gpio_handle, ILI9341_SPI_RESET_PIN, PIN_DIRECTION_OUTPUT);
    }
    else
    {
        ESP_LOGE("TAG", "Failed to create RESET GPIO");
    }
    dc_gpio_handle = gpio_hal_create(ILI9341_SPI_DC_PORT);
    if (dc_gpio_handle != NULL)
    {
        ESP_LOGI("TAG", "Initializing DC GPIO");
        gpio_hal_init(dc_gpio_handle);
        gpio_hal_pin_direction(dc_gpio_handle, ILI9341_SPI_DC_PIN, PIN_DIRECTION_OUTPUT);
    }
    else
    {
        ESP_LOGE("TAG", "Failed to create DC GPIO");
    }
    bckl_gpio_handle = gpio_hal_create(ILI9341_SPI_BCKL_PORT);
    if (bckl_gpio_handle != NULL)
    {
        ESP_LOGI("TAG", "Initializing BCKL GPIO");
        gpio_hal_init(bckl_gpio_handle);
        gpio_hal_pin_direction(bckl_gpio_handle, ILI9341_SPI_BCKL_PIN, PIN_DIRECTION_OUTPUT);
    }
    else
    {
        ESP_LOGE("TAG", "Failed to create BCKL GPIO");
    }
    p_spi_hal_t spi_device_handle = spi_hal_create_device(SPI_BUS_1, NULL);
    if (spi_device_handle != NULL)
    {
        spi_hal_initialize();
        spi_hal_set_baud(spi_device_handle, SPI_HAL_BAUD_30MHz);
        spi_hal_enable(spi_device_handle, true);
        if (drv_ili9341_init(spi_device_handle, cs_gpio_handle, reset_gpio_handle, dc_gpio_handle, bckl_gpio_handle))
        {
            drv_ili9341_set_rotation(1);
            drv_ili9341_fill_screen(ILI9341_BLUE);
            drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 10, 30, 310, 230);
            drv_ili9341_draw_empty_rect(ILI9341_YELLOW, 310, 230, 10, 30);
            // Write something
            drv_ili9341_draw_string(10, 10, ILI9341_WHITE, ILI9341_BLACK, "Welcome!", 2);
        }
        else
        {
            ESP_LOGE("TAG", "Failed to initialize ILI9341 display");
        }
    }
    else
    {
        ESP_LOGE("TAG", "Failed to create SPI device");
    }
#endif

#if HAS_ST7789 == 1
    // -----------------------------------------------------------------------
    // M5Stack StickS3: The display's power rail (L3B) is controlled by the
    // M5PM1 power management chip at I2C address 0x6E. GPIO2 of the M5PM1
    // must be driven HIGH to enable the display before any SPI communication.
    // Without this step, the ST7789 panel has no power and ignores all data.
    // -----------------------------------------------------------------------
    p_i2c_hal_t i2c_handle = i2c_hal_create_device(0U, I2C1_SDA_PORT, I2C1_SDA_PIN, I2C1_SCL_PORT, I2C1_SCL_PIN, NULL);
    if (i2c_handle != NULL)
    {
#define M5PM1_ADDR 0x6EU
        uint8_t reg_val = 0u;

        // Disable I2C idle sleep mode (reg 0x09 = 0x00)
        reg_val = 0x00u;
        i2c_hal_write(i2c_handle, M5PM1_ADDR, 0x09u, 1u, &reg_val, 1u);

        // Set GPIO2 as GPIO function (clear bit 2 in reg 0x16)
        i2c_hal_read(i2c_handle, M5PM1_ADDR, 0x16u, 1u, &reg_val, 1u);
        reg_val &= ~(1u << 2u);
        i2c_hal_write(i2c_handle, M5PM1_ADDR, 0x16u, 1u, &reg_val, 1u);

        // Set GPIO2 as output (set bit 2 in reg 0x10)
        i2c_hal_read(i2c_handle, M5PM1_ADDR, 0x10u, 1u, &reg_val, 1u);
        reg_val |= (1u << 2u);
        i2c_hal_write(i2c_handle, M5PM1_ADDR, 0x10u, 1u, &reg_val, 1u);

        // Set GPIO2 push-pull mode (clear bit 2 in reg 0x13)
        i2c_hal_read(i2c_handle, M5PM1_ADDR, 0x13u, 1u, &reg_val, 1u);
        reg_val &= ~(1u << 2u);
        i2c_hal_write(i2c_handle, M5PM1_ADDR, 0x13u, 1u, &reg_val, 1u);

        // Drive GPIO2 HIGH = L3B_EN = LCD power ON (set bit 2 in reg 0x11)
        i2c_hal_read(i2c_handle, M5PM1_ADDR, 0x11u, 1u, &reg_val, 1u);
        reg_val |= (1u << 2u);
        i2c_hal_write(i2c_handle, M5PM1_ADDR, 0x11u, 1u, &reg_val, 1u);

        ESP_LOGI("TAG", "M5PM1: LCD power rail (L3B) enabled");
        clock_hal_delay(100u); // Wait for power rail to stabilise
    }
    else
    {
        ESP_LOGE("TAG", "M5PM1: failed to create I2C handle - display may not work");
    }

    cs_gpio_handle = gpio_hal_create(ST7789_SPI_CS_PORT);
    if (cs_gpio_handle != NULL)
    {
        gpio_hal_init(cs_gpio_handle);
        gpio_hal_pin_direction(cs_gpio_handle, ST7789_SPI_CS_PIN, PIN_DIRECTION_OUTPUT);
    }
    reset_gpio_handle = gpio_hal_create(ST7789_SPI_RESET_PORT);
    if (reset_gpio_handle != NULL)
    {
        gpio_hal_init(reset_gpio_handle);
        gpio_hal_pin_direction(reset_gpio_handle, ST7789_SPI_RESET_PIN, PIN_DIRECTION_OUTPUT);
    }
    dc_gpio_handle = gpio_hal_create(ST7789_SPI_DC_PORT);
    if (dc_gpio_handle != NULL)
    {
        gpio_hal_init(dc_gpio_handle);
        gpio_hal_pin_direction(dc_gpio_handle, ST7789_SPI_DC_PIN, PIN_DIRECTION_OUTPUT);
    }
    bckl_gpio_handle = gpio_hal_create(ST7789_SPI_BCKL_PORT);
    if (bckl_gpio_handle != NULL)
    {
        gpio_hal_init(bckl_gpio_handle);
        gpio_hal_pin_direction(bckl_gpio_handle, ST7789_SPI_BCKL_PIN, PIN_DIRECTION_OUTPUT);
    }
    p_spi_hal_t spi_device_handle = spi_hal_create_device(SPI_BUS_2, NULL); // SPI2: MOSI=39, SCK=40
    if (spi_device_handle != NULL)
    {
        spi_hal_initialize();
        spi_hal_set_baud(spi_device_handle, SPI_HAL_BAUD_1MHz);
        spi_hal_enable(spi_device_handle, true);
        if (drv_st7789_init(spi_device_handle, cs_gpio_handle, reset_gpio_handle, dc_gpio_handle, bckl_gpio_handle))
        {
            drv_st7789_invert_colors(true); // Required for M5Stack StickS3
            clock_hal_delay(100);

            // Simple pixel-by-pixel test
            for (int i = 0; i < 100; i++)
            {
                drv_st7789_draw_pixel(i, 10, ST7789_WHITE);
                drv_st7789_draw_pixel(i, 11, ST7789_RED);
                drv_st7789_draw_pixel(i, 12, ST7789_GREEN);
                drv_st7789_draw_pixel(i, 13, ST7789_BLUE);
            }

            clock_hal_delay(2000);

            // Test pattern to verify display is working
            drv_st7789_fill_screen(ST7789_RED);
            clock_hal_delay(500);
            drv_st7789_fill_screen(ST7789_GREEN);
            clock_hal_delay(500);
            drv_st7789_fill_screen(ST7789_BLUE);
            clock_hal_delay(500);

            // Draw some test rectangles
            drv_st7789_fill_screen(ST7789_BLACK);
            drv_st7789_fill_rectangle(10, 10, 50, 50, ST7789_RED);
            drv_st7789_fill_rectangle(70, 10, 50, 50, ST7789_GREEN);
            drv_st7789_fill_rectangle(10, 70, 50, 50, ST7789_BLUE);
            drv_st7789_fill_rectangle(70, 70, 50, 50, ST7789_YELLOW);
        }
    }
#endif // HAS_ST7789

#endif // HW_CONFIG_SPI

    timer_hal_enable(timer_handle, true);

    uint32_t last_found_address = 8U;

    while (last_found_address != 0U)
    {
        last_found_address =
            i2c_hal_scan(0U, I2C1_SDA_PORT, I2C1_SDA_PIN, I2C1_SCL_PORT, I2C1_SCL_PIN, last_found_address);
        if (last_found_address != 0U)
        {
            // Device found at last_found_address
            volatile uint32_t dummy = last_found_address; // Set breakpoint here to inspect detected device address
        }
    }

    while (1 == 1)
    {

        while (!timer_hal_get_overflow(timer_handle))
        {
            vTaskDelay(1); // Delay 1ms, feeds watchdog
        }
        timer_hal_reset_count(timer_handle);
        timer_hal_start(timer_handle);
        // clock_hal_delay(1000U);
        // drv_led_toggle(led_handle);

        // drv_button_is_pressed(p_button_handle);
        // bool pressed = drv_button_is_pressed(p_button_handle);
        // ESP_LOGI("TAG", "Button is %s", pressed ? "PRESSED" : "RELEASED");
        /*
       if (pressed)
       {
           gpio_hal_set_state(gpioa_handle, LED_PIN, true);
       }
       else
       {
           gpio_hal_set_state(gpioa_handle, LED_PIN, false);
       }
           */
    }
}
#endif
