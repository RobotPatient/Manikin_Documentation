/*!\addtogroup ManikinOS
  \file Settings.h
  \ingroup ManikinOS
  @{
  @brief Universal and specific settings and defines of module properties
  @author Gert van Lagen
*/

/// Type Defines and Constants

#ifndef _SETTINGS_
#define _SETTINGS_

/// Module generic defines

// number of known modules, both plugged and unplugged
#define number_of_modules   5

// controller names used to recognize modules in transfers
#define smart_hub_controller_name             'm'
#define compression_controller_name           'c'     
#define ventilation_controller_name           'v' 
#define communication_controller_name         'u' 
#define bus_monitor_controller_name           'b' 

// controller indexes used to access them in module array 
#define smart_hub_controller_index         1
#define compression_controller_index       2     
#define ventilation_controller_index       3 
#define communication_controller_index     4
#define bus_monitor_controller_index       5

#define smart_hub_controller_priority      0
#define compression_controller_priority    0
#define ventilation_controller_priority    0
#define communication_controller_priority  0
#define bus_monitor_controller_priority    0

// i2c addresses and port numbers
#define smart_hub_controller_i2c_address        0x10
#define smart_hub_controller_i2c_port           1

#define compression_controller_i2c_address      0x20
#define compression_controller_i2c_port         2

#define ventilation_controller_i2c_address      0x30
#define ventilation_controller_i2c_port         3

#define communication_controller_i2c_address    0x40
#define communication_controller_i2c_port       4

#define bus_monitor_controller_i2c_address      0x50
#define bus_monitor_controller_i2c_port         5

#define Serial                                  Serial // Adafruit Samd51 Boards. Use SerialUSB for Sparkfun Samd51 Boards
#define serial_baudrate                         115200 // Dependent on used BLE-module

#define TIME_SLOT_DURATION_MS                   700
#define TIME_SLOT_ELEMENTS                      10
#define TIME_SLOT_ROWS                          1
#define SCHEDULE_DURATION_MS                    (TIME_SLOT_ROWS*TIME_SLOT_ELEMENTS*TIME_SLOT_DURATION_MS)
#define MAX_TIME_SLOT_USERS                     0// at monitoring at start of time slot, previous tasks should not have released semaphore and not have access to time slot
#define BLOCK_TIME_AWAITING_START_NOTIFICATION_MS         portMAX_DELAY
#define BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS     ( TickType_t ) 0

/// Module specific defines
#define NUMBER_OF_TASKS                         10

#define hb_led_pin                              13
#define hb_time_on_ms                           500
#define hb_time_off_ms                          100

#define test_pin                                9


#endif

//*****************************************************************
