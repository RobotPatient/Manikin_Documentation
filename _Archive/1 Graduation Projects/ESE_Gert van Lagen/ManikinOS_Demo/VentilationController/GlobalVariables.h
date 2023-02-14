/*!\addtogroup ManikinOS
  \file GlobalVariables.h
  \ingroup ManikinOS
  @{
  @brief Contains global variables used in the main file of ManikinOS VentilationController: VentilationController.ino.
  @author Gert van Lagen
*/

/// Task handles of measuring controller tasks
TaskHandle_t Handle_TaskGatherInput;        // Not a time slot task
TaskHandle_t Handle_TaskComputeResult;
TaskHandle_t Handle_TaskWriteToMain;
TaskHandle_t Handle_TaskSynchronizeModules;
TaskHandle_t Handle_TaskHeartbeat;
TaskHandle_t Handle_TaskSafeCheckpoint;
TaskHandle_t Handle_TaskFunctionalSafety;   // Not a time slot task
TaskHandle_t Handle_TaskCheckpointRecovery; // Not a time slot task
TaskHandle_t Handle_TaskMonitor;
TaskHandle_t Handle_TaskModuleSetup;        // Not a time slot task

/// Semaphore used to protect the serial port
SemaphoreHandle_t x_Serial_Semaphore;

/// Semaphore used to keep track of time slot users except for the system tick interrupt callback, which starts a new time slot. 
SemaphoreHandle_t semaphore_time_slot_users;

/// Index variables for two-dimensional time slot buffer which is holding TaskHandles of time slot attached tasks
int time_slot = 0;
int row = 0;

int demo_time_slot_counter = 0;
boolean demo_synchronisation = false;
int incomingByte = 0;

/// Indicates if first time slot is starting. Used in time slot system tick interrupt to perform some safety checks. Switched to false after first time slot on serial port
boolean start_time_slot = true;

/// Used to turn on and off respectively all debug information 
boolean show_debug = true;

/// Used to turn on and off respectively time slot notification, time slot semaphore take, global time and current time slot on serial port
boolean show_debug_job_execution = true;

/// Used to turn on and off monitor job debugging on serial port
boolean show_debug_monitor_job = true;

/// Used to test timeslot functionality with single character and minimal debug overhead
boolean show_test_timeslots = true;

/// Used to turn on and off global time debugging
boolean show_debug_global_time = true;

/// Struct containing global time variables
struct time_format {
  TickType_t day;
  TickType_t hour;
  TickType_t minute;
  TickType_t second;
  TickType_t millisecond;
  TickType_t milliseconds_total;
};

/// Global time object starting on time 00:00:00:00:00
time_format global_time = {0, 0, 0, 0, 0, 0};

/// Global time update variables
const TickType_t global_time_transfer_delay_ms = 10; /// '10' is a testvalue. @todo delay to be implemented
TickType_t new_global_time_in_ms  = 0; 
TickType_t delay_before_time_slot_schedule_start = 0; /// (number_of_modules-1)*global_time_transfer_delay_ms 

/// A boolean indicating the arrival of a new global time synchronisation message, used to indicate in tick hook if global time needs to be reset.
boolean new_global_time_update = false;

/// Manually set to true in order to pass by the synchronisation at system start up. Should be false when Smart Hub Controller/main module is synchronising the modules at start.
boolean start_time_slot_schedule = false;

/// Time slot buffer holding TaskHandles of time slot attached tasks. Changing buffer to new schedule is easily possible by changing defines of TIME_SLOT_ROWS and TIME_SLOT_ELEMENTS in settings.h and changing time slot initialisation at this location
TaskHandle_t *buffer_time_slot[TIME_SLOT_ROWS][TIME_SLOT_ELEMENTS] =
{
  //             0                            1                           2                       3                    4                     5                           6                                       7                                 8                                 9
  /*0*/{ &Handle_TaskComputeResult , &Handle_TaskWriteToMain  , &Handle_TaskWriteToMain , &Handle_TaskWriteToMain , NULL                   , NULL                   , NULL                         , &Handle_TaskHeartbeat       , &Handle_TaskSynchronizeModules       , &Handle_TaskSafeCheckpoint},
};

/// Task handle variables to store tasks attached to current and previous time slot. Initialised at first time slot, updated in callback at system tick interrupt 
TaskHandle_t previous_time_slot = *buffer_time_slot[0][0];
TaskHandle_t current_time_slot = *buffer_time_slot[0][0];
