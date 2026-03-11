/*
Code to receieve commands to control the appliances from another Arduino using RLYR999 Module
and send "DONE" in reply after implementing the required task and display the communication and tasks over I2C LCD.
The appliances Lamp and Fan are connected to relay which will be turne ON and OFF based on commands received
by platwithcircuit.com
*/
#include <LiquidCrystal_I2C.h>

// below MACROS are for LORA communication
#define REPLY_TIMEOUT_IN_MS 300
#define REPLY_END_CHAR '\n'
#define CMD_END_CHAR '\n'
#define MODULE_ADDRESS 1
#define TRANSMITTER_ADDRESS 0
#define MIN_CHAR_TO_RCV 1
#define WAIT_FOR_REQUEST 3000

// these four commands can be received from controller module
String sCMDLampON = "L1";
String sCMDLampOFF = "L0";
String sCMDFanON = "F1";
String sCMDFanOFF = "F0";

// save received command in this global string object
String receivedCommand;

// Init LCD at 0x27, 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Appliance Control MACROS
#define LAMP_CTRL_PIN 11
#define FAN_CTRL_PIN 12

#define LAMP_ON() digitalWrite(LAMP_CTRL_PIN, 0)
#define LAMP_OFF() digitalWrite(LAMP_CTRL_PIN, 1)
#define FAN_ON() digitalWrite(FAN_CTRL_PIN, 0)
#define FAN_OFF() digitalWrite(FAN_CTRL_PIN, 1)

void setup() {
  boolean boRetVal = false;
  // begin serial commmunicatio at baud 115200,n,8,1
  // to comunicate with the RF module
  Serial.begin(115200);

  // Initialize the LCD
  lcd.init();
  // Turn ON the Backlight
  lcd.backlight();
  // Clear the display buffer
  lcd.clear();

  // initialize applinaces pins and turn off
  pinMode(LAMP_CTRL_PIN, OUTPUT);
  pinMode(FAN_CTRL_PIN, OUTPUT);
  LAMP_OFF();
  FAN_OFF();

  receivedCommand.reserve(50);  // prevents fragmentation, as multiple times data shall be received in this string

  delay(1000);

  flushBuffer();  // clear rx data

  // Reset settings to factory defaults
  boRetVal = boRestoreFactoryDefaults();

  // setting the address if reset successfull
  if (boRetVal == true) {
    flushBuffer();  // clear rx data
    boRetVal = boSetAddress();
  }

  if (boRetVal == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Module Init");
    lcd.setCursor(0, 1);
    lcd.print("Successful");
    delay(1000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Module Init");
    lcd.setCursor(0, 1);
    lcd.print("Failed");
    while (1)
      ;
  }
}

void loop() {
  String expected_reply = "DONE";
  bool boRetVal = false;

  // check string sent by Initiator
  boRetVal = rcvCommand(CMD_END_CHAR, WAIT_FOR_REQUEST);

  if (boRetVal == false) {
    // Displaying Failed Msg
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Command");
    lcd.setCursor(0, 1);
    lcd.print("Received");
  } else {
    // check if valid command is received
    if ((receivedCommand == sCMDLampON) || (receivedCommand == sCMDLampOFF) || (receivedCommand == sCMDFanON) || (receivedCommand == sCMDFanOFF)) {
      // Implement receivedCommand
      vImplementTask(receivedCommand);
      // Displaying Sent Msg
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Command Received");
      lcd.setCursor(0, 1);
      lcd.print("Task Done");
      delay(1000);
      // tranmits receivedCommand
      boRetVal = boSendData(expected_reply);
      if (boRetVal == true) {
        // Displaying Sent Msg
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reply Sent");
      } else {
        // Displaying Failed Msg
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reply");
        lcd.setCursor(0, 1);
        lcd.print("Sending Failed");
      }
      delay(1000);
    } else {
      // Displaying Invalid Msg
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid: ");
      lcd.print(receivedCommand);
      lcd.setCursor(0, 1);
      lcd.print("Command Received");
    }
  }
}

void sendCrLf(void) {
  Serial.write(0x0D);  // Carriage Return
  Serial.write(0x0A);  // Line Feed
}

void flushBuffer(void) {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

bool rcvCommand(char receiveUntil, unsigned int timeout) {
  bool boReturnValue = false;  // function's return value
  unsigned long startTime;
  int firstComma;
  int secondComma;
  int thirdComma;
  String data;
  // wait for reply
  do {
    timeout--;
    delay(1);  // delay of 1 ms
  } while ((Serial.available() < MIN_CHAR_TO_RCV) && (timeout > 0));

  if (timeout) {
    startTime = millis();
    // if timeout is left then a reply is received check for the string in the reply
    data = Serial.readStringUntil(receiveUntil);
    if ((millis() - startTime) <= timeout) {
      boReturnValue = true;  // it means in readStringUntil() function receiveUntil character is received
      // as the data will be in form of "+RCV=0,2,F1,-88,11" we need to extract command between second and third comma
      firstComma = data.indexOf(',');
      secondComma = data.indexOf(',', firstComma + 1);
      thirdComma = data.indexOf(',', secondComma + 1);
      // extract value between 2nd and 3rd comma
      receivedCommand = data.substring(secondComma + 1, thirdComma);
    } else {
      boReturnValue = false;  // it means in readStringUntil() function timeout has occured
    }
  } else {
    boReturnValue = false;  // it means get character timeout has occured
  }

  // return result
  return boReturnValue;
}

bool chkReply(String chkString, char receiveUntil, unsigned int timeout) {
  String receivedString;       // save received data in this string object
  bool boReturnValue = false;  // function's return value

  // wait for reply
  do {
    timeout--;
    delay(1);  // delay of 1 ms
  } while ((Serial.available() < MIN_CHAR_TO_RCV) && (timeout > 0));

  if (timeout) {
    // if timeout is left then a reply is received check for the string in the reply
    receivedString = Serial.readStringUntil(receiveUntil);
    if (receivedString.indexOf(chkString) != -1) {
      boReturnValue = true;
    } else {
      boReturnValue = false;
    }
  } else {
    boReturnValue = false;
  }

  // return result
  return boReturnValue;
}

bool boRestoreFactoryDefaults(void) {
  const char factoryDefaultCmd[] = "AT+FACTORY";  // command to be sent
  bool boReturnValue = false;                     // function's return value
  char downCounter = 100;                         // Down counter to wait for reply
  String receivedString;                          // save received data in this string object

  String chkRcvString1 = "+FACTORY";
  String chkRcvString2 = "+READY";

  // send command
  Serial.print(factoryDefaultCmd);
  sendCrLf();

  // check first string in reply
  boReturnValue = chkReply(chkRcvString1, REPLY_END_CHAR, REPLY_TIMEOUT_IN_MS);
  if (boReturnValue == true) {
    // check second string in reply
    boReturnValue = chkReply(chkRcvString2, REPLY_END_CHAR, REPLY_TIMEOUT_IN_MS);
  }

  // return result
  return boReturnValue;
}

bool boSetAddress(void) {
  const char setAddressCmd[] = "AT+ADDRESS=";  // command to be sent
  bool boReturnValue = false;                  // function's return value
  String chkRcvString = "+OK";

  // send command
  Serial.print(setAddressCmd);
  Serial.print(MODULE_ADDRESS);
  sendCrLf();

  // check reply
  boReturnValue = chkReply(chkRcvString, REPLY_END_CHAR, REPLY_TIMEOUT_IN_MS);

  // return result
  return boReturnValue;
}

bool boSendData(String data) {
  const char sendDataCmd[] = "AT+SEND=";  // command to be sent
  bool boReturnValue = false;             // function's return value
  String chkRcvString = "+OK";

  // send command
  Serial.print(sendDataCmd);
  Serial.print(TRANSMITTER_ADDRESS);
  Serial.print(',');
  Serial.print(data.length());
  Serial.print(',');
  Serial.print(data);
  sendCrLf();

  // check reply
  boReturnValue = chkReply(chkRcvString, REPLY_END_CHAR, REPLY_TIMEOUT_IN_MS);

  // return result
  return boReturnValue;
}

// this funtion is used to control the relay which controls the appliances using digital pins
void vImplementTask(String data) {
  if (receivedCommand == sCMDLampON) {
    LAMP_ON();
  } else if (receivedCommand == sCMDLampOFF) {
    LAMP_OFF();
  } else if (receivedCommand == sCMDFanON) {
    FAN_ON();
  } else if (receivedCommand == sCMDFanOFF) {
    FAN_OFF();
  }
}
