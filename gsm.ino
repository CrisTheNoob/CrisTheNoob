
void initialize_gsm() {
  GSM.begin(9600);
}


unsigned int in_queue_millis = 0;

void send_sms(String mobile, String message) {

  Serial.println(mobile);
  Serial.println(message);


  // Send AT
  in_queue_millis += 10000;
  timer.setTimeout([]() {
    Serial.println("AT");
    Serial.println();
    GSM.println("AT");  // Configure Text mode
    clear_buffer();

    in_queue_millis -= 10000;
  },
                   in_queue_millis);

  // Send AT+CMGF
  in_queue_millis += 3000;
  timer.setTimeout([]() {
    Serial.println("AT+CMGF=1");
    Serial.println();
    GSM.println("AT+CMGF=1");  // Configure Text mode
    clear_buffer();

    in_queue_millis -= 3000;
  },
                   in_queue_millis);

  // Send AT+CMGS
  in_queue_millis += 3000;
  timer.setTimeout([mobile]() {
    Serial.println("AT+CMGS=\"" + String(mobile) + "\"");
    Serial.println();
    GSM.println("AT+CMGS=\"" + String(mobile) + "\"");
    clear_buffer();

    in_queue_millis -= 3000;
  },
                   in_queue_millis);

  // Send message
  in_queue_millis += 3000;
  timer.setTimeout([message]() {
    Serial.println(message);
    Serial.println();
    GSM.print(message);  //text content
    clear_buffer();

    in_queue_millis -= 3000;
  },
                   in_queue_millis);

  // Send endline
  in_queue_millis += 3000;
  timer.setTimeout([]() {
    // Serial.println("26");
    Serial.println("26");
    Serial.println();
    GSM.write(26);
    clear_buffer();

    in_queue_millis -= 3000;
  },
                   in_queue_millis);
}

void clear_buffer() {
  ;
  while (GSM.available()) {
    char s = GSM.read();
    Serial.print(s);
  }
}