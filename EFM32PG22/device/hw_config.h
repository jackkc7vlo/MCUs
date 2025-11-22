/******************************************************************************
 * Copyright (C) 2025 by Jack Wilson
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
 *
 ************************(C) COPYRIGHT 2025 Jack Wilson **********************/
/**
 * @file      hw_config.h
 * @defgroup  features Controls the features available for a project
 * @author    Jack Wilson
 * @brief     #defines to determine what features are included in a project
 *
 * @details   Depending on the project various feature may or may not be
 *            available. Since the code is purposefully generic and may be
 *            part of a library, we want to be able to easily configure what
 *            features are a part of it.  The will be done with defines in
 *            the form of #define HAS_FEATURE_xxxx such as:
 *            #define HAS_FEATURE_DEBUG_UART 1 and then code that uses the
 *            feature would need to wrap call and include files with
 *            #if HAS_FEATURE_DEBUG_UART==1
 *               #include "debug_uart.h"
 *            #endif
 *
 *
 * @mod
 * @moditem{V1.0.x, Mar-08-2025, Jack Wilson, Initial Release, jackkc7vlo@gmail.com }
 * @modend
 *
*/
#include <stdint.h>
#include <stdbool.h>

#ifndef HW_CONFIG_H_
#define HW_CONFIG_H_

#define HW_CONFIG_GPIO       1



#define HW_CONFIG_M4_FPU     0
#define HW_CONFIG_DEBUG_UART 0
#define HW_CONFIG_TIMEBASE   0
#define HW_CONFIG_BSP        0
#define HW_CONFIG_ADC        0
#define HW_CONFIG_SPI        0
#define HW_CONFIG_I2C        0

#endif /* HW_CONFIG_H_ */
