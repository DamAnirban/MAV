/*
#include <ESPAsyncTCPbuffer.h>
#include <tcp_axtls.h>
#include <SyncClient.h>
#include <DebugPrintMacros.h>
#include <async_config.h>
#include <ESPAsyncTCP.h>
#include <AsyncPrinter.h>
*/
// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 

const int potPin2 = 13;
const int potPin3 = 27;
int potValue2 = 0;
int potValue3 = 0;
// variable for storing the potentiometer value
  

#define tINT 3    


#define pingDelay 50      
#define debugDelay 500   

                   
unsigned long lastPollMillis;
unsigned long lastDebugMillis;
int sensor_data[] = {0, 0, 0};


//*************************Mesh network***********************************

#include "painlessMesh.h"
#include <Arduino_JSON.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

//Number for this node
int nodeNumber = 2;

//String to send to other nodes with sensor readings
String readings;

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String getReadings(); // Prototype for sending sensor readings

Task taskSendMessage( TASK_SECOND * 2 , TASK_FOREVER, &sendMessage );

String getReadings () {          // message that is broadcasted
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["sensor1"] = sensor_data[0];
  jsonReadings["sensor2"] = sensor_data[1];
  jsonReadings["sensor3"] = sensor_data[2];
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage() {
  String msg = getReadings();
  mesh.sendBroadcast( msg );
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double led1 = myObject["led1"];
  double led2 = myObject["led2"];
  double led3 = myObject["led3"];
  Serial.print("Node: ");
  Serial.print(node);
  Serial.print("Led1 should be: ");
  Serial.println(led1);
  Serial.print("Led2 should be: ");
  Serial.print(led2);
  Serial.println("Led3 should be");
  Serial.print("led3: ");
  Serial.print(led2);
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {

  //***************************mesh network**************************************
  Serial.begin(115200);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  
  lastPollMillis = millis();
  lastDebugMillis = millis();
  pinMode(potPin3, INPUT);
    pinMode(potPin3, INPUT);
  delay(1000);
}



void loop() {
  // it will run the user scheduler as well
  mesh.update();
  potValue2 = digitalRead(potPin2);
  potValue3 = digitalRead(potPin3);
  Serial.print(potValue2);
  Serial.print(' ');
  Serial.println(potValue3);
  sensor_data[0] = potValue2;
  sensor_data[1] = potValue3;
  sensor_data[2] = 0;
  Serial.print("*********");
  Serial.print(sensor_data[0]);
  Serial.print(" ");
  Serial.println(sensor_data[1]);
  
}
