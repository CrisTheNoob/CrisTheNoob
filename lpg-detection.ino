#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <MQUnifiedsensor.h>
#include <SoftwareSerial.h>  
#include <AsyncTimer.h>


// WiFi
#ifndef STASSID
#define STASSID "PLDT_Home_40D81"
#define STAPSK  "pldthome"
#endif

// OLED
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_MOSI     D7
#define OLED_CLK      D5
#define OLED_DC       D2
#define OLED_CS       D8
#define OLED_RESET    D3

// GSM
#define RX                  D4
#define TX                  D0

// Buzzer
#define BUZZER              D1

// MQ2 Sensor
#define MQ2_SENSOR          A0
#define Board               ("ESP8266")
#define Pin                 (A0)  
#define Type                ("MQ-2") //MQ2
#define Voltage_Resolution  (3.3) // 3V3 <- IMPORTANT
#define ADC_Bit_Resolution  (10) // For ESP8266
#define RatioMQ2CleanAir    (9.83)


// PPM Status Constants
#define NORMAL_STATUS     "normal"
#define WARNING_STATUS    "warning"
#define HAZARDOUS_STATUS  "hazardous"

// Intiantiate
AsyncTimer timer;
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
SoftwareSerial GSM(RX, TX);
WiFiClient client;
HTTPClient http;
MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

// Macros
String SERVER_IP = "http://lpg-detection.herokuapp.com";
// String SERVER_IP = "http://10.0.0.33:8000";
bool is_connected = false;
unsigned long last_millis = millis();
unsigned long last_sms_timestamp = last_millis;

const String device_id = "LPG0003";
String owner_mobile;
String bfp_mobile;
String full_name;
String address;

// // PPM 500 to 999
// String warning_mesage = "We have detected an UNUSUAL gas level from your area. Please make sure your tank regulator is OFF!";
// // PPM >=1000
// String hazardous_message = "We have detected a HAZARDOUS gas level in your area!  Please make sure your tank regulator is OFF!";



void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  // Initialize modules
  initialize_oled();
  initialize_wifi();
  initialize_sensor();
  initialize_buzzer();
  initialize_gsm();
}

void loop() {
  unsigned long current_millis = millis();

  // handle timer
  timer.handle();

  // Read LPG PPM
  MQ2.update();
  float ppm = MQ2.readSensor() - 100;
  ppm = ppm < 0 ? 0 : ppm;
  String ppm_status = get_ppm_status(ppm);


  // Update display
  if (is_connected) {
    set_display_status(ppm_status);
  } else {
    display_connecting_status();
  }

  // Buzzer
  buzzer_tone(ppm_status);

  // Post data to server every 500ms
  if (current_millis - last_millis > 1000) {
    post_data(ppm);
    last_millis = current_millis;
  }

  // HAZARDOUS
  // Alert owner and BPF
  if (current_millis - last_sms_timestamp > 30000 && ppm_status == HAZARDOUS_STATUS) {
    post_alert(ppm, ppm_status);

    String message_to_owner = "We have detected a HAZARDOUS gas levels from your area. Please make sure your tank regulator is OFF!";
    send_sms(owner_mobile, message_to_owner);

    String message_to_bfp = "We have detected a HAZARDOUS gas levels at " + full_name +  "'s area - " + address + ". Please do the necessary actions to inform the user and mitigate the risk of catching a fire!";
    // String message_to_bfp = full_name + address;
    send_sms(bfp_mobile,  message_to_bfp);
    last_sms_timestamp = current_millis;
  }

  // WARNING
  // Alert owner
  if (current_millis - last_sms_timestamp > 30000 && ppm_status == WARNING_STATUS) {
    post_alert(ppm, ppm_status);

    String message_to_owner = "We have detected an UNUSUAL gas levels from your area. Please make sure your tank regulator is OFF!";
    send_sms(owner_mobile, message_to_owner);
    last_sms_timestamp = current_millis;
  }
}
