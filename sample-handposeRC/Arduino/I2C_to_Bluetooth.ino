
// Include the Wire library for I2C
#include <Wire.h>


void setup() {
  Wire.begin(0x08);  // Set device as I2C slave with address 8
  Serial.begin(38400);  // Begin serial communication with same baudrate as Bluetooth module               
  Wire.onReceive(receiveEvent);  // Define function to perform when receive I2C data 
  delay(5000);
}

// Function to excecute when received data through I2C
void receiveEvent(int howMany) {  
  byte cmd = Wire.read();  // First byte receieved (cmd) is unused
  byte rc_command = Wire.read();  // Second byte represents command for rc car (forward, backward, etc)

  // Check data received through I2C and send through serial to Bluetooth module
  switch(rc_command){
   case 0: Serial.write('s'); break; 
   case 1: Serial.write('f'); break;
   case 2: Serial.write('b'); break;
   case 3: Serial.write('r'); break;
   case 4: Serial.write('l'); break;
  }
}


void loop() {
  delay(10);  
}
