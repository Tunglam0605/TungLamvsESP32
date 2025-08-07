#include <PS4Controller.h>

#define LED_PIN 2    // LED D2 được kết nối với chân số 2 (ESP32)
unsigned long lastTimeStamp = 0;
#define TX_PIN 17    // Chân TX cho UART1 (ESP32 → Vi điều khiển nhận)
#define RX_PIN 16    // Chân RX cho UART1 (ESP32 nhận)

HardwareSerial &serialPort = Serial1;  

// Giá trị của joystick
int LX1;
int LY1;
int rStickX;
int RY1;

int Mode = 0;
int Speed = 995;


// Định nghĩa ngưỡng vùng chết và giới hạn giá trị cho joystick
#define DEADZONE  110
#define MAX_VAL   110

// Thời gian kéo dài tín hiệu (ms)
#define PULSE_DURATION 100

// --- Các biến latch cho từng nút ---
unsigned long latchTime_Left     = 0;
bool latchedLeft                 = false;

unsigned long latchTime_Down     = 0;
bool latchedDown                 = false;

unsigned long latchTime_Right    = 0;
bool latchedRight                = false;

unsigned long latchTime_Up       = 0;
bool latchedUp                   = false;

unsigned long latchTime_Square   = 0;
bool latchedSquare               = false;

unsigned long latchTime_Cross    = 0;
bool latchedCross                = false;

unsigned long latchTime_Circle   = 0;
bool latchedCircle               = false;

unsigned long latchTime_Triangle = 0;
bool latchedTriangle             = false;

unsigned long latchTime_L1       = 0;
bool latchedL1                   = false;

unsigned long latchTime_R1       = 0;
bool latchedR1                   = false;

unsigned long latchTime_Share    = 0;
bool latchedShare                = false;

unsigned long latchTime_Options  = 0;
bool latchedOptions              = false;

unsigned long latchTime_PSButton = 0;
bool latchedPSButton             = false;

unsigned long latchTime_Touchpad = 0;
bool latchedTouchpad             = false;

unsigned long latchTime_Charging = 0;
bool latchedCharging             = false;

// Biến toàn cục lưu trạng thái LED
bool ledState = false;
bool Tocdo = false;

// Các biến lưu trạng thái nút của lần callback trước
bool prevLeft     = false;
bool prevDown     = false;
bool prevRight    = false;
bool prevUp       = false;
bool prevSquare   = false;
bool prevCross    = false;
bool prevCircle   = false;
bool prevTriangle = false;
bool prevL1       = false;
bool prevR1       = false;
bool prevShare    = false;
bool prevOptions  = false;
bool prevPSButton = false;
bool prevTouchpad = false;
bool prevCharging = false;

void notify() {

  // Đọc giá trị joystick
  LX1 = PS4.LStickX();
  LY1 = PS4.LStickY();
  rStickX = PS4.RStickX();
  RY1 = PS4.RStickY();

    if (abs(LX1) < DEADZONE && abs(LY1) < DEADZONE && abs(rStickX) < DEADZONE && abs(RY1) < DEADZONE) { Mode = 0; } // STOP
    else if (RY1 > DEADZONE)  { Mode = 1; } // Tiến
    else if (RY1 < -DEADZONE) { Mode = 2; } // Lùi
    else if (rStickX > DEADZONE)  { Mode = 6; } // Tiến
    else if (rStickX < -DEADZONE) { Mode = 5; } // Lùi
    else if (LX1 > DEADZONE && abs(LY1) < MAX_VAL) {  // Di chuyển sang phải
      if (PS4.R2Value() > 200 && PS4.L2Value() < 200) {
        Mode = 6;  // R2 cao quay phải
        }
      else if (PS4.L2Value() > 200 && PS4.R2Value() < 200) {
        Mode = 5;  // L2 cao quay trái
        }
      else {
        Mode = 3;  // R2 và L2 đều thấp
        }
      }
    else if (LX1 < -DEADZONE && abs(LY1) < MAX_VAL) {  // Di chuyển sang trái
      if (PS4.R2Value() > 200 && PS4.L2Value() < 200) {
        Mode = 6;  // R2 cao quay phải
        }
      else if (PS4.L2Value() > 200 && PS4.R2Value() < 200) {
        Mode = 5;  // L2 cao quay trái
        }
      else {
        Mode = 4;  // R2 và L2 đều thấp
        }
      }

    else if (LY1 > DEADZONE && abs(RY1) < MAX_VAL) {
        if (PS4.R2Value() > 200 && PS4.L2Value() < 200) {
        Mode = 7;  // R2 cao Tiến phải
      }
      else if (PS4.L2Value() > 200 && PS4.R2Value() < 200) {
        Mode = 9;  // L2 cao Tiến trái
      }
      else {
        Mode = 1;  // R2 và L2 đều thấp
      }
    }
    else if (LY1 < -DEADZONE && abs(RY1) < MAX_VAL) {
      if (PS4.R2Value() > 200 && PS4.L2Value() < 200) {
        Mode = 8;  // R2 cao Lùi phải
      }
      else if (PS4.L2Value() > 200 && PS4.R2Value() < 200) {
        Mode = 10;  // L2 cao Lùi trái
      }
      else {
        Mode = 2;  // R2 và L2 đều thấp
      }
    }


  // Edge detection cho từng nút (chỉ đúng trong lần callback đầu khi chuyển từ false -> true)
  bool newLeft     = PS4.Left()     && !prevLeft;
  bool newDown     = PS4.Down()     && !prevDown;
  bool newRight    = PS4.Right()    && !prevRight;
  bool newUp       = PS4.Up()       && !prevUp;
  bool newSquare   = PS4.Square()   && !prevSquare;
  bool newCross    = PS4.Cross()    && !prevCross;
  bool newCircle   = PS4.Circle()   && !prevCircle;
  bool newTriangle = PS4.Triangle() && !prevTriangle;
  bool newL1       = PS4.L1()       && !prevL1;
  bool newR1       = PS4.R1()       && !prevR1;
  bool newShare    = PS4.Share()    && !prevShare;
  bool newOptions  = PS4.Options()  && !prevOptions;
  bool newPSButton = PS4.PSButton() && !prevPSButton;
  bool newTouchpad = PS4.Touchpad() && !prevTouchpad;
  bool newCharging = PS4.Charging() && !prevCharging;

  // Cập nhật trạng thái nút cho lần callback sau
  prevLeft     = PS4.Left();
  prevDown     = PS4.Down();
  prevRight    = PS4.Right();
  prevUp       = PS4.Up();
  prevSquare   = PS4.Square();
  prevCross    = PS4.Cross();
  prevCircle   = PS4.Circle();
  prevTriangle = PS4.Triangle();
  prevL1       = PS4.L1();
  prevR1       = PS4.R1();
  prevShare    = PS4.Share();
  prevOptions  = PS4.Options();
  prevPSButton = PS4.PSButton();
  prevTouchpad = PS4.Touchpad();
  prevCharging = PS4.Charging();

  // --- Xử lý pulse stretching (latching) cho từng nút ---

  // Sử dụng biến cờ cho mỗi nút để giữ tín hiệu 1 trong PULSE_DURATION
  // Nút Left
  if (newLeft) {
    latchedLeft = true;
    latchTime_Left = millis();
  }
  if (millis() - latchTime_Left >= PULSE_DURATION) {
    latchedLeft = false;
  }

  // Nút Down
  if (newDown) {
    latchedDown = true;
    latchTime_Down = millis();
  }
  if (millis() - latchTime_Down >= PULSE_DURATION) {
    latchedDown = false;
  }

  // Nút Right
  if (newRight) {
    latchedRight = true;
    latchTime_Right = millis();
  }
  if (millis() - latchTime_Right >= PULSE_DURATION) {
    latchedRight = false;
  }

  // Nút Up
  if (newUp) {
    latchedUp = true;
    latchTime_Up = millis();
  }
  if (millis() - latchTime_Up >= PULSE_DURATION) {
    latchedUp = false;
  }

  // Nút Square
  if (newSquare) {
    latchedSquare = true;
    latchTime_Square = millis();
  }
  if (millis() - latchTime_Square >= PULSE_DURATION) {
    latchedSquare = false;
  }

  // Nút Cross
  if (newCross) {
    latchedCross = true;
    latchTime_Cross = millis();
  }
  if (millis() - latchTime_Cross >= PULSE_DURATION) {
    latchedCross = false;
  }

  // Nút Circle
  if (newCircle) {
    latchedCircle = true;
    latchTime_Circle = millis();
  }
  if (millis() - latchTime_Circle >= PULSE_DURATION) {
    latchedCircle = false;
  }

  // Nút Triangle
  if (newTriangle) {
    latchedTriangle = true;
    latchTime_Triangle = millis();
  }
  if (millis() - latchTime_Triangle >= PULSE_DURATION) {
    latchedTriangle = false;
  }

  // Nút L1
  if (newL1) {
    latchedL1 = true;
    latchTime_L1 = millis();
  }
  if (millis() - latchTime_L1 >= PULSE_DURATION) {
    latchedL1 = false;
  }

  // Nút R1
  if (newR1) {
    latchedR1 = true;
    latchTime_R1 = millis();
  }
  if (millis() - latchTime_R1 >= PULSE_DURATION) {
    latchedR1 = false;
  }

  // Nút Share
  if (newShare) {
    latchedShare = true;
    latchTime_Share = millis();
  }
  if (millis() - latchTime_Share >= PULSE_DURATION) {
    latchedShare = false;
  }

  // Nút Options
  if (newOptions) {
    latchedOptions = true;
    latchTime_Options = millis();
  }
  if (millis() - latchTime_Options >= PULSE_DURATION) {
    latchedOptions = false;
  }

  // Nút PSButton
  if (newPSButton) {
    latchedPSButton = true;
    latchTime_PSButton = millis();
  }
  if (millis() - latchTime_PSButton >= PULSE_DURATION) {
    latchedPSButton = false;
  }

  // Nút Touchpad
  if (newTouchpad) {
    latchedTouchpad = true;
    latchTime_Touchpad = millis();
  }
  if (millis() - latchTime_Touchpad >= PULSE_DURATION) {
    latchedTouchpad = false;
  }

  // Nút Charging
  if (newCharging) {
    latchedCharging = true;
    latchTime_Charging = millis();
  }
  if (millis() - latchTime_Charging >= PULSE_DURATION) {
    latchedCharging = false;
  }

  // --- LED Toggle ---  
  // Để tránh toggle nhiều lần nếu nhiều nút cùng lúc được press, ta dùng cờ chung:
  bool anyNewEvent = newLeft || newDown || newRight || newUp ||
                     newSquare || newCross || newCircle || newTriangle ||
                     newL1 || newR1 || newShare || newOptions ||
                     newPSButton || newTouchpad || newCharging;
                     
  if (anyNewEvent) {
    // Chỉ toggle LED 1 lần khi có sự kiện mới xuất hiện
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  if (newTriangle) {
    Tocdo = !Tocdo;
      switch(Tocdo) {
        case 0: Speed = 850;
          break;
        case 1: Speed = 820;
          break;
        }
      }

  // --- Định dạng chuỗi dữ liệu gửi đi ---  
  // Sử dụng các biến latched (đã kéo dài tín hiệu) để gửi trạng thái nút
  char messageString[200];
  sprintf(messageString, "%d,%d,%d,%d,%2d,%d,%d,%d,%d,%d,%d,%4d,%4d,%d,%d",
    PS4.Up(),        // Nút Up 
    PS4.Right(),     // Nút Right
    PS4.Down(),      // Nút Down
    PS4.Left(),      // Nút Left
    Mode,            // Mode 
    PS4.Triangle(),  // Nút Tam giác
    PS4.Circle(),    // Nút Tròn
    PS4.Cross(),     // Nút X
    PS4.Square(),    // Nút Vuông    
    PS4.R1(),        // Nút R1
    PS4.L1(),        // Nút L1
    PS4.L2Value(),   // Giá trị analog của nút L2
    PS4.R2Value(),   // Giá trị analog của nút R2
    Speed,
    PS4.Battery()    // Trạng thái pin của controller
  );                          

  // Gửi dữ liệu qua UART và Serial mỗi 50ms
  if (millis() - lastTimeStamp > 38) {
    serialPort.println(messageString);
    Serial.println(messageString);
    lastTimeStamp = millis();
  }                               
}                                     

void onConnect() {
  Serial.println("PS4 Connected!");
  serialPort.println("PS4 Connected!");
  digitalWrite(LED_PIN, HIGH);

}

void onDisConnect() {
  Serial.println("PS4 Disconnected!");
  serialPort.println("PS4 Disconnected!");
  digitalWrite(LED_PIN, LOW);

}

void setup() {
  Serial.begin(115200);
  serialPort.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  
  // Cấu hình chân LED D2
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Khởi tạo LED tắt
  
  PS4.attach(notify);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisConnect);
  PS4.begin();
  
  Serial.println("Ready.");
  serialPort.println("Ready.");
}

void loop() {
  // Không cần xử lý gì trong loop vì đã dùng callback từ PS4Controller
}
