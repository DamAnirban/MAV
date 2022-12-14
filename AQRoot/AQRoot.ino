
#include "painlessMesh.h"
#include <Arduino_JSON.h>

// MESH Details
#define   MESH_PREFIX     "whateverYouLike" //name for your MESH
#define   MESH_PASSWORD   "somethingSneaky" //password for your MESH
#define   MESH_PORT       5555 //default port

int led_arr[] = {0,0,0,0,0,0,0,0,0};
int sens_arr[] = {0,0,0,0,0,0,0,0,0};
int occupied;
int vacant;

//Number for this node, 7575 for root
int nodeNumber = 7575;

//String to send to other nodes with sensor readings
String readings;

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String getReadings(int code); // Prototype for sending sensor readings

//Create tasks: to send messages and get readings;
Task taskSendMessage(TASK_SECOND * 2 , TASK_FOREVER, &sendMessage);

String getReadings (int code) {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  //jsonReadings["code"] = code;
  
  if(code == 1)
  {
    jsonReadings["led1"] = led_arr[0];
    jsonReadings["led2"] = led_arr[1];
    jsonReadings["led3"] = led_arr[2];
  }
  else if(code == 2)
  {
    jsonReadings["led1"] = led_arr[3];
    jsonReadings["led2"] = led_arr[4];
    jsonReadings["led3"] = led_arr[5];
  }
  else if(code == 3)
  {
    jsonReadings["led1"] = led_arr[6];
    jsonReadings["led2"] = led_arr[7];
    jsonReadings["led3"] = led_arr[8];
  }
  
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage () {
  // all calculations here.
  int code = 1;
  String msg = getReadings(code);
  mesh.sendBroadcast(msg);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  //Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  int s1 = myObject["sensor1"];
  int s2 = myObject["sensor2"];
  int s3 = myObject["sensor3"];
    
  if(node == 1)
  {
    sens_arr[0] = s1;
    sens_arr[1] = s2;
    sens_arr[2] = s3;
  }
  else if(node == 2)
  {
    sens_arr[3] = s1;
    sens_arr[4] = s2;
    sens_arr[5] = s3;
  }
  else if(node == 3)
  {
    sens_arr[6] = s1;
    sens_arr[7] = s2;
    sens_arr[8] = s3;
  }
  /*
  for(int i = 0; i<=8 ; i++)
  {
    Serial.print(sens_arr[i]);
    Serial.print(" - ");
  }*/
  //Serial.println(" ");
  Serial.print(sens_arr[0]);
  Serial.print("-"); Serial.print(sens_arr[1]);
  Serial.print("-"); Serial.print(sens_arr[3]);
  Serial.print("-"); Serial.println(sens_arr[4]);
}

void newConnectionCallback(uint32_t nodeId) {
  //Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  //Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
