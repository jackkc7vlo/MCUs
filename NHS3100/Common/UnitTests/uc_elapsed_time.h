/*
********************************************************************************
*                       MODULE TO MEASURE EXECUTION TIME
********************************************************************************
*/

/*
********************************************************************************
*                MAXIMUM NUMBER OF ELAPSED TIME MEASUREMENT SECTIONS
********************************************************************************
*/

//#define  ELAPSED_TIME_MAX_SECTIONS  10

/*
********************************************************************************
*                             FUNCTION PROTOTYPES
********************************************************************************
*/
#include  <stdint.h>
#include <hw_config.h>

#if UNIT_TESTS == 1u
void  uc_elapsed_time_clr   (void);      // Clear measured values
void  uc_elapsed_time_init  (void);             // Module initialization
void  uc_elapsed_time_start (void);      // Start measurement
void  uc_elapsed_time_stop  (void);      // Stop  measurement
uint32_t  uc_elapsed_time_value  (void); // Get  measurement
#endif /* UNIT_TESTS */
