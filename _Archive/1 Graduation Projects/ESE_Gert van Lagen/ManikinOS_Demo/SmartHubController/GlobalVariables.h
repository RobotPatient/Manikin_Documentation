/*!@addtogroup ManikinOS
  @file GlobalVariables.h
  @ingroup ManikinOS
  @{
  @brief Contains global variables used in the main file of ManikinOS SmartHubController: SmartHubController.ino.
  @author Gert van Lagen
*/

/// Task handles of MainController tasks
TaskHandle_t Handle_TaskRequestData;            // periodic
TaskHandle_t Handle_TaskDecideFeedback;         // periodic
TaskHandle_t Handle_TaskSendData;               // periodic
TaskHandle_t Handle_TaskSynchronizeModules;     // periodic
TaskHandle_t Handle_TaskSafeCheckpoint;         // periodic
TaskHandle_t Handle_TaskHeartbeat;              // periodic
TaskHandle_t Handle_TaskMonitor;                // periodic
TaskHandle_t Handle_TaskTimeSlotTimeLeft;       // aperiodic
TaskHandle_t Handle_TaskFunctionalSafety;       // aperiodic and sporadic
TaskHandle_t Handle_TaskCheckpointRecovery;     // aperiodic and sporadic
TaskHandle_t Handle_TaskModuleSetup;            // aperiodic and sporadic

/// Semaphore used to protect the serial port
SemaphoreHandle_t x_Serial_Semaphore;

/// Semaphore used to keep track of time slot users except for the system tick interrupt callback, which starts a new time slot. 
SemaphoreHandle_t semaphore_time_slot_users;

/// Semaphore used to protect the I2C-bus from usage by multiple tasks at the same time
SemaphoreHandle_t sempahore_inter_modulair_communication_bus;

/// Demonstrates synchronisation
int demo_time_slot_counter  = 0;
boolean demo_synchronisation = false;
int incomingByte = 0;

/// Index variables for two-dimensional time slot buffer which is holding TaskHandles of time slot attached tasks
int time_slot = 0;
int row = 0;

/// Indicates if first time slot is starting. Used in time slot system tick interrupt to perform some safety checks. Switched to false after first time slot on serial port
boolean start_time_slot = false;

/// Indicates if task module setup is notified at system start
boolean task_module_setup_is_notified = false;

/// Indicates if modules are synched at system start
boolean all_joined_modules_synched = false;

/// Used to turn on and off respectively all debug information 
boolean show_debug = true;

/// Used to turn on and off respectively time slot notification, time slot semaphore take, global time and current time slot on serial port
boolean show_debug_job_execution = false;

/// Used to turn on and off monitor job debugging on serial port
boolean show_debug_monitor_job = false;

/// Used to test timeslot functionality with single character and minimal debug overhead
boolean show_test_timeslots = false;

/// Used to turn on and off global time debugging
boolean show_debug_global_time = false;

/// Boolean used to enable plx-daq excel logging tool - please disable all serial monitor printing using the booleans show_debug_<> in this file
boolean using_plx_daq = false;

/// Struct containing global time variables
struct time_format {
  TickType_t day; // Uses 1 byte
  TickType_t hour; // Uses 1 byte
  TickType_t minute; // Uses 1 byte
  TickType_t second; // Uses 1 byte
  TickType_t millisecond; // Uses 2 bytes
  TickType_t milliseconds_total; // Uses 4 bytes
};

/// Global time object starting on time 00:00:00:00:00
time_format global_time = {0, 0, 0, 0, 0, 0};

/// Global time update variables
const TickType_t global_time_transfer_delay_ms = 1000; /// test value
TickType_t new_global_time_in_ms  = 0;
TickType_t delay_before_time_slot_schedule_start = number_of_modules*global_time_transfer_delay_ms; /// Gives an extra delay buffer, since smart hub controller is module as well, but does not need to receive global time

boolean start_time_slot_schedule = false;

/// Time slot buffer holding TaskHandles of time slot attached tasks. Changing buffer to new schedule is easily possible by changing defines of TIME_SLOT_ROWS and TIME_SLOT_ELEMENTS in settings.h and changing time slot initialisation at this location
TaskHandle_t *buffer_time_slot[TIME_SLOT_ROWS][TIME_SLOT_ELEMENTS] =
{
  //             0                            1                           2                       3                    4                     5                           6                                       7                                 8                                 9
  /*0*/{ NULL ,                &Handle_TaskRequestData  , &Handle_TaskRequestData , &Handle_TaskRequestData , &Handle_TaskDecideFeedback  , &Handle_TaskSendData  ,     NULL                , &Handle_TaskHeartbeat       , &Handle_TaskSynchronizeModules       , &Handle_TaskSafeCheckpoint        },
};

///// Time slot buffer holding TaskHandles of time slot attached tasks. Changing buffer to new schedule is easily possible by changing defines of TIME_SLOT_ROWS and TIME_SLOT_ELEMENTS in settings.h and changing time slot initialisation at this location
//TaskHandle_t *buffer_time_slot[TIME_SLOT_ROWS][TIME_SLOT_ELEMENTS] =
//{
//  //             0                            1                           2                       3                    4                     5                           6                                       7                                 8                                 9
//  /*0*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskChangeModeCPR        , &Handle_TaskSafeCheckpoint       , &Handle_TaskSynchronizeModules       , &Handle_TaskHeartbeat        },
//
//  /*1*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//
//  /*2*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//
//  /*3*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//
//  /*4*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//
//  /*5*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskChangeModeCPR        , &Handle_TaskSafeCheckpoint       , &Handle_TaskSynchronizeModules       , &Handle_TaskHeartbeat        },
//
//  /*6*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//
//  /*7*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//
//  /*8*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//
//  /*9*/{ &Handle_TaskEmptySlot , &Handle_TaskRequestData  , &Handle_TaskDecideFeedback , &Handle_TaskSendData , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot  , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot            , &Handle_TaskEmptySlot                , &Handle_TaskEmptySlot        },
//};

/// Task handle variables to store tasks attached to current and previous time slot. Initialised at first time slot, updated in callback at system tick interrupt 
TaskHandle_t previous_time_slot = *buffer_time_slot[0][0];
TaskHandle_t current_time_slot = *buffer_time_slot[0][0];
