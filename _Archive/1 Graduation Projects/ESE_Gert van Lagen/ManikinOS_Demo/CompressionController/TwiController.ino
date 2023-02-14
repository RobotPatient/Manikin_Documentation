/*!\addtogroup ManikinOS
  \file TwiController.ino
  \ingroup ManikinOS
  @{
  @brief Contains functions to manage I2C-communication between modules
  @author Gert van Lagen
*/

/// Header files
#include "settings.h"
#include <Wire.h>

/// Global variables

typedef struct{
  char module_name;
  byte address;
  byte message_priority;
  boolean is_available;
} Module;

Module modules[number_of_modules] = {};

//**************************************************************************
// I2C functions
//**************************************************************************
void initTwi(){
  Wire.begin(compression_controller_i2c_address); 
  Wire.onRequest(sendJoiningInformation);
  Wire.onReceive(readBytesFromMaster);
}

void sendJoiningInformation(){
  Serial.println("Got a master request, let's send him some bytes...");
  Wire.write("hello Compression controller here");
}

///@todo update new global time update with compensation for transferdelay
void readBytesFromMaster(int num_bytes){
  byte timeBuffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  char c;
  int i = 0;
//  Serial.print("Available bytes on I2C bus: ");
//  Serial.println(num_bytes);
  while (0 < Wire.available()) // 1 < Wire.available() loop through all but the last
  {
    c = Wire.read(); /// Receive I2C-byte as a character, doesn't work when putting byte directly in timeBuffer
    timeBuffer[i] = c;
    i++;
    //Serial.println((int)c);
  }
  if (i==10){
    global_time.day = timeBuffer[0];
    global_time.hour = timeBuffer[1];
    global_time.minute = timeBuffer[2];
    global_time.second = timeBuffer[3];
    global_time.millisecond = timeBuffer[4];
    global_time.millisecond = (global_time.millisecond << 8) | timeBuffer[5];
    delay_before_time_slot_schedule_start = (timeBuffer[6] << 24) | (timeBuffer[7] << 16) | (timeBuffer[8] << 8) | (timeBuffer[9]);
 
    /// Set milliseconds_total to new total ms received from master
    global_time.milliseconds_total = global_time.millisecond + (1000*global_time.second) + (1000*60*global_time.minute) + (1000*60*60*global_time.hour) + (1000*60*60*24*global_time.day);
    
    new_global_time_update=true;
  }
}
