void Task1(void *pvParameters) {
  for (;;) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10)); // Tạm dừng tránh quá tải CPU
  }
}
// HTML cho giao diện web
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 Control</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                margin-top: 50px;
                background-color: lightblue;
            }
            .button {
                display: inline-block;
                width: 100px;
                height: 100px;
                border-radius: 50%;
                border: none;
                color: white;
                font-size: 16px;
                margin: 10px;
                cursor: pointer;
                background-color: red;
            }
            .button.active {
                background-color: green;
            }
            .button.auto-on {
                background-color: green;
            }
            .button.auto-off {
                background-color: blue;
            }
            .sensor {
                font-size: 18px;
                margin: 10px 0;
            }
        </style>
        <script>
            async function updateButton(buttonId) {
                const formData = new FormData();
                formData.append("button", buttonId);
                await fetch('/update', { method: 'POST', body: formData });
            }

            async function connectWiFi() {
                const ssid = document.getElementById("ssid").value;
                const password = document.getElementById("password").value;
                const formData = new FormData();
                formData.append("ssid", ssid);
                formData.append("password", password);
                await fetch('/connect', { method: 'POST', body: formData });
                alert('Đang cố gắng kết nối đến Wi-Fi mới!');
            }

            function syncButtons(status) {
                document.getElementById("buttonOut").classList.toggle('active', status.Go_Out === 1);
                document.getElementById("buttonIn").classList.toggle('active', status.Go_In === 1);
                document.getElementById("buttonMode").className = 'button ' + (status.autoMode ? 'auto-on' : 'auto-off');
            }

            async function fetchStatus() {
                const response = await fetch('/status');
                const status = await response.json();
                syncButtons(status);
            }

            setInterval(fetchStatus, 500);
        </script>
    </head>
    <body>
        <h1>ESP32 Button Control</h1>
        <button id="buttonOut" class="button" onclick="updateButton(1)">OUT</button>
        <button id="buttonIn" class="button" onclick="updateButton(2)">IN</button>
        <button id="buttonMode" class="button auto-off" onclick="updateButton(3)">MODE</button>
        
        <h2>Nhập thông tin Wi-Fi mới</h2>
        SSID: <input type="text" id="ssid"><br>
        Mật khẩu: <input type="password" id="password"><br>
        <button onclick="connectWiFi()">Kết nối</button>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// Cập nhật trạng thái từ giao diện web
void handleUpdate() {
  if (server.hasArg("button")) {
    int buttonId = server.arg("button").toInt();
    if (buttonId == 1) {
      Go_Out = 1;
      Go_In = 0;
      Serial.println("OUT clicked from web.");
    } else if (buttonId == 2) {
      Go_In = 1;
      Go_Out = 0;
      Serial.println("IN clicked from web.");
    } else if (buttonId == 3) {
      autoMode = !autoMode;
      Go_In = 0;
      Go_Out = 0;
      Serial.printf("MODE clicked from web. AutoMode = %d\n", autoMode);
    }
  }
  server.send(200, "text/plain", "OK");
}

// Xử lý kết nối Wi-Fi
void handleConnect() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String newSSID = server.arg("ssid");
    String newPassword = server.arg("password");

    // Lưu thông tin Wi-Fi mới vào Preferences
    preferences.begin("wifi", false);
    preferences.putString("ssid", newSSID);
    preferences.putString("password", newPassword);
    preferences.end();

    Serial.print("Kết nối đến Wi-Fi: ");
    Serial.println(newSSID);

    // Ngắt kết nối khỏi Access Point
    WiFi.softAPdisconnect(true);

    // Kết nối đến Wi-Fi mới
    WiFi.begin(newSSID.c_str(), newPassword.c_str());

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
      Serial.println("Connecting to WiFi...");
      BUZZER.attach(0.5, toggleLed);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Kết nối thành công!");
      Serial.print("Địa chỉ IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("Kết nối thất bại!");
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleStatus() {
  // Sử dụng String để ghép các phần của JSON
  String json = "{\"Go_Out\":" + String(Go_Out) +
                ",\"Go_In\":" + String(Go_In) +
                ",\"autoMode\":" + String(autoMode) +
                ",\"giaTriU\":" + String(giaTriU) +
                ",\"giaTriI\":" + String(giaTriI) +
                ",\"giaTriP\":" + String(giaTriP) + "}";
  // Gửi phản hồi JSON
  server.send(200, "application/json", json);
}
