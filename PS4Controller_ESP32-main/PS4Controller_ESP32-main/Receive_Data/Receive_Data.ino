#include <PS4Controller.h>

unsigned long lastTimeStamp = 0;
#define TX_PIN 17  // Chân TX cho UART1 (ESP32 → Vi điều khiển nhận)
#define RX_PIN 16  // Chân RX cho UART1 (ESP32 nhận)

HardwareSerial &serialPort = Serial1;  

void notify()
{
  char messageString[200];
  sprintf(messageString, "%4d,%4d,%4d,%4d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%4d,%4d,%3d,%3d,%3d,%3d,%3d,%3d",
  PS4.LStickX(),
  PS4.LStickY(),
  PS4.RStickX(),
  PS4.RStickY(),
  PS4.Left(),
  PS4.Down(),
  PS4.Right(),
  PS4.Up(),
  PS4.Square(),
  PS4.Cross(),
  PS4.Circle(),
  PS4.Triangle(),
  PS4.L1(),
  PS4.R1(),
  PS4.L2Value(),
  PS4.R2Value(),  
  PS4.Share(),
  PS4.Options(),
  PS4.PSButton(),
  PS4.Touchpad(),
  PS4.Charging(),
  PS4.Battery()
  );

  // Chỉ gửi qua UART khi đã qua 20ms để tránh quá tải
  if (millis() - lastTimeStamp > 50)
  {
    serialPort.println(messageString);  // Gửi qua UART1
    Serial.println(messageString);
    lastTimeStamp = millis();
  }
}

void onConnect()
{
  Serial.println("PS4 Connected!");
  serialPort.println("PS4 Connected!");
}

void onDisConnect()
{
  Serial.println("PS4 Disconnected!");  
  serialPort.println("PS4 Disconnected!");
}

void setup() 
{
  Serial.begin(115200);
  serialPort.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);  // Cấu hình UART1 với baudrate 115200

  PS4.attach(notify);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisConnect);
  PS4.begin();
  
  Serial.println("Ready.");
  serialPort.println("Ready.");
}

void loop() 
{
  // Không cần xử lý gì vì đã có callback notify()
}
