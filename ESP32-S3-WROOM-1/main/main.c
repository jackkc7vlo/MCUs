/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "clock_hal.h"
#include "driver/ledc.h"

#include "drv_ili9341.h"
#include "drv_pcf8574a.h"
#if HAS_ST7735 == 1u
#include "drv_st7735.h"
#endif
#if HAS_ES8311 == 1u
#include "drv_es8311.h"
#endif
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_hal.h"
#include "hw_config.h"
#include "i2c_hal.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "spi_hal.h"
#include <driver/gpio.h>
#include <esp_log.h>
#include <spi_hal.h>
#include <stdio.h>
// static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
// #define BLINK_GPIO CONFIG_BLINK_GPIO

#if HAS_LED_STRIP == 1
#include "led_strip.h"
#define BLINK_GPIO CONFIG_BLINK_GPIO;

static led_strip_handle_t led_strip;
#endif // HAS_LED_STRIP

#define RED_LED_GPIO 50
#define GREEN_LED_GPIO 51
#define BLUE_LED_GPIO 52

#if HW_CONFIG_SPI == 1U
#if HAS_ILI9341 == 1U
#define PIN_NUM_RST 16

///*********************************************************** */
#define LCD_HOST SPI2_HOST

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

#endif // HAS_ILI9341
#endif // HW_CONFIG_SPI
///*********************************************************** */

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

//******************* APP MAIN ***********************************/
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

    example_ledc_init();
    //   Set duty to 50%
    // ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 2048));
    //   Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    led_strip_set_pixel(led_strip, 0, 16, 0, 0);
    led_strip_refresh(led_strip);
    // gpio_handle->set(gpio_handle, 53, true);
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
