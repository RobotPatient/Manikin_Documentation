/*!@addtogroup ManikinOS
  @file TwiController.ino
  @ingroup ManikinOS
  @{
  @brief Contains functions to manage I2C-communication between modules
  @author Gert van Lagen
*/

/// Header files
#include "settings.h"
#include <Wire.h>

/// Global I2C variables

typedef struct{
  char module_name;
  byte address;
  byte message_priority;
  boolean is_available;
  boolean synchronized_at_start;
} Module;

Module modules[number_of_modules] = {};

//**************************************************************************
// I2C functions
//**************************************************************************

/*!
  @brief Initializes module array with module specific parameters. Starts I2C-communication at High Speed mode (3.4Mbit/second).
  @param [in]
  @param [out]
  @note
  @todo 
*/
void initTwi(){
  // Initialise existing modules in array of module struct
  modules[smart_hub_controller_index] =     {smart_hub_controller_name, smart_hub_controller_i2c_address, smart_hub_controller_priority,  false, true};
  modules[compression_controller_index] =   {compression_controller_name, compression_controller_i2c_address, compression_controller_priority,  false, false};
  modules[ventilation_controller_index] =   {ventilation_controller_name, ventilation_controller_i2c_address, ventilation_controller_priority,  false, false};
  modules[communication_controller_index] = {communication_controller_name, communication_controller_i2c_address, communication_controller_priority,  false, false};
  modules[bus_monitor_controller_index] =   {bus_monitor_controller_name, bus_monitor_controller_i2c_address, bus_monitor_controller_priority, false, false};
  
  Wire.begin(); // No I2C-address passed in order to join the bus as a master I2C-controller 
  Wire.setClock(3400000); /// Needs to be done only at I2C-master
}

boolean moduleIsJoinedI2C(int module_number){
  // check if i2c connection is available/if module has joined the I2C-bus
  Wire.beginTransmission(modules[module_number].address);
  int error = Wire.endTransmission();
  
  // check if availability returns zero error and set module availability
  modules[module_number].is_available = (error == 0);
  if(show_debug){Serial.print("Module address: 0x");}
  if(show_debug){Serial.print(modules[module_number].address, HEX);}
  
  // print address
  if (modules[module_number].is_available) {
    if(show_debug){Serial.println(" is available...");}
  } else {
    if(show_debug){Serial.println(" is unavailable...");}
  }
  return modules[module_number].is_available;
}

/*!
  @brief 
  @param [in] module_number is an integer of the module to send the time to. This integer is part of module struct containing all module specific information.
  @param [in] day represents the global time day number
  @param [in] hour represents the global time hour number
  @param [in] minute represents the global time minute number
  @param [in] second represents the global time second number
  @param [in] millisecond represents the global time millisecond number
  @param [in] wait_until_ms is the time point of where to start the time slot schedule on each module after synchronisation. It takes the different receive times of modules in mind.
  @param [out]
  @note
  @todo
*/
void SendTimeToModule(int module_number, byte timeBuffer[]){
  if (modules[module_number].is_available) {    
    Wire.beginTransmission(modules[module_number].address);
    for (int i = 0; i<10; i++){
      Wire.write(timeBuffer[i]);
    }
    int error = Wire.endTransmission();
    new_global_time_in_ms = global_time.milliseconds_total;
    if (error == 1){
      Serial.println("Sending time to secondary SAMD21 failed");
    }
    else if (error == 0){modules[module_number].synchronized_at_start=true;}
  }
}    


/*!
  @brief Event-Triggered joining of modules to I2C-bus. Used for testing. Not in use anymore
  @param [in]
  @param [out]
  @note
  @todo
*/
/*void moduleI2CDynamicJoin(int module_number, int interruptValue){
  // Set interrupt_pin value to simulated value
  modules[module_number].interrupt_pin = interruptValue;
  
  // interrupt line low: serial print (module m not connected)
  if(modules[module_number].interrupt_pin == 0){
    Serial.print("module ");
    Serial.print(modules[module_number].module_name);
    Serial.println(" not connected, wrong detection of module interrupt line");
  }
  else{// join module 
    Serial.print("module ");
    Serial.print(modules[module_number].module_name);
    Serial.println(" waiting to join");
  
    // check if i2c connection is available/if module has joined the I2C-bus
    Wire.beginTransmission(modules[module_number].address);
    int error = Wire.endTransmission();
    
    // check if availability returns zero error and set module availability
    modules[module_number].is_available = (error == 0);
    Serial.print("Module address: 0x");
    Serial.print(modules[module_number].address, HEX);
    
    // print address
    if (modules[module_number].is_available) {
      Serial.println(" is available...");
    } else {
      Serial.println(" is unavailable...");
    }

    if(modules[module_number].is_available == 1){
      // Request number of bytes from slave module
      Wire.requestFrom(modules[module_number].address, 33);
      while(Wire.available())    // slave may send less than requested
      {
        char c = Wire.read();    // receive a byte as character
        Serial.print(c);         // print the character
      }
      Serial.println("\nThis was the joining message of the slave module");
      
      // write byte to slave module
      Wire.beginTransmission(modules[module_number].address);
      byte join_request_message = 'j';
      Wire.write(join_request_message);
      error = Wire.endTransmission(true);
      
      if (error == 0) {
        Serial.println("Succesfully sent request for joining message to module ");
      } else {
        Serial.println("Sending request for joining message failed");
      }
      // set priority by extracting JSON message of joining module
      //Wire.requestFrom(modules[module_number].address, 2);
        
      // print out cool stuff
    
      // module[m] should clear interrupt line when main module has joined module
      // while(modules[module_number].interrupt_line == 1){;}
    }
    else{
      Serial.print("Module at address ");
      Serial.print(modules[module_number].address, HEX);
      Serial.println("not available, so not requesting joining message");
    }
  }
}                   
*/
