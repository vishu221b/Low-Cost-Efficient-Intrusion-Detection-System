/******************************/
/** @Developer: Vishal Dogra **/
/******************************/

#include <string.h>


#define LED_PIN                 11              // Pin for the Teensy 2.0 LED
#define PIR_INPUT_PIN           15
#define MAX_CMD_SIZE            32
#define MAX_LINE_CHAR_SIZE      50

#define _SEPERATOR_COLON        ":"
#define _MODULE_BADGE_ID        "CTx2" //"CTx1" | "CTx2" | "CTx3"
#define _CMD_TRIGGER_SUFFIX     "_CMD"

String EMPTY_STRING             = "";
String PIR_START_CMD            = "PIR_START";
String PIR_STOP_CMD             = "PIR_STOP";
String PIR_CHANGE_TIMEOUT_CMD   = "PIR_TIMEOUT";
String PIR_VERBOSE_CMD          = "PIR_VERBOSE";
String DETECTION_EVENT          = "EVENT_MOTION_DETECTED";
String PIR_START_EVENT          = "EVENT_PIR_STARTED";
String PIR_STOP_EVENT           = "EVENT_PIR_STOPPED";
String PIR_TIMEOUT_CHANGE_EVENT = "EVENT_TIMEOUT_CHANGED";

const char* FEED_START          = "FEED_START";
const char* FEED_STOP           = "FEED_STOP";
const char* INIT_MSG_STRING     = "%s: Beginning Serial Communication...\n";

bool __LOG_LOG                                      = true;
bool PIR_ACTIVE                                     = true;
long long unsigned int STARTING_TIME                = 1000;
long long unsigned int PIR_TIMEOUT_IN_MILLISECONDS  = 5000;
long long unsigned int CURRENT_TIME                 = 0x0;

void printLine();
void processSerialInput();
String generateLookupPrefix();

void setup() {
  pinMode(LED_PIN, OUTPUT);      // declare LED as output
  pinMode(PIR_INPUT_PIN, INPUT);     // declare sensor as input
 
  Serial.begin(9600);
  /*
  * Connect Bluetooth
  */
  Serial1.begin(9600);
  char init_msg[84];
  sprintf(init_msg, INIT_MSG_STRING, _MODULE_BADGE_ID);
  Serial.write(init_msg);
  Serial1.write(init_msg);
}
 
int val=0, LED_STATE = LOW;


void loop(){

  // Serial.print("Processing input.....");
  if(Serial1.available()){
    Serial.write("Available\n");
    processSerialInput();
  }
  // Serial.print("Processed input.....");

  // Since delay adds a hang in the program, remove this by manipulating @millis() for real-time command parsing
  CURRENT_TIME=millis();

  // Serial.print(CURRENT_TIME);

  if(PIR_ACTIVE && (CURRENT_TIME-STARTING_TIME > PIR_TIMEOUT_IN_MILLISECONDS)){
    int val = digitalRead(PIR_INPUT_PIN);  // read input value
    if (val == HIGH)	{ // check if the input is HIGH
      // digitalWrite(LED_PIN, HIGH);
      LED_STATE = HIGH;  // turn LED ON
      Serial.println("Motion detected!");	// print on output change
      PrepareAndWritePayload(DETECTION_EVENT, EMPTY_STRING);
      // delay(PIR_TIMEOUT_IN_MILLISECONDS);
      STARTING_TIME=CURRENT_TIME;
    }else{
      LED_STATE = LOW;  // turn LED OFF
    }
  }
  if(!PIR_ACTIVE){
    LED_STATE = LOW;  // turn LED OFF
    // Serial.print("IDLE...\n");
  }
  digitalWrite(LED_PIN, LED_STATE);
}

String generateLookupPrefix(){ 
  return String(_MODULE_BADGE_ID) + String(_CMD_TRIGGER_SUFFIX) + String(_SEPERATOR_COLON);
}

void processSerialInput(){
  printLine();
  printLine();

  String input = Serial1.readString();

  Serial.println("RECEIVED INPUT: " + input);

  String __LOOKUP__CMD = generateLookupPrefix();

  size_t inputBufferLength = input.length();
  size_t lookupBufferLength = __LOOKUP__CMD.length();

  if(input.startsWith(__LOOKUP__CMD) && inputBufferLength > lookupBufferLength){
    
    String filteredCommand = input.substring(lookupBufferLength);
    
    if(filteredCommand.startsWith(PIR_START_CMD)){
    
      Serial1.print("STARTING PIR SENSING\n");
      PrepareAndWritePayload(PIR_START_EVENT, EMPTY_STRING);
      PIR_ACTIVE=true;
    
    }else if(filteredCommand.startsWith(PIR_STOP_CMD)){
    
      Serial1.print("STOPPING PIR SENSING\n");
      PrepareAndWritePayload(PIR_STOP_EVENT, EMPTY_STRING);
      PIR_ACTIVE=false;
    
    }else if(filteredCommand.startsWith(PIR_CHANGE_TIMEOUT_CMD)){
    
      PrepareAndWritePayload(PIR_TIMEOUT_CHANGE_EVENT, EMPTY_STRING);
      int timeoutSeconds=filteredCommand.substring(PIR_CHANGE_TIMEOUT_CMD.length()).trim().toInt();

      if(timeoutSeconds > 60){
        // Limit to 60 seconds
        timeoutSeconds=60;
      }

      Serial1.printf("CHANGING PIR SENSING TIME TO: %d\n", timeoutSeconds);
      PIR_TIMEOUT_IN_MILLISECONDS=convertToMillis(timeoutSeconds);
    
    }else if(filteredCommand.startsWith(PIR_VERBOSE_CMD)){
      
      PrepareAndWritePayload(PIR_VERBOSE_CMD, EMPTY_STRING);
      int verbosity=filteredCommand.substring(PIR_VERBOSE_CMD.length()).trim().toInt();

      if(verbosity > 0){
        __LOG_LOG=true;
      }else{
        __LOG_LOG=false;
      }

      Serial1.printf("CHANGING PIR LOG VERBOSITY TO: %d\n", verbosity);
    }
  }else{
    
    Serial1.println("There was some error parsing the command.");
  
  }

  printLine();
  printLine();

}

long long unsigned int convertToMillis(int target){
  return target * 1000;
}

void printLine(){
  int x=0;
  while(x<MAX_LINE_CHAR_SIZE){
    Serial.print("-");
    if(x==MAX_LINE_CHAR_SIZE-1){
      Serial.print("\n");
    }
    x++;
  }
}

const char * HOLDER_EVENT           = "$EVENT";
const char * HOLDER_MODULE_BADGE_ID = "$MODULE_BADGE_ID";
const char * HOLDER_MESSAGE         = "$MESSAGE";

void PrepareAndWritePayload(String event, String message){
  
  String payload="{ 'event': '$EVENT', 'module_badge_id': '$MODULE_BADGE_ID', 'msg': '$MESSAGE' }\n";
  
  payload.replace(HOLDER_EVENT, event);
  payload.replace(HOLDER_MODULE_BADGE_ID, _MODULE_BADGE_ID);
  payload.replace(HOLDER_MESSAGE, message);
  
  char payloadBuffer[payload.length()+1];
  payload.toCharArray(payloadBuffer, payload.length()+1);

  if (__LOG_LOG){
    Serial.write(payloadBuffer);
  }
  Serial1.write(payloadBuffer);
}Agfx6dMfpCOdKBhD



