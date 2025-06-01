// CHƯƠNG TRÌNH BẬT TẮT, ĐẢO CHIỀU ĐỘNG 
  void DC_OUT(){
    digitalWrite(PIN_IN_RA,  HIGH);
    digitalWrite(PIN_IN_VAO, LOW);
  }
  void DC_IN(){
    digitalWrite(PIN_IN_RA,  LOW);
    digitalWrite(PIN_IN_VAO, HIGH);
  }
  void DC_STOP(){
    digitalWrite(PIN_IN_RA,  LOW);
    digitalWrite(PIN_IN_VAO, LOW);
  }
// CÒI BÁO ĐỘNG
// Toggle LED state
  void toggleLed() {
    ledState = !ledState;
    digitalWrite(PIN_LED_HOAT_DONG, ledState);
  }
// Toggle LED state
  void phatAmThanhBip() {
    BUZZERState = !BUZZERState;
    digitalWrite(PIN_BUZZER, BUZZERState);
  }