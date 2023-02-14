/*!\addtogroup ManikinOS
  \file SerialHelper.ino
  \ingroup ManikinOS
  @{
  @brief This file contains functions to manage serial communication
  @author Gert van Lagen
*/

/// Header files
#include "settings.h"

///Serial functions

/*!
  @brief Starts serial port at in settings.h defined serial_baudrate. 
*/
void serialInit() {
  SERIAL.begin(serial_baudrate);

  delay(1000); // prevents usb driver crash on startup, do not omit this
  while (!SERIAL);  // Wait for serial terminal to open port before starting program
}
//*****************************************************************
