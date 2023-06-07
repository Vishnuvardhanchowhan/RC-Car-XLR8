//This is a code of receiver

#include <esp_now.h>
#include <WiFi.h>

// Define variables to store incoming readings
// structure type should match with senders data
typedef struct struct_message {
  int x,y,cmd;
  int spid;
} struct_message;

// Create a structured object in which we will store incoming values
struct_message myData;

struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println(myData.spid);

}

// Use appropriate pins from the Notion ESP32 resources
const int ENA = 22;
const int IN1_2 = 21;
const int IN2_2 = 19;
const int IN3_2 = 18;
const int IN4_2 = 05;


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

    Serial.println("setup stareted");
  // initialize pins as an output.
  pinMode(ENA, OUTPUT);
  pinMode(IN1_2, OUTPUT);
  pinMode(IN2_2, OUTPUT);
  pinMode(IN3_2, OUTPUT);
  pinMode(IN4_2, OUTPUT);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
 
  // Set values that are received
  int value = myData.cmd;
  int spdlvl = myData.spid;
    
  Serial.println(value);
  Serial.println(spdlvl);

  /*
  In the Joystick(transmitter) we mapped the speed output from (0->4095) to (0->5).
  We will now map the speed back to generate PWM
  */
  int spd = map(spdlvl,1,5,100,255); 
    
  if(value==0){
      digitalWrite(IN1_2, LOW); // BOTH THE MOTOR STOPS THUS HALTING THE MOTION
      digitalWrite(IN2_2, LOW);
      digitalWrite(IN3_2, LOW);
      digitalWrite(IN4_2, LOW);
      analogWrite(ENA, spd);
    }
    else if(value==1){
      digitalWrite(IN1_2, LOW); // RIGHT MOTOR TURNS ANTICLOCKWISE AND LEFT MOTOR TURNS CLOCKWISE TAKING SHARP RIGHT TURN
      digitalWrite(IN2_2, HIGH);
      digitalWrite(IN3_2, HIGH);
      digitalWrite(IN4_2, LOW);
      analogWrite(ENA, spd);
    }
    else if(value==2){
      digitalWrite(IN1_2, HIGH);
      digitalWrite(IN2_2, LOW);
      digitalWrite(IN3_2, LOW); // LEFT MOTOR TURNS CLOCKWISE RIGHT MOTOR TURNS ANTICLOCKWISE TAKING SHARP LEFT TURN
      digitalWrite(IN4_2, HIGH);
      analogWrite(ENA, spd);
    }
    else if(value==4){
      digitalWrite(IN1_2, HIGH); // BOTH THE MOTOR ROTATES ANTICLOCKWISE THUS MOVING IN REVERSE DIRECTION
      digitalWrite(IN2_2, LOW);
      digitalWrite(IN3_2, HIGH);
      digitalWrite(IN4_2, LOW);
      analogWrite(ENA, spd);
    }
    else if(value==3){
      digitalWrite(IN1_2, LOW); // BOTH THE MOTOR ROTATES CLOCKWISE THUS MOVING FORWARD DIRECTION
      digitalWrite(IN2_2, HIGH);
      digitalWrite(IN3_2, LOW);
      digitalWrite(IN4_2, HIGH);
      analogWrite(ENA, spd);
    }
  delay(100);
}