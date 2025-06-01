//---- khai báo thư viện cần thiết----
#include <WiFi.h>
#include <FirebaseESP32.h>
/*
Tên thư viện: Firebase ESP32 Client
Version: 3.9.6
*/

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define LED 2
#define RL1 12
#define RL2 13
#define RL3 14

// Khai báo thông tin đăng nhập WiFi
#define WIFI_SSID "CLB"
#define WIFI_PASSWORD "56785678"

// Khai báo thông tin đăng nhập Firebase
#define FIREBASE_HOST "esp-32-test-44923-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "HKkOl0jagzkBj0cUDUjWB0dgdD1vCjSb2IDmNOkK"

// Khai báo đối tượng Firebase
FirebaseData firebaseData;
String path ="/";


RF24 radio(4,5);  // nRF24L01 (CE,CSN), CE là chân số 9, CSN là chân số 10

RF24Network network(radio);      // khởi tạo một mạng RF24Network với một đối tượng radio đã được tạo trước đó. 
//Điều này cho phép việc gửi và nhận dữ liệu thông qua mạng RF24Network.

const uint16_t this_node = 00;   //khai báo một biến có tên là "this_node" và gán giá trị là 00 (định dạng bát phân (Octal)), đây là địa chỉ master

byte incomingData[2];//khai báo một mảng byte có với kích thước là 2

void setup() {
  // put your setup code here, to run once:
pinMode(LED,OUTPUT);
pinMode(RL1,OUTPUT);
pinMode(RL2,OUTPUT);
pinMode(RL3,OUTPUT);

Serial.begin(9600);
// Kết nối Wifi
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

// Kiểm tra trạng thái kết nối Wifi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");

// Lấy địa chỉ IP Wifi
  Serial.println(WiFi.localIP());
  Serial.println();

// Khởi tạo kết nối giữa ESP32 và Firebase
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);

// Kết nối lại Wi-Fi cho thư viện Firebase trong trường hợp kết nối mạng bị mất.
  Firebase.reconnectWiFi(true);

// Kiểm tra việc khởi tạo luồng dữ liệu
  if(!Firebase.beginStream(firebaseData,path))
  { 
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
 else 
 {
    Serial.println("Đã kết nối Firebase");
  }

SPI.begin();//khởi tạo bus SPI
radio.begin();//khởi tạo module RF

network.begin(90, this_node);  //(kênh, địa chỉ)

radio.setDataRate(RF24_2MBPS);//thiết lập tốc độ truyền dữ liệu, 2MPS 1MPS 250KPS
}

void loop() {
  // put your main code here, to run repeatedly:
  network.update();//cập nhật trạng thái mạng

while(network.available() ) //kiểm tra xem có dữ liệu nào được gửi đến không?
{
    RF24NetworkHeader header;//Biến này được sử dụng để đại diện cho tiêu đề của gói tin trong mạng RF24Network.
    network.read(header, &incomingData, sizeof(incomingData)); //đọc dữ liệu từ mạng RF24Network và lưu vào biến "incomingData" dựa trên thông tin của biến "header"
    if (header.from_node == 01) { //Cảm biến ánh sáng
      /*biến header.from_node là một thuộc tính của biến "header", nó được sử dụng để lưu trữ thông tin
      về nút gói tin được gửi đến, trong trường hợp này, nếu giá trị của "heade.from_node" là 01, tức là gói tin đến từ Cảm biến ánh sáng.
      */
      Serial.print("Cảm biến ánh sáng: ");
      Serial.println(incomingData[0]);
      Firebase.setInt(firebaseData, "/Cảm biến ánh sáng",incomingData[0]);// gửi dữ liệu lên Firebase
  
      }
    if (header.from_node == 02) {    // Cảm biến nhiệt độ-độ ẩm
          /*biến header.from_node là một thuộc tính của biến "header", nó được sử dụng để lưu trữ thông tin
      về nút gói tin được gửi đến, trong trường hợp này, nếu giá trị của "heade.from_node" là 02, tức là gói tin đến từ Cảm biến nhiệt độ - độ ẩm.
      */
      Serial.print("Cảm biến độ ẩm: ");
      Serial.print(incomingData[0]);
      Firebase.setInt(firebaseData, "/Cảm biến độ ẩm",incomingData[0]);// gửi dữ liệu lên Firebase

      Serial.print(" ");
      Serial.print("Cảm biến nhiệt độ: ");
      Serial.println(incomingData[1]);
      Firebase.setInt(firebaseData, "/Cảm biến nhiệt độ",incomingData[1]);// gửi dữ liệu lên Firebase

    }
    if (header.from_node ==  04) {    // Cảm biến độ ẩm đất
          /*biến header.from_node là một thuộc tính của biến "header", nó được sử dụng để lưu trữ thông tin
      về nút gói tin được gửi đến, trong trường hợp này, nếu giá trị của "heade.from_node" là 04, tức là gói tin đến từ Cảm biến độ ẩm đất.
      */
      Serial.print("Cảm biến độ ẩm đất: ");
      Serial.println(incomingData[0]);
      Firebase.setInt(firebaseData, "/Cảm biến độ ẩm đất",incomingData[0]);// gửi dữ liệu lên Firebase
    }

}
Firebase.getInt(firebaseData, "DataWrite");
int control = firebaseData.intData();
if (control == 1)
{
      digitalWrite(RL1, LOW); // Bật RL1
      Serial.println("Turning RL1 ON");
}
if(control == 2)
{
      digitalWrite(RL1, HIGH); // Tắt RL1
      Serial.println("Turning RL1 OFF");
}

if (control == 3)
{
      digitalWrite(RL2, LOW); // Bật RL2
      Serial.println("Turning RL2 ON");
}
if(control == 4)
{
      digitalWrite(RL2, HIGH); // Tắt RL2
      Serial.println("Turning RL2 OFF");
}

if (control == 5)
{
      digitalWrite(RL3, LOW); // Bật RL3
      Serial.println("Turning RL3 ON");
}
if(control == 6)
{
      digitalWrite(RL3, HIGH); // Tắt RL3
      Serial.println("Turning RL4=3 OFF");
}

if (control == 7)
{
      digitalWrite(RL1, LOW); // Bật RL1
      digitalWrite(RL2, LOW); // Bật RL2
      digitalWrite(RL3, LOW); // Bật RL3
      Serial.println("Bật tất cả");

}
if (control == 8)
{
      digitalWrite(RL1, HIGH); // Tắt RL1
      digitalWrite(RL2, HIGH); // Tắt RL2
      digitalWrite(RL3, HIGH); // Tắt RL3
      Serial.println("Tắt tất cả");

}

/*
       if (Firebase.getInt(firebaseData, "/RL1")) {
    int state1 = firebaseData.intData();
    Serial.print("Received RL1 value: ");
    Serial.println(state1);// In ra giá trị nhận được từ Firebase
    if (state1 == 1) {
      digitalWrite(RL1, LOW); // Bật RL1
      Serial.println("Turning RL1 ON");
    } else {
      digitalWrite(RL1,HIGH ); // Tắt RL1
      Serial.println("Turning RL1 OFF");
    }
  }

     if (Firebase.getInt(firebaseData, "/RL2")) {
    int state2 = firebaseData.intData();
    Serial.print("Received RL2 value: ");
    Serial.println(state2);// In ra giá trị nhận được từ Firebase
    if (state2 == 1) {
      digitalWrite(RL2, LOW); // Bật RL2
      Serial.println("Turning RL2 ON");
    } else {
      digitalWrite(RL2,HIGH ); // Tắt RL2
      Serial.println("Turning RL2 OFF");
    }
  }
  

  if (Firebase.getInt(firebaseData, "/RL3")) {
    int state3 = firebaseData.intData();
    Serial.print("Received RL3 value: ");
    Serial.println(state3);// In ra giá trị nhận được từ Firebase
    if (state3 == 1) {
      digitalWrite(RL3, LOW); // Bật RL3
      Serial.println("Turning RL3 ON");
    } else {
      digitalWrite(RL3,HIGH ); // Tắt RL3
      Serial.println("Turning RL3 OFF");
    }

}*/
}
