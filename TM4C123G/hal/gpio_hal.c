/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       gpio_hal.c
 * @author     Jack Wilson
 * @brief      GPIO hardware abstraction layer
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-19-2023     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 *
 * ==============================================================================
 * @endverbatim
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*lint -esym(793,__*)*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> /*lint -e129*/
#include <gpio_hal.h>
#include <assert.h>
#include <reg_hal.h>
#include <hw_config.h>

    /********************************************************************************
     * Defines
     ********************************************************************************/

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    //<! Pointer to an incomplete type (hides implementation)
    typedef struct gpio_hal_def_s
    {
        uint32_t gpio_num;            /*!< index */
        uint32_t port;                /*!< port */
        uint32_t pin;                 /*!< pin */
        gpio_hal_mode_t mode;         /*!< mode */
        uint32_t pull;                /*!< pull */
        p_gpio_callback_t p_callback; /*!< GPIO IRQ callback */
    } gpio_hal_def_t;

    static gpio_hal_def_t *p_gpios[MAX_GPIO] = {NULL}; /*lint !e956*/
    /********************************************************************************
     * Functions
     ********************************************************************************/

    static uint32_t gpio_hal_get_free_id(void)
    {
        for (uint32_t i = 0u; i < MAX_GPIO; i++)
        {
            if (p_gpios[i] == NULL)
            {
                return i;
            }
        }
        return MAX_GPIO;
    }

    p_gpio_hal_def_t gpio_hal_create_device(const uint32_t gpio_port, const uint32_t gpio_pin,
                                            const gpio_hal_mode_t mode, const uint32_t pull,
                                            const p_gpio_callback_t p_callback)
    {
        const uint32_t gpio_num = gpio_hal_get_free_id();
        if (gpio_num >= MAX_GPIO)
        {
            return NULL;
        }

        const p_gpio_hal_def_t p_gpio = calloc(1u, sizeof(gpio_hal_def_t));
        if (p_gpio != NULL)
        {
            p_gpios[gpio_num] = p_gpio; // OK is null as it will just put that in the table
            p_gpio->gpio_num = gpio_num;
            p_gpio->port = gpio_port;
            p_gpio->pin = gpio_pin;
            p_gpio->mode = mode;
            p_gpio->pull = pull;
            p_gpio->p_callback = p_callback;
        }
        return p_gpio;
    }

    void gpio_hal_free(const p_gpio_hal_def_t p_gpio)
    {
        p_gpios[p_gpio->gpio_num] = NULL; // OK is null as it will just put that in the table
        if (p_gpio != NULL)
        {
            free(p_gpio);
        }
        else
        {
            assert(false); /*lint !e506 */
        }
    }

    p_gpio_hal_def_t gpio_hal_get(const uint32_t gpio_num)
    {
        if (gpio_num >= MAX_GPIO)
        {
            return NULL; /*lint !e527 */
        }
        return p_gpios[gpio_num];
    }

    static void gpio_hal_pin_mode_set(const p_gpio_hal_def_t p_gpio)
    {

        switch (p_gpio->mode)
        {
        case GPIO_HAL_MODE_INPUT:

            NSS_GPIO[p_gpio->port].DIR &= ~(1UL << p_gpio->pin);

            break;
        case GPIO_HAL_MODE_OUTPUT:
            NSS_GPIO[p_gpio->port].DIR |= (1UL << p_gpio->pin);
            break;
        case GPIO_HAL_MODE_ALTERNATE:
            // mode = _GPIO_P_MODEL_MODE0_PUSHPULLALT;
            // break;
        case GPIO_HAL_MODE_ANALOG:
            // not sure for now, so drop through
        default:
            assert(false); /*lint !e506 */
            break;
        }

        gpio_hal_set_state(p_gpio, (bool)false);
    }

    void gpio_hal_init(const p_gpio_hal_def_t p_gpio)
    {
        if (p_gpio != NULL)
        {
            gpio_hal_pin_mode_set(p_gpio);
        }
    }

    void gpio_hal_set_state(const p_gpio_hal_def_t p_gpio, const bool state)
    {
        NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin] = (uint32_t)state << p_gpio->pin;

    } /*lint !e818*/

    bool gpio_hal_get_state(const p_gpio_hal_def_t p_gpio)
    {
        return (NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin]) != 0u;
    } /*lint !e818*/

    void gpio_hal_toggle_state(const p_gpio_hal_def_t p_gpio)
    {
        NSS_GPIO[p_gpio->port].DATA[1u << p_gpio->pin] ^= (1u << p_gpio->pin);
    } /*lint !e818*/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
