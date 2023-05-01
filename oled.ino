
void initialize_oled(){
  display.begin(SSD1306_SWITCHCAPVCC);

  display_connecting_status();
}

void display_connecting_status(){
  display.clearDisplay();

  // Connecting text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25,30);
  display.println("Connecting...");
  display.display();
}

void display_heating_sensor(){
  display.clearDisplay();

  // Connecting text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15,30);
  display.println("Heating sensor...");
  display.display();
}

void set_display_status(String ppm_status){
  display.clearDisplay();

  // Header
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30,0);
  display.println("LPG Leakage");

  // Status
  display.setTextSize(2);

  if(ppm_status == "normal") {
    display.setCursor(30,30);
    display.println("NORMAL");
    display.display();
  } else if (ppm_status == "warning") {
    display.setCursor(25,30);
    display.println("WARNING");
    display.display();
  } else {
    display.setCursor(15,30);
    display.println("HAZARDOUS");
    display.display();
  }
}