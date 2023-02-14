/**@addtogroup ManikinOS
  @file SmartHubController.ino
  @ingroup ManikinOS
  @{
  @brief This is the main file of the smart hub controller of the ManikinOS. 
  Manages input from measuring modules, selects output based on user defined system protocols and priorities. 
  Finally passes the selected information through to the communication module.
  @author Scott Briscoe: FreeRTOS functionality for SAMD51.
  @author Gert van Lagen Scheduling, synchronisation, task and timeslot monitoring functionality
*/

/// Header files
#include <FreeRTOS_SAMD51.h>
#include <i2cdetect.h>
#include "Settings.h"
#include "GlobalVariables.h"

/*!
  @brief Runs when no time slot task uses CPU in a time slot.
  @param [in]
  @param [out]
  @note
  @todo Implement task functionality
*/
static void taskTimeSlotTimeLeft( void *pvParameters ) {
  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;

  /// Run while loop forever
  while (1){
    //if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}

    task_start = xTaskGetTickCount();
    /// Do task job
    if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}

    if(show_debug_job_execution){Serial.println("TaskGatherInput");}
    //myDelayMs(500);
    
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  //}
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Request data from measuring modules.
  @param [in]
  @param [out]
  @note
  @todo Implement task functionality
*/
static void taskRequestData( void *pvParameters )
{
  // PSEUDO CODE
  // while inf(
  //  Timeslot arrived, got signal from ISR which manages the start and ends of the timeslots
  //  Send request to module for its data
  //  Read data when available
  //  Send data to feedback decision task
  //  signal when finished within current time slot
  //  Wait for new notification with max delay
  // END while

  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;

  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  /// Task blocking time until time slot semaphore can be taken
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;

  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;
  
  /// Bytes received on serial line
  byte incomingByte1 = 0xAA;
  byte incomingByte2 = 0xAA;
  
  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        task_start = xTaskGetTickCount();
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    } 
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Feedbackmechanism. Checks if deadline can be met and decides which feedback to provide.
  @param [in]
  @param [out]
  @note
  @todo Implement task functionality
*/
static void taskDecideFeedback( void *pvParameters ) {
  // PSEUDO CODE
  // Receive data from taskRequestData
  // Check for feedback requirements/priorities
  // Decide weather to give feedback or not
  // Make feedback type
  // Send decision to taskSendData
  // signal when finished within current time slot

  
  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
   if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        task_start = xTaskGetTickCount();
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        incomingByte = Serial.read();
        if (incomingByte == 'd'){
          Serial.println("DELAY decideFeedback!");
          myDelayMs(1500);
        }
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Sends data to Communication Controller using the I2C-bus.
  @param [in]
  @param [out]
  @note
  @todo Implement task functionality.
*/
static void taskSendData( void *pvParameters ) {
  // PSEUDO CODE
  // Read feedback decision
  // Write feedback decision with I2C
  // signal when finished within current time slot

  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
//        digitalWrite(test_pin, HIGH);
//        myDelayUs(200);
//        digitalWrite(test_pin, LOW);
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        task_start = xTaskGetTickCount();
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Synchronizes periodically other modules to time of this module.
  @param [in]
  @param [out]
  @note
  @todo Implement task functionality
*/
static void taskSynchronizeModules( void *pvParameters ) {
  // PSEUDO CODE
  // Send periodic pulse to all modules (pulse length = counter*TimeSlotIntervalInMS)
  // Increment counter

  
  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        
        task_start = xTaskGetTickCount();
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Receives periodic life sign messages of other modules on I2C-request. Executes a watchdog timer
  @param [in]
  @param [out]
  @note
  @todo Implement task functionality
*/
static void taskHeartBeat( void *pvParameters )
{
  // PSEUDO CODE
  // Send periodical message to all available modules
  // Wait for response of other modules to be sure they are alive
  // No response -> User screen update + checkpoint recovery
  // Response -> continue time slot design
  // signal when finished within current time slot

  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Run while loop forever
  while (1){
    task_start = xTaskGetTickCount();
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
//        digitalWrite(test_pin, HIGH);
//        myDelayUs(200);
//        digitalWrite(test_pin, LOW);
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Safes safe state and BLS-data of this specific module to local non-volatile FRAM.
  @param [in]
  @param [out]
  @note
  @todo Implement task functionality.
*/
static void taskSafeCheckpoint( void *pvParameters ) {
  // PSEUDO CODE
  // Check if system state is safe
  // Determine important parameters and data of module
  // Store important parameters and data in modulegeneric (JSON?) object
  // Safe JSON on non-volatile FLASH memory
  // signal when finished within current time slot

  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
//        digitalWrite(test_pin, HIGH);
//        myDelayUs(200);
//        digitalWrite(test_pin, LOW);
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        
        task_start = xTaskGetTickCount();
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Recovers module to last safed safe module state after failure. 
  @param [in]
  @param [out]
  @note Should run when called by failure detecting tasks like the monitor task.
  @todo Implement task functionality.
*/
static void taskCheckpointRecovery( void *pvParameters ) {
  // PSEUDO CODE
  // Send to absent module watchdog trigger, so it will restart
  // Inform user about restart of module(s) and ask if it wants to restart session or to recover from checkpoint
  // Send to all modules command checkpoint recovery
  // Load (JSON)object with checkpoint data from last safed safe state, so all module state align in time again
  // signal when finished within current time slot

  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        task_start = xTaskGetTickCount();
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Functional safety task with failure countermeasure. 
  @param [in]
  @param [out]
  @note Should run when called by failure detecting tasks like the monitor task.
  @todo Implement task functionality.
*/
static void taskFunctionalSafety( void *pvParameters ) {
  // PSEUDO CODE
  // Send to absent module watchdog trigger, so it will restart
  // Inform user about restart of module(s) and ask if it wants to restart session or to recover from checkpoint
  // Send to all modules command checkpoint recovery
  // Load (JSON)object with checkpoint data from last safed safe state, so all module state align in time again
  // signal when finished within current time slot
  
  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      /// Take semaphore, if it is not available, block task for duration of specified ticks
      if( xSemaphoreTake( semaphore_time_slot_users, block_time_semaphore_take ) == pdTRUE )
      {  
//        digitalWrite(test_pin, HIGH);
//        myDelayUs(400);
//        digitalWrite(test_pin, LOW);
        if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
        
        task_start = xTaskGetTickCount();
        /// Do task job
        if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
        if (show_debug) {Serial.println("-------");}
        if (show_debug) {Serial.println("FAILURE!!");}
        if (show_debug) {Serial.println("-------");}
        /// Task job finished, increment the time slot semaphore
        xSemaphoreGive( semaphore_time_slot_users );
      }
      /// Semaphore is not available
      else{
        if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Could not obtain time slot semaphore");}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Monitors amount of time slot users in order to indicate a delayed task of a previous time slot.
  @param [in]
  @param [out]
  @note
  @todo Time out of task notification happens also at module setup before start of time slot schedule. Other tasks are blocking portMAX_DELAY, but this task just for one time slot duration. So this might interfere with the setup task and mess up the synchronisation procedure. Option is to use portMAX_DELAY also in monitoring task. However, missing the task notification will cause harm, since it is not known and nothing is monitoring the monitor task.
*/
static void taskMonitor( void *pvParameters ) {
  // PSEUDO CODE
  // Read from all modules start up joining message
  // Safe module priorities in generic buffers for all modules
  // Add module to queue of available modules
  // signal when finished within current time slot
  
  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( TIME_SLOT_DURATION_MS ); 
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t time_slot_notification;

  /// Storing number of time slot semaphore holders
  UBaseType_t TimeSlotUsers;
  if (using_plx_daq){
    Serial.print("DATA,TIME,TIMER,");
    Serial.println( (String) 1 + "," + (String) row + "," + (String) time_slot + "," + (String) 4 + ",AUTOSCROLL_20");
  }
  /// Run while loop forever
  while (1){
    /// Task blocks until notification is received or time out is passed
    time_slot_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

    /// If received notification from tick hook
    if ( time_slot_notification  > 0) {
      if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
      task_start = xTaskGetTickCount();
      
      /// Do task job
      if (show_debug_job_execution) {
        Serial.print(pcTaskGetName( NULL ));
        Serial.print(": ");
        Serial.println("Obtained time slot semaphore, executing task job");}

      // Do task job
      TimeSlotUsers = NUMBER_OF_TASKS-uxSemaphoreGetCount( semaphore_time_slot_users );
      Serial.print(TimeSlotUsers);Serial.println(" users");
      if(show_debug_monitor_job){
        Serial.print(pcTaskGetName( NULL ));
        Serial.print(": ");
        Serial.print("Number of time slot users: ");
        Serial.println(TimeSlotUsers);}
      
      // If one task is stopped, there is no delay in time slots, so decrement/reset to zero
      if(TimeSlotUsers == 0){
        if(show_debug_monitor_job){
          Serial.print(pcTaskGetName( NULL ));
          Serial.print(": ");
          Serial.println("GREEN: all time slot users finished their job before time slice ended.");}
      }
      else if(TimeSlotUsers >0 && TimeSlotUsers <= MAX_TIME_SLOT_USERS){
        if(show_debug_monitor_job){
          Serial.print(pcTaskGetName( NULL ));
          Serial.print(": ");
          Serial.print("ORANGE: ");
          Serial.print(TimeSlotUsers);
          Serial.println(" tasks using current time slot.");}
      }
      else if(TimeSlotUsers > MAX_TIME_SLOT_USERS){
          if(show_debug_monitor_job){
            Serial.print(pcTaskGetName( NULL ));
            Serial.print(": ");
            Serial.print("RED: ");
            Serial.print(TimeSlotUsers);
            Serial.print(" tasks using time slot, max allowed users are: ");
            Serial.println(MAX_TIME_SLOT_USERS);}
          xTaskNotifyGive( Handle_TaskFunctionalSafety );
        }
      else{
        if(show_debug_monitor_job){Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print("RED: unknown time slot semaphore value.");Serial.println(TimeSlotUsers);}
      }
    }
    /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
    else {
      if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get time slot notification");}
    }
    /// Calculate and print task execution time in ms
    task_end = xTaskGetTickCount();
    task_execution_time_in_ticks =  task_end - task_start;
    if(show_debug_job_execution){
      Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
    }
    /// Debugpin at start of time slot task execution
    digitalWrite(test_pin, LOW);
  }
  /// Task should never come here, but if it does task is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Synchronizes all modules in system before time slot schedule starts. This task is executed at the first tick hook interrupt, after global time is set to it's first value.
  @param [in]
  @param [out]
  @note
  @note Do not disable tick hook interrupts because it manages progression of time. This task will not be interrupted by thick hook when it is finished within one millisecond, the period of the interrupt. 
  @todo Protect i2c-port with semaphore
  @todo Make sure modules to synchronize are already joined on i2c bus before checking availability, for example by putting a delay at the start of the task using myDelayMs or so.
  @todo Wait for end of time schedule to start at 0,0 at synched time. 
  @todo Make sure time is not send before receiving modules are waiting for the new_global_time_update boolean to be true, because this might cause unpredictable delay. You can add a delay before sending the global time to decrease the probability of occurence of this risk.
*/
static void taskModuleSetup( void *pvParameters ) {
  /// Tick frequency defined in FreeRTOSConfig.h at 1ms (1000Hz).
  volatile TickType_t task_start = 0;
  volatile TickType_t task_end = 0;
  volatile TickType_t task_execution_time_in_ticks =  0;
  
  /// Blocks task for specified time until time slot notification is received
  const TickType_t block_time_start_notification = pdMS_TO_TICKS( BLOCK_TIME_AWAITING_START_NOTIFICATION_MS );
  const TickType_t block_time_semaphore_take = BLOCK_TIME_AWAITING_TIME_SLOT_SEMAPHORE_TICKS;
  
  /// Holding the number of received unprocessed time slot notifications from the system tick interrupt
  uint32_t task_notification;

  /// Task blocks until notification is received or time out is passed
  task_notification  = ulTaskNotifyTake( pdFALSE, block_time_start_notification );

  /// Storing time needed for each module to reach in order to start a synchronised time slot schedule
  TickType_t wait_until = 0;
  
  /// If received notification from tick hook
  if ( task_notification  > 0) {
    if (show_test_timeslots) {Serial.println(pcTaskGetName( NULL ));}
    
    if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Got my time slot notification");}
    task_start = xTaskGetTickCount();
    /// Do task job
    if (show_debug_job_execution) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Obtained time slot semaphore, executing task job");}
    if (show_debug_job_execution) {Serial.println("Sending time to modules");}

    /// Look for availability of module on I2C-bus and set it's availability boolean  
    for (int module_to_synchronize = 1; module_to_synchronize<number_of_modules; module_to_synchronize++){
      moduleIsJoinedI2C(module_to_synchronize); 
    }

    /// Synchronisation time and module waiting time after synchronisation before starting time slot schedule. 
    /// Needs to be declared as close to the SendTimeToModule function, to prevent too early time update, 
    /// but outside the for loop.
    wait_until = global_time.milliseconds_total+delay_before_time_slot_schedule_start; 
    byte timeBuffer[10] = {
      global_time.day, 
      global_time.hour, 
      global_time.minute, 
      global_time.second, 
      global_time.millisecond>>8&0xFF, 
      global_time.millisecond,
      wait_until>>24&0xFF,
      wait_until>>16&0xFF,
      wait_until>>8&0xFF,
      wait_until&0xFF
    };
    
    /// Send global time and waiting time to modules
    for (int module_to_synchronize = 1; module_to_synchronize<number_of_modules; module_to_synchronize++){
      /// Starting at 1, because index 0 is this smart hub controller sending the time
      SendTimeToModule(module_to_synchronize, timeBuffer); 
    }
    
    /// Global time is send to other modules, so let's wait until specified time (number_of_modules*global_time_transfer_delay_ms)
    while ( global_time.milliseconds_total < (new_global_time_in_ms + delay_before_time_slot_schedule_start) ){myDelayUs(1);}
    
    /// All modules synched - 1 (=smart hub controller)
    start_time_slot_schedule = true;
    
    if (show_debug_global_time) {
      Serial.print(global_time.milliseconds_total);
      Serial.print(" | ");
      Serial.print(wait_until);
      Serial.print(" | ");
      Serial.print(global_time.day);
      Serial.print(":");
      Serial.print(global_time.hour);
      Serial.print(":");
      Serial.print(global_time.minute);
      Serial.print(":");
      Serial.print(global_time.second);
      Serial.print(":");
      Serial.println(global_time.millisecond);
    }
    Serial.println("Start timeslotschedule"); 
    /// Task finished its job and is deleted in a clean way
    vTaskDelete( NULL );
  }
  /// If not received time slot notification from tick hook after BLOCK_TIME_AWAITING_START_NOTIFICATION_MS
  else {
    if (show_debug) {Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.println("Didn't get task notification");}
  }
  /// Calculate and print task execution time in ms
  task_end = xTaskGetTickCount();
  task_execution_time_in_ticks =  task_end - task_start;
  if(show_debug_job_execution){
    Serial.print(pcTaskGetName( NULL ));Serial.print(": ");Serial.print(task_execution_time_in_ticks);Serial.println("ms");
  }  
  /// Task finished its job and is deleted in a clean way
  vTaskDelete( NULL );
}

/*!
  @brief Setup of the RTOS, creating semaphores, initiating Serial and I2C-communication, creating tasks and starting FreeRTOS task scheduler.
  @param [in]
  @param [out]
  @note
  @todo
*/
void setup()
{
  serialInit();
  initTwi();

  pinMode(test_pin, OUTPUT);
  digitalWrite(test_pin, LOW);
  
  x_Serial_Semaphore = xSemaphoreCreateMutex();

  if (x_Serial_Semaphore == NULL) {if (show_debug) {Serial.println("Mutex can not be created");}}

  semaphore_time_slot_users = xSemaphoreCreateCounting( NUMBER_OF_TASKS, NUMBER_OF_TASKS );
  if( semaphore_time_slot_users != NULL ){
    Serial.println("Created time slot semaphore");
  }
  else{
    Serial.println("Could not create semaphore_time_slot_users succesfully");
  }
  
  if (show_debug) {Serial.println("");}
  if (show_debug) {Serial.println("******************************");}
  if (show_debug) {Serial.println("      Program SHC start       ");}
  if (show_debug) {Serial.println("******************************");}

  blinkInitLEDs(3);

  // Create the threads that will be managed by the rtos
  // Sets the stack size and priority of each task
  // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
  // new tasks based on design
  xTaskCreate(taskTimeSlotTimeLeft,   "Task TimeSlotTimeLeft",    256, NULL, tskIDLE_PRIORITY + 1, &Handle_TaskTimeSlotTimeLeft);
  xTaskCreate(taskRequestData,        "Task RequestData",         256, NULL, tskIDLE_PRIORITY + 2, &Handle_TaskRequestData);
  xTaskCreate(taskDecideFeedback,     "Task DecideFeedback",      256, NULL, tskIDLE_PRIORITY + 2, &Handle_TaskDecideFeedback);
  xTaskCreate(taskSendData,           "Task SendData",            256, NULL, tskIDLE_PRIORITY + 2, &Handle_TaskSendData);
  xTaskCreate(taskSynchronizeModules, "Task SynchronizeModules",  256, NULL, tskIDLE_PRIORITY + 2, &Handle_TaskSynchronizeModules);
  xTaskCreate(taskHeartBeat,          "Task HeartBeat",           256, NULL, tskIDLE_PRIORITY + 2, &Handle_TaskHeartbeat);
  xTaskCreate(taskSafeCheckpoint,     "Task SafeCheckpoint",      256, NULL, tskIDLE_PRIORITY + 2, &Handle_TaskSafeCheckpoint);
  xTaskCreate(taskFunctionalSafety,   "Task FunctionalSafety",    256, NULL, tskIDLE_PRIORITY + 4, &Handle_TaskFunctionalSafety);
  xTaskCreate(taskCheckpointRecovery, "Task CheckpointRecovery",  256, NULL, tskIDLE_PRIORITY + 4, &Handle_TaskCheckpointRecovery);
  xTaskCreate(taskMonitor,            "Task Monitor",             256, NULL, tskIDLE_PRIORITY + 3, &Handle_TaskMonitor);
  xTaskCreate(taskModuleSetup,        "Task ModuleSetup",         256, NULL, tskIDLE_PRIORITY + 4, &Handle_TaskModuleSetup);
  
  Serial.println("Created tasks");
  
  // Start the RTOS, this function will never return and will schedule the tasks.
  vTaskStartScheduler();

  // error scheduler failed to start
  while (1)
  {
    Serial.println("Scheduler Failed! \n");
    delay( 1000 );
  }

}

/*!
  @brief The RTOS idle task, runs when no other task can be scheduled.
  @param [in]
  @param [out]
  @note
  @todo
*/
void loop()
{
  Serial.println("In idle task loop()");
}

/*!
  @brief This is the rtos tick hook interrupt callback/hook function. Prototype defined in Arduino.h of Adafruit and FreeRTOS_SAMD51/src/idle_hook.c
  @param [in]
  @param [out]
  @note Serial.prints used for debugging are timeconsuming and might influence your debugresults.
  @todo
*/
void vApplicationTickHookTimeSlot()
{    
  if(demo_synchronisation == true){  if(demo_time_slot_counter ==4000){start_time_slot_schedule = false;}}
  
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  // Update global time
  global_time.millisecond++;
  global_time.milliseconds_total++;
  if (global_time.millisecond % 1000 == 0) {
    global_time.second++;
    global_time.millisecond = 0;
    if (global_time.second % 60 == 0) {
      global_time.minute++;
      global_time.second = 0;
      if (global_time.minute % 60 == 0) {
        global_time.hour++;
        global_time.minute = 0;
        if (global_time.hour % 24 == 0) {
          global_time.day++;
          global_time.hour = 0;
        }
      }
    }
  }

  /// If modules are not synched before time slot schedule is started
  if (start_time_slot_schedule == false && task_module_setup_is_notified == false) {
    /// Call synching task at start of system and at start of a time slot
    vTaskNotifyGiveFromISR(Handle_TaskModuleSetup, &xHigherPriorityTaskWoken);
    // Ensure context switch if notified task is unblocked AND has higher priority
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    task_module_setup_is_notified = true;
  }
  /// Modules are synched, so move on to time slot task execution
  else if (start_time_slot_schedule == true){  
    /// If new time slot starts, update time slot coordinate
    if (global_time.millisecond % TIME_SLOT_DURATION_MS == 0) {
      if (show_debug) {Serial.println("*");}
      demo_time_slot_counter++;
      if (show_debug_global_time) {
        Serial.print("[element][row]: [");
        Serial.print(time_slot);
        Serial.print("][");
        Serial.print(row);
        Serial.println("]");
    
        Serial.print("Global time: ");
        Serial.print(global_time.day);
        Serial.print(":");
        Serial.print(global_time.hour);
        Serial.print(":");
        Serial.print(global_time.minute);
        Serial.print(":");
        Serial.print(global_time.second);
        Serial.print(":");
        Serial.println(global_time.millisecond);
      }
      // Set task handle of task of this new time slot
      current_time_slot = *buffer_time_slot[row][time_slot];
      
      // Notify monitor to check which task is running
      vTaskNotifyGiveFromISR( Handle_TaskMonitor, &xHigherPriorityTaskWoken );
      // Ensure context switch if notified task is unblocked AND has higher priority
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
      
      // Notify current time slot task with start notification
      vTaskNotifyGiveFromISR( current_time_slot, &xHigherPriorityTaskWoken );
      // Ensure context switch if notified task is unblocked AND has higher priority
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  
      // Update time slot element and row of time slot buffer
      time_slot++;
      if (time_slot == TIME_SLOT_ELEMENTS) {
        row++;
        time_slot = 0;
        if (row >= TIME_SLOT_ROWS) {
          row = 0;
        }
      }
    }
  }
}
