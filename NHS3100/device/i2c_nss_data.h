/*
 * i2c_nss_data.h
 *
 *  Created on: Oct 13, 2019
 *      Author: orine
 */

#ifndef I2C_NSS_DATA_H_
#define I2C_NSS_DATA_H_

/* I2C common interface structure */
struct i2c_interface {
    NSS_I2C_T *ip; /* IP base address of the I2C device */
//    CLOCK_PERIPHERAL_T clk; /* Clock used by I2C */
    I2C_EVENTHANDLER_T mEvent; /* Current active Master event handler */
//    I2C_EVENTHANDLER_T sEvent; /* Slave transfer events */
    I2C_XFER_T *mXfer; /* Current active xfer pointer */
//    I2C_XFER_T *sXfer; /* Pointer to store xfer when bus is busy */
    uint32_t flags; /* Flags used by I2C master and slave */
};

#endif /* I2C_NSS_DATA_H_ */
