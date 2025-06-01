
void Task2(void *pvParameters) {
  for (;;) {
    // BIẾN LƯU TRẠNG THÁI NÚT NHẤN
      static int lastOutState     = HIGH;
      static int lastInState      = HIGH;
      static int lastAutoState    = HIGH;
      static int lastCTHTOutState = HIGH;
      static int lastCTHTInState  = HIGH;

      int currentOutState     = digitalRead(PIN_NUT_RA);
      int currentInState      = digitalRead(PIN_NUT_VAO);
      int currentAutoState    = digitalRead(PIN_NUT_CHE_DO);
      int currentCTHTOutState = digitalRead(PIN_CTH_OUT);
      int currentCTHTInState  = digitalRead(PIN_CTH_IN);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// CẢM BIẾN ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // ĐỌC GIÁ TRỊ CẢM BIẾN
      do_ap         = analogRead(PIN_Vdc);
      do_dong       = 2 * analogRead(PIN_Idc);
      giaTriBienTro = analogRead(PIN_BIEN_TRO);
    
    // BIẾN ĐỔI GIÁ TRỊ CẢM BIẾN ĐO ĐƯỢC
      giaTriI   = (VCC_CBdong *0.5 - (do_dong * 3.3 / 4095)/ donhayCBdong);
      giaTriU   = (do_ap * VCC_DC) / 4095;
      // Lưu ý: tuỳ mạch offset, ví dụ 512 là mức trung tính, cần hiệu chỉnh
      NGUONG_CONG_SUAT = map(giaTriBienTro, 0, 4095, 0, 150); 
      giaTriP = giaTriI * giaTriU;

    /********** Đọc và lọc Kalman cho cảm biến **********/
      giaTriAnhSangChuaLoc = analogRead(PIN_CBAS);
      giaTriAnhSangDaLoc   = kalmanFilter.updateEstimate(giaTriAnhSangChuaLoc);

      giaTriMuaChuaLoc = analogRead(PIN_CBM);
      giaTriMuaDaLoc   = kalmanFilter.updateEstimate(giaTriMuaChuaLoc);

    // Cập nhật giá trị ánh sáng định kỳ
      if (millis() - mocThoiGianCbas > thoiGianChoCbas)
        {
        giaTriAnhSang   = giaTriAnhSangDaLoc / 4.095;
        mocThoiGianCbas = millis();
        }

    // Cập nhật giá trị mưa định kỳ
      if (millis() - mocThoiGianCbm > thoiGianChoCbm) 
        {
        giaTriMua      = giaTriMuaDaLoc / 4.095;
        mocThoiGianCbm = millis();
        }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// CÔNG TẮC ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // KIỂM TRA CÔNG TẮC HÀNH TRÌNH GIỚI HẠN DC OUT
      if (currentCTHTOutState == LOW && lastCTHTOutState == HIGH) 
        {
        CTHT_OUT = 1;
        Go_Out = 0;
        Serial.println("CTHT OUT pressed.");
        }
      if (currentCTHTOutState == HIGH && lastCTHTOutState == HIGH)  CTHT_OUT = 0;
      lastCTHTOutState = currentCTHTOutState;

    // KIỂM TRA CÔNG TẮC HÀNH TRÌNH GIỚI HẠN DC IN
      if (currentCTHTInState == LOW && lastCTHTInState == HIGH) 
        {
        CTHT_IN = 1;
        Go_In = 0;
        Serial.println("CTHT IN pressed.");
        }
      if (currentCTHTInState == HIGH && lastCTHTInState == HIGH) CTHT_IN = 0;
      lastCTHTInState = currentCTHTInState;

    // KIỂM TRA NÚT NHẤN TRẠNG THÁI DC
      // NÚT BẤM ĐỘNG CƠ ĐI RA
        if (currentOutState == LOW && lastOutState == HIGH) 
          {
          Go_Out = 1;
          Go_In = 0;
          LUONTHUCONG = 1;
          mocThoiGianRESETcomebackAUTO = millis();
          Serial.println("OUT button pressed. + LUONTHUCONG = 1");
          }
        lastOutState = currentOutState;

      // NÚT BẤM ĐỘNG CƠ ĐI VÀO
        if (currentInState == LOW && lastInState == HIGH) 
          {
          Go_In = 1;
          Go_Out = 0;
          LUONTHUCONG = 1;
          mocThoiGianRESETcomebackAUTO = millis();
          Serial.println("IN button pressed. + LUONTHUCONG = 1");
          }
        lastInState = currentInState;

      // NÚT BẤM CHUYỂN ĐỔI CHẾ ĐỘ TỰ ĐỘNG VÀ BẰNG TAY
        if (currentAutoState == LOW && lastAutoState == HIGH ) 
          {
          autoMode = !autoMode;
          Go_In = 0;
          Go_Out = 0;
          Serial.printf("AutoMode toggled. AutoMode = %d\n", autoMode);
          }
        lastAutoState = currentAutoState;
        vTaskDelay(pdMS_TO_TICKS(10)); // Tạm dừng tránh quá tải CPU
      }
    }
    