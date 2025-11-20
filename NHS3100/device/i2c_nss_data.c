/*
 * i2c_nss_data.c
 *
 *  Created on: Oct 13, 2019
 *      Author: orine
 */

#include <reg_hal.h>
#include "i2c_nss.h"
#include "i2c_nss_data.h"
#include <stdlib.h> /*lint -e129*/
void i2c_hal_event_handler(const I2C_ID_T iic, const I2C_EVENT_T event);
/* I2C interfaces */
struct i2c_interface I2C0_DATA /*[I2C_NUM_INTERFACE]*/ = {NSS_I2C,
                                                          //                                                        CLOCK_PERIPHERAL_I2C0,
                                                          i2c_hal_event_handler,
                                                          //                                                        NULL,
                                                          NULL,
                                                          //                                                        NULL,
                                                          0};
