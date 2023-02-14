  /*!@addtogroup ManikinOS
  @file TimeHelper.ino
  @ingroup ManikinOS
  @{
  @brief This file contains wrapper functions to manage task delays. 
  Can use these function for RTOS delays. 
  Takes into account processor speed. 
  Use these instead of delay(...) in rtos tasks. 
  Don't use the functions in the setup and the idle loop of FreeRTOS, it might cause unavailable serial connection to computer
  @author Scott Bristoe, creator of FreeRTOS library for SAMD51 and SAMD21 processors.
  @author Gert van Lagen
*/

//**************************************************************************
// Header files
//**************************************************************************

#include <FreeRTOS_SAMD51.h>

/*!
  @brief Delay specified microseconds from call
  @param [in] us number of desired microseconds to delay task
*/
void myDelayUs(int us)
{
  vTaskDelay( us / portTICK_PERIOD_US );
}

/*!
  @brief Delay specified milliseconds from call
  @param [in] ms number of desired milliseconds to delay task
*/
void myDelayMs(int ms)
{
  vTaskDelay( (ms * 1000) / portTICK_PERIOD_US );
}

/*!
  @brief Delay task until desired microseconds from previous time point
  @param [in] previousWakeTime time reference from where to delay task desired number of milliseconds
  @param [in] ms number of desired milliseconds to delay task
*/
void myDelayMsUntil(TickType_t *previousWakeTime, int ms)
{
  vTaskDelayUntil( previousWakeTime, (ms * 1000) / portTICK_PERIOD_US );
}
