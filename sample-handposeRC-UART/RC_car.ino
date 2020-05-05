//www.elegoo.com


// Define control pin numbers
#define ENA 5  // Motor voltage pins (speed)
#define ENB 6
#define IN1 7  // H bridge pins (direction)
#define IN2 8
#define IN3 9
#define IN4 11

unsigned char carSpeed = 155;  // Turning speed (0-255)
char command;  // Variable used to store serial data receieved

// Define control functions
void forward(){ 
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void back(){
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}
 
void left(){
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH); 
}
 
void right(){
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}
 
void stop(){
  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
}

 
void setup() { 
  Serial.begin(38400);  // Begin serial at same baudrate as Bluetooth module
  pinMode(IN1,OUTPUT);  // Initialize pins as I/O
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  stop();
}
 
void loop() { 
  if(Serial.available() > 0){  // Check number of bytes of serial data available
    command = byte(Serial.read());  // Read serial data receieved
    switch(command){
       case 'f': forward(); break;  // Check receieved char and select control function accordingly
       case 'b': back(); break;
       case 'l': left(); break;
       case 'r': right(); break;
       case 's': stop(); break;
       default: stop(); break;
    }
   }
   delay(10);
}
