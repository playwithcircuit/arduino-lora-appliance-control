/*
Code to receive commands fom Mobile device using blutooth functionality of RLY999 module and 
send these commands from one Arduino to another Arduino using LORA functionality of RYL999 Module and Display
complete communication on I2C LCD by platwithcircuit.com
*/
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// below MACROS are for LORA communication
#define REPLY_TIMEOUT_IN_MS 300
#define REPLY_END_CHAR '\n'
#define SELF_ADDRESS 0
#define RECEIVERS_ADDRESS 1
#define MIN_CHAR_TO_RCV 1
#define WAIT_FOR_RECIVERS_REPLY 3000

// below MACROS and strings are for Bluetooth communication
#define START_CHAR_BT_COMM '*'
#define END_CHAR_BT_COMM '#'
#define START_CHAR_TIME_OUT_BT_COMM (3000U)  // time in ms
#define END_CHAR_TIME_OUT_BT_COMM (300U)      // time in ms

// these four strings can be received from bluetooth Rx pin which are transmitted by the mobile application
String sCMDLampON = "L1";
String sCMDLampOFF = "L0";
String sCMDFanON = "F1";
String sCMDFanOFF = "F0";

// save received command in this global string object
String receivedCommand;

// initialize softserial port at pin 3(Rx) and 2(Tx)
SoftwareSerial btSerial(3, 2);

// Init LCD at 0x27, 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  boolean boRetVal = false;
  // begin serial communication at baud 115200,n,8,1
  // to communicate with the LORA functionality of module
  Serial.begin(115200);
  // begin soft serial communicatio at baud 115200,n,8,1
  // to communicate with the Bluetooth functionality of module
  btSerial.begin(115200);
  btSerial.setTimeout(END_CHAR_TIME_OUT_BT_COMM);  // set time out for readStringUntil() function

  // Initialize the LCD
  lcd.init();
  // Turn ON the Backlight
  lcd.backlight();
  // Clear the display buffer
  lcd.clear();

  receivedCommand.reserve(50);  // prevents fragmentation, as multiple times data shall be received in this string

  // clear receive buffer of LORA
  flushBuffer();  // clear rx data

  // Reset settings to factory defaults for LORA functionality
  boRetVal = boRestoreFactoryDefaults();

  // setting the address of LORA
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

  flushBuffer();  // clear rx data

  // get commands using BT funtionality using Mobile Application
  boRetVal = rcvCommand(START_CHAR_TIME_OUT_BT_COMM);

  if (boRetVal == false) {
    // Displaying Failed Msg
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No BT Command");
    lcd.setCursor(0, 1);
    lcd.print("Received");
  } else {
    // check if valid command is received
    if ((receivedCommand == sCMDLampON) || (receivedCommand == sCMDLampOFF) || (receivedCommand == sCMDFanON) || (receivedCommand == sCMDFanOFF)) {
      // tranmits receivedCommand
      boRetVal = boSendData(receivedCommand);
      if (boRetVal == true) {
        // Displaying Sent Msg
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cmd Rcvd & Sent");
        lcd.setCursor(0, 1);
        lcd.print(receivedCommand);
        delay(1000);
        boRetVal = chkReply(expected_reply, REPLY_END_CHAR, WAIT_FOR_RECIVERS_REPLY);

        if (boRetVal == true) {
          // Displaying received Msg
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Reply Received:");
          lcd.setCursor(0, 1);
          lcd.print(expected_reply);
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("No Reply");
          lcd.setCursor(0, 1);
          lcd.print("Received");
        }
      } else {
        // Displaying Failed Msg
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Command");
        lcd.setCursor(0, 1);
        lcd.print("Sending Failed");
      }
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
/******************************************** Function Defenition Related to LORA Functionality ***********************/

void sendCrLf(void) {
  Serial.write(0x0D);  // Carriage Return
  Serial.write(0x0A);  // Line Feed
}

// clear receive buffer of LORA
void flushBuffer(void) {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

// check data on rx pin of LORA functionality
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

// Reset settings to factory defaults for LORA functionality
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

// setting the address of LORA
bool boSetAddress(void) {
  const char setAddressCmd[] = "AT+ADDRESS=";  // command to be sent
  bool boReturnValue = false;                  // function's return value
  String chkRcvString = "+OK";

  // send command
  Serial.print(setAddressCmd);
  Serial.print(SELF_ADDRESS);
  sendCrLf();

  // check reply
  boReturnValue = chkReply(chkRcvString, REPLY_END_CHAR, REPLY_TIMEOUT_IN_MS);

  // return result
  return boReturnValue;
}

// Send data to LORA in command form
bool boSendData(String data) {
  const char sendDataCmd[] = "AT+SEND=";  // command to be sent
  bool boReturnValue = false;             // function's return value
  String chkRcvString = "+OK";

  // send command
  Serial.print(sendDataCmd);
  Serial.print(RECEIVERS_ADDRESS);
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

/******************************************** Function Defenition Related to BT Functionality *************************/

// Receive Command from Rx pin of Bluetooth functionality
bool rcvCommand(unsigned int timeout) {
  bool boReturnValue = false;  // function's return value
  unsigned long startTime;

  // wait for reply
  do {
    timeout--;
    delay(1);  // delay of 1 ms
    if (((btSerial.available() >= MIN_CHAR_TO_RCV) && (btSerial.read() == START_CHAR_BT_COMM)) || (timeout == 0)) {
      break;
    }
  } while (1);

  // if start cracter is receved within time then timeout will be greater tahn or equal to 1
  if (timeout) {
    startTime = millis();
    receivedCommand = btSerial.readStringUntil(END_CHAR_BT_COMM);
    if ((millis() - startTime) <= END_CHAR_TIME_OUT_BT_COMM) {
      boReturnValue = true;  // it means in readStringUntil() function '#' is received
    } else {
      boReturnValue = false;  // it means in readStringUntil() function timeout has occured
    }
  } else {
    boReturnValue = false;  // it means get character timeout has occured
  }

  // return result
  return boReturnValue;
}