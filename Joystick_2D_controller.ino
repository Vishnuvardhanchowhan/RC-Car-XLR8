/*
This code is of transmitter
*/

#include <esp_now.h>
#include <WiFi.h>
// Variables for test data
int JoyStick_X = 34; // Analog Pin  X
int JoyStick_Y = 35; // // Analog Pin  Y
int SpeedPin = 36; // speed pin from other joystick


// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x08, 0xb6, 0x1f, 0x3b, 0x60, 0x10};

// Define variables to store Joystick readings to be sent
typedef struct struct_message {

  int x,y,cmd;
  int spid;

} struct_message;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure

// Create a struct_message called myData to hold joystick readings
struct_message myData;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Init Joystick
  pinMode(JoyStick_X,INPUT);
  pinMode(JoyStick_Y,INPUT);
  pinMode(SpeedPin , INPUT);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
 
  // Set values to send
  int x = analogRead(JoyStick_X); //  X
  int y = analogRead(JoyStick_Y); //  Y
  int speedraw = analogRead(SpeedPin); // speed

// Format structured data
  myData.x = x;   //myData.x now stores value of X
  myData.y = y;   //myData.y now stores value of Y
  myData.cmd = cmd();
  myData.spid= map(speedraw, 0, 4090, 0,5);    // linearly maps (0, 4095) -> (0, 5) which will be using for speed control 
  Serial.println(x);
  Serial.println(y);
  Serial.println(myData.cmd);
  Serial.println(myData.spid);
  Serial.println();
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
    digitalWrite( 2, HIGH);
    Serial.println("\n");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(100);
}

/*
cmd function returns a signle digit value which will then control the direction of the bot
eg. if return 
0 -> dont move
1 -> go forward
*/
int cmd()
{
  int valX = digi(analogRead(JoyStick_X));    // wirte the value of X
  int valY = digi(analogRead(JoyStick_Y));
  // put your main code here, to run repeatedly:

  if ((valX==1)and (valY==1)){  /// valX = 1 and valY = 1 means joystick is in it's original position. Bot should not move
      return 0; // stationary
    }
  if ((valX==1)and (valY==2)){
      return 1; // right
    }
  if ((valX==1)and (valY==0)){
      return 2; // left
    }
  if ((valX==0)and (valY==1)){
      return 3; // back
    }
  if ((valX==2)and (valY==1)){
      return 4; // forward
    }
  else {
    return 0; // eat 5 star do nothing :)
  }
}

/*
digi function divides range of Joystick output into three parts. 
Fill in the appropriate values in the blanks for digi function do so
eg. if ( analval < 1900)  return 0;
    else return 1;

    The above code divides the input into two parts.
*/
int digi(int analval){
    if ( analval <= 1365 )  return 0;
    if((analval  > 1365)and (analval< 2730))  return 1;
    if (analval >= 2730) return 2;
    else return 3;
}