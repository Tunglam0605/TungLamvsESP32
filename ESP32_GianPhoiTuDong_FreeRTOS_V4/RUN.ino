
void Task3(void *pvParameters) {
  for (;;) {
    switch (autoMode)
    {
      case 0:    // CHẾ ĐỘ CHẠY TỰ ĐỘNG
          digitalWrite(PIN_LED, HIGH); // LED báo hoạt động
          digitalWrite(PIN_LED_HOAT_DONG, LOW); // LED báo chế độ tự động

          // ĐỘNG CƠ ĐI RA KHI NHẤN NÚT OUT
            if (Go_Out == 1 && CTHT_OUT == 0 && LUONTHUCONG == 1) 
              {
              Go_In = 0;  // KHÓA CHÉO
              digitalWrite(PIN_LED_HOAT_DONG, HIGH); // LED báo chế độ tự động
              DC_OUT();
              Serial.println(" ĐỘNG CƠ ĐANG ĐI RA + SAU 5P CHUYỂN VỀ CHẾ ĐỘ TỰ ĐỘNG");
              }

          // ĐỘNG CƠ ĐI VÀO 
            if (Go_In == 1 && CTHT_IN == 0 && LUONTHUCONG == 1) 
              {
              Go_Out = 0; // KHÓA CHÉO
              digitalWrite(PIN_LED_HOAT_DONG, LOW); // LED báo chế độ tự động
              DC_IN();
              Serial.println(" ĐỘNG CƠ ĐANG ĐI VÀO + SAU 5P CHUYỂN VỀ CHẾ ĐỘ TỰ ĐỘNG");
              }

          // Nếu trời KHÔNG mưa + đủ sáng + CTHT_OUT chưa đóng => ra
            if (((giaTriMua >= doNhayCbm) && (giaTriAnhSang < doNhayCbas) && (CTHT_OUT == 0)) && (LUONTHUCONG == 0)) 
              {
              Go_Out = 1;
              Go_In = 0;  // KHÓA CHÉO
              DC_OUT();
              //Serial.println(" TRỜI KHÔNG MƯA VÀ ĐỦ SÁNG ");
              }
              
          // Nếu trời MƯA HOẶC tối + CTHT_IN chưa đóng => vào
            if (((giaTriMua < doNhayCbm || giaTriAnhSang >= doNhayCbas) && (CTHT_IN == 0)) && (LUONTHUCONG == 0))
              {
              Go_Out = 0; // KHÓA CHÉO
              Go_In = 1;
              DC_IN();
              //Serial.println(" TRỜI MƯA HOẶC TRỜI TỐI ");
              }

          else if ((Go_In == 0 && Go_Out == 0)||(Go_In == 1 && CTHT_IN == 1)||(Go_Out == 1 && CTHT_OUT == 1))
             { 
              DC_STOP(); 
              Serial.println(" stop");
              }

    // VỀ LẠI CHẾ ĐỘ TỰ ĐỘNG SAU 5 PHÚT
              if (millis() - mocThoiGianRESETcomebackAUTO > thoiGianCho )  
              {
              autoMode = 0; 
              LUONTHUCONG = 0; 
              }         
        break;

      case 1: // CHẾ ĐỘ CHẠY BẰNG TAY < NÚT NHẤN >
          digitalWrite(PIN_LED, LOW); // LED báo chế độ tự động
          digitalWrite(PIN_LED_HOAT_DONG, HIGH);// LED báo chế độ bằng tay
          // ĐỘNG CƠ ĐI RA KHI NHẤN NÚT OUT
            if (Go_Out == 1 && CTHT_OUT == 0 && LUONTHUCONG == 1) 
              {
              Go_In = 0;  // KHÓA CHÉO
              DC_OUT();
              Serial.println(" ĐỘNG CƠ ĐANG ĐI RA");
              }

          // ĐỘNG CƠ ĐI VÀO 
            if (Go_In == 1 && CTHT_IN == 0 && LUONTHUCONG == 1) 
              {
              Go_Out = 0; // KHÓA CHÉO
              DC_IN();
              Serial.println(" ĐỘNG CƠ ĐANG ĐI VÀO");
              }
    
          // ĐỘNG CƠ DỪNG KHI KHÔNG ẤN NÚT OUT, IN HAY CÓ 1 CÔNG TẮC HÀNH TRÌNH NÀO ĐÓ TÁC ĐỘNG KẾT HỢP KHÓA CHÉO, QUÁ TẢI
            if ((Go_In == 0 && Go_Out == 0 && LUONTHUCONG == 1)||(Go_In == 1 && CTHT_IN == 1 && LUONTHUCONG == 1)||(Go_Out == 1 && CTHT_OUT == 1 && LUONTHUCONG == 1))
              {
              DC_STOP();
              Serial.println(" ĐỘNG CƠ ĐÃ DỪNG LẠI");
              }   
          break;  
    }

    // CÒI BÁO ĐỘNG QUÁ TẢI 
      if (giaTriP > NGUONG_CONG_SUAT)
        {
        BUZZER.attach(1, toggleLed); 
        DC_STOP();
        Serial.println(" ĐỘNG CƠ ĐÃ DỪNG LẠI DO QUÁ TẢI");
        }    
    vTaskDelay(pdMS_TO_TICKS(10)); // Tạm dừng tránh quá tải CPU
  }
}
