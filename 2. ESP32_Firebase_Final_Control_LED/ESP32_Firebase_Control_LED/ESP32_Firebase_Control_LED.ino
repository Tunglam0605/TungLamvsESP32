#include <WiFi.h>
#include <FirebaseESP32.h>
/*
Tên thư viện: Firebase ESP32 Client
Version: 3.9.6
*/
#define LED 2

// Khai báo thông tin đăng nhập WiFi
#define WIFI_SSID "Automatic"
#define WIFI_PASSWORD "0325270213"

#define FIREBASE_HOST "iot-basic-83481-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "b0xkNbviGv8A39ytWEpds1FO0rCMNYW9ddn03Pej"
// Khai báo đối tượng Firebase
FirebaseData firebaseData;
String path ="/";

void setup() {

  pinMode(LED,OUTPUT);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if(!Firebase.beginStream(firebaseData,path))
  { 
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
 else 
 {
    Serial.println("Đã kết nối Firebase");
  }

}
void loop(){
      Firebase.setInt(firebaseData, "/number1",random(0,100));
      Firebase.setInt(firebaseData, "/number2",random(0,100));
      Firebase.setInt(firebaseData, "/number3",random(0,100));
 
   if (Firebase.getInt(firebaseData, "/LED_STATUS")) {
    int ledState = firebaseData.intData();
    Serial.print("Received LED_STATUS value: ");
    Serial.println(ledState); // In ra giá trị nhận được từ Firebase
    if (ledState == 1) {
      digitalWrite(LED, HIGH); // Bật LED 1
      Serial.println("Turning LED ON");
    } else {
      digitalWrite(LED, LOW); // Tắt LED 1
      Serial.println("Turning LED OFF");
    }
  }
  

} 