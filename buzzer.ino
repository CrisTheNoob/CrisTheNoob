

void initialize_buzzer() {
  pinMode(BUZZER, OUTPUT);
}

void buzzer_tone(String ppm_status){
  bool is_hazardous = ppm_status == HAZARDOUS_STATUS;
  digitalWrite(BUZZER, is_hazardous ? HIGH: LOW);
}