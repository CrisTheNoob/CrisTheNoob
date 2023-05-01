
void initialize_wifi(){
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  get_device_details();
}

void get_device_details() {
  if ((WiFi.status() == WL_CONNECTED)) {
    is_connected = true;

    String path = SERVER_IP + "/device/?device_id=" + device_id;
    http.begin(client, path.c_str());
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    if (httpResponseCode>0) {
      // call http get
      String payload = http.getString();

      // deserialize
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      owner_mobile = String(doc["contact_number"]);
      bfp_mobile = String(doc["bfp_number"]);
      full_name = String(doc["full_name"]);
      address = String(doc["address"]);

      Serial.print("full_name: ");
      Serial.println(full_name);
      Serial.print("address: ");
      Serial.println(address);
      Serial.print("owner_mobile: ");
      Serial.println(owner_mobile);
      Serial.print("bfp_mobile: ");
      Serial.println(bfp_mobile);

      http.end();
      return;
    }
    get_device_details();
  } else {
    is_connected = false;
  }
}

void post_data(float ppm) {
  if ((WiFi.status() == WL_CONNECTED)) {
    is_connected = true;

    String path = SERVER_IP + "/ppm/";
    http.begin(client, path.c_str());
    http.addHeader("Content-Type", "application/json");
    http.POST("{\"ppm\":" + String(ppm) + ", \"device_id\": \"" + device_id + "\"}");
    http.end();
  } else {
    is_connected = false;
  }
}

void post_alert(float ppm, String status) {
  if ((WiFi.status() == WL_CONNECTED)) {
    is_connected = true;

    String path = SERVER_IP + "/alerts/";
    http.begin(client, path.c_str());
    http.addHeader("Content-Type", "application/json");
    http.POST("{\"ppm\":" + String(ppm) + ", \"status\":\"" + status + "\",  \"device_id\": \"" + device_id + "\"}");
    http.end();
  } else {
    is_connected = false;
  }
}