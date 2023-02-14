/*!\addtogroup ManikinOS
  \file LedHelper.ino
  \ingroup ManikinOS
  @{
  @brief This file handles the led functionality.
  @author Gert van Lagen
*/

// Set the led the rtos will blink when we have a fatal rtos error
// RTOS also Needs to know if high/low is the state that turns on the led.
// Error Blink Codes:
//    3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
//    2 blinks - Malloc Failed, Happens when you couldn't create a rtos object.
//               Probably ran out of heap.
//    1 blink  - Stack overflow, Task needs more bytes defined for its stack!
//               Use the taskMonitor thread to help gauge how much more you need
// Same led is used for heartbeat indication

/*!
  @brief 
  @param [in]
  @param [out]
  @note
  @todo
*/
void initLEDs() {
    vSetErrorLed(hb_led_pin, LOW);
}

/*!
  @brief 
  @param [in]
  @param [out]
  @note
  @todo
*/
void blinkInitLEDs(int t) {
  for (int i = 0; i < t; i++) {
    digitalWrite(hb_led_pin, HIGH);
    delay(100);
    digitalWrite(hb_led_pin, LOW);
    delay(100);
  }
  digitalWrite(hb_led_pin, LOW);
}

//*****************************************************************
