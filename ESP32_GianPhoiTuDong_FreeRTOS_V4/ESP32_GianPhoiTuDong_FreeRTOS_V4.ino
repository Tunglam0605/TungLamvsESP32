// KHỞI TẠO THƯ VIỆN CẦN DÙNG
  #include <Arduino.h>
  #include <SimpleKalmanFilter.h>
  #include <WiFi.h>
  #include <WebServer.h>
  #include <Ticker.h>
  #include <Preferences.h>

// LƯU WIFI
  Preferences preferences; // Đối tượng lưu thông tin WiFi

// WiFi credentials
const char* defaultSSID = "LAM DAI";      // Tên Wifi mặc định
const char* defaultPassword = "khongnho"; // Mật khẩu Wifi mặc định

// Access Point credentials
  const char* apSSID = "ESP32_AP";   // Tên Access Point
  const char* apPassword = "12345678"; // Mật khẩu Access Point

// Web server
  WebServer server(80);

// ----------- Kalman Filter -----------
  SimpleKalmanFilter kalmanFilter(2, 2, 0.01); // Tham số tuỳ chỉnh

// ----------- Khai báo các chân (PIN) -----------
  #define PIN_CTH_OUT        23  // Công tắc hành trình "OUT"
  #define PIN_CTH_IN         22  // Công tắc hành trình "IN"

  #define PIN_IN_RA          17  // Tín hiệu điều khiển motor
  #define PIN_IN_VAO         16  // Tín hiệu điều khiển motor

  #define PIN_CBM            33  // Cảm biến mưa
  #define PIN_Vdc             4  // Cảm biến điện áp
  #define PIN_Idc            26  // Cảm biến dòng
  #define PIN_CBAS           32  // Cảm biến ánh sáng
  #define PIN_LED            25  // LED hiển thị
  #define PIN_BIEN_TRO       27  // Chân biến trở chỉnh dòng (ADC)
  #define PIN_LED_HOAT_DONG  2   // LED báo hoạt động (nếu cần)

  #define PIN_NUT_CHE_DO     21  // Nút chuyển chế độ
  #define PIN_NUT_RA         19  // Nút ra
  #define PIN_NUT_VAO        18  // Nút vào
  #define PIN_BUZZER         13  // Kết nối còi/buzzer

// Ngưỡng đo so sánh khi các cảm biến đạt thì kích hoạt 
  int   NGUONG_CONG_SUAT   = 120;           // Ngưỡng công suất (W)
  int   doNhayCbas         = 3000;          // Ngưỡng cho cảm biến ánh sáng
  int   doNhayCbm          = 1500;          // Ngưỡng cho cảm biến mưa

  unsigned long thoiGianChoCbas   = 10UL * 60000; // 10 phút
  unsigned long thoiGianChoCbm    = 1000;         // 1 giây
  unsigned long thoiGianCho       = 5UL * 60000;  // 5 phút để trở về chế độ tự động

  unsigned long mocThoiGianCbas = 0;
  unsigned long mocThoiGianCbm  = 0;
  unsigned long mocThoiGian     = 0;

  unsigned long mocThoiGianRESETcomebackAUTO = 0; // MỐC THỜI GIAN SO SÁNH ĐỂ TỰ ĐỘNG CHUYỂN VỀ CHẾ ĐỘ TỰ ĐỘNG SAU KHI CHỜ 5P MÀ KHÔNG NHẤN NÚT NÀO NỮA


// ----------- Thông số cảm biến điện áp, dòng -----------
  const float TI_LE_CAM_BIEN_AP   = 3.3f / 4095.0f;   // Tỉ lệ điện áp
  const float TI_LE_CAM_BIEN_DONG = 30.0f / 4095.0f;  // Tỉ lệ cho ACS712 30A
  int do_dong, do_ap,giaTriBienTro;
  float giaTriP;
  float giaTriU;
  int giaTriI;

// HẰMG SỐ NGUỒN CẤP ĐỘNG LỰC THAY ĐỔI THEO NGUỒN DÙNG
  const float VCC_DC = 12;

// Hằng số tra DataSheet của cảm biến dòng 
    const float VCC_CBdong = 5;     // NGUỒN NUÔI CHO IC ĐO DÒNG CÓ THỂ LÀ 5V HOẶC 3.3V
    const float donhayCBdong = 0.1; // ĐANG ĐỂ LÀ LOẠI ACS712ELCTR-20A-T CÓ ĐỘ NHẠY LÀ 0.1  (mV/A)

/*
  ACS712ELCTR-05B-7 CÓ ĐỘ NHẠY LÀ 1.5  (mV/A)
  ACS712ELCTR-20A-T CÓ ĐỘ NHẠY LÀ 0.1  (mV/A)
  ACS712ELCTR-30A-T CÓ ĐỘ NHẠY LÀ 0.66 (mV/A)
*/

// States BIẾN TRUNG GIAN ĐIỀU KHIỂN TRẠNG THÁI NÚT NHẤN
  int Go_Out      = 0;   // 0: STOP, 1: ĐI RA
  int Go_In       = 0;   // 0: STOP, 1: ĐI VÀO
  int autoMode    = 0;   // 0: AutoMode off, 1: AutoMode on
  int CTHT_OUT    = 0;   // 0: CTHT off, 1: CTHT on
  int CTHT_IN     = 0;   // 0: CTHT off, 1: CTHT on
  bool LUONTHUCONG = 0;  // 0: Luôn thủ công tắt, 1: Bật

// ----------- Biến đọc cảm biến -----------
  float giaTriAnhSangChuaLoc = 0.0f;
  float giaTriAnhSangDaLoc   = 0.0f;
  float giaTriAnhSang        = 0.0f;
  float giaTriMuaChuaLoc     = 0.0f;
  float giaTriMuaDaLoc       = 0.0f;
  float giaTriMua            = 0.0f;

// Ticker for LED blinking
  Ticker ledTicker;
  Ticker BUZZER;

  bool BUZZERState = false;
  bool ledState = false;

// Task Handles
  TaskHandle_t Task1Handle = NULL;
  TaskHandle_t Task2Handle = NULL;
  TaskHandle_t Task3Handle = NULL;

void setup() {
  Serial.begin(115200); // Khởi tạo Serial

  // Khôi phục thông tin Wi-Fi từ Preferences
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", defaultSSID);
  String password = preferences.getString("password", defaultPassword);
  preferences.end();

  // Kết nối đến Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long startAttemptTime = millis();

  // Thử kết nối Wi-Fi
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi. Starting Access Point...");
    WiFi.softAP(apSSID, apPassword);
    Serial.print("Access Point \"");
    Serial.print(apSSID);
    Serial.println("\" started.");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP()); // In địa chỉ IP của Access Point
  }

  // Configure button and LED pins
  pinMode(PIN_CTH_OUT,    INPUT_PULLUP);
  pinMode(PIN_CTH_IN,     INPUT_PULLUP);
  pinMode(PIN_NUT_CHE_DO, INPUT_PULLUP);
  pinMode(PIN_NUT_RA,     INPUT_PULLUP);
  pinMode(PIN_NUT_VAO,    INPUT_PULLUP);

  pinMode(PIN_CBM,  INPUT);
  pinMode(PIN_CBAS, INPUT);
  pinMode(PIN_Idc,  INPUT);
  pinMode(PIN_Vdc,  INPUT);
  
  pinMode(PIN_BUZZER,        OUTPUT);
  pinMode(PIN_LED,           OUTPUT);
  pinMode(PIN_IN_RA,         OUTPUT);
  pinMode(PIN_IN_VAO,        OUTPUT);
  pinMode(PIN_LED_HOAT_DONG, OUTPUT);

  // Tắt buzzer khi khởi động
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_HOAT_DONG, HIGH);

  // Đọc giá trị ánh sáng lúc đầu
  giaTriAnhSang = analogRead(PIN_CBAS) / 4.095;

  // RESET TRẠNG THÁI BAN ĐẦU ĐỘNG CƠ 
    Go_In = 0;
    Go_Out =0; // Reset motor state

  // Setup server routes
  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/connect", HTTP_POST, handleConnect); // Thêm route cho kết nối Wi-Fi
  server.begin();
  Serial.println("Server ready!");

  // Tạo Task 1
  xTaskCreatePinnedToCore(
    Task1,          // Hàm thực hiện task
    "WebSerVer",    // Tên task
    4096,           // Kích thước stack (word)
    NULL,           // Tham số truyền vào (không dùng)
    2,              // Độ ưu tiên (1 thấp, cao hơn là 2, 3, ...)
    &Task1Handle,   // Task handle
    0               // Lõi 0 (hoặc tCORE_NO_AFFINITY để ESP tự quyết định)
  );

  // Tạo Task 2
  xTaskCreatePinnedToCore(
    Task2,          // Hàm thực hiện task
    "Đọc Nút Nhấn", // Tên task
    4096,           // Kích thước stack (word)
    NULL,           // Tham số truyền vào (không dùng)
    1,              // Độ ưu tiên
    &Task2Handle,   // Task handle
    1               // Lõi 1
  );

  // Tạo Task 3
  xTaskCreatePinnedToCore(
    Task3,          // Hàm thực hiện task
    "ATK Đông Cơ",  // Tên task
    4096,           // Kích thước stack (word)
    NULL,           // Tham số truyền vào (không dùng)
    1,              // Độ ưu tiên
    &Task3Handle,   // Task handle
    1               // Lõi 1
  );

}

void loop() {
  // Không cần code trong loop vì các task đã thực hiện song song
}
