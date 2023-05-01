void initialize_sensor(){
  display_heating_sensor();
  delay(30000);

  MQ2.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ2.setA(44771); 
  MQ2.setB(-3.245); // Configure the equation to to calculate LPG concentration
  /*
    Exponential regression:
    Gas    | a      | b
    LPG    | 44771  | -3.245
    CH4    | 2*10^31| 19.01
    CO     | 521853 | -3.821
    Alcohol| 0.3934 | -1.504
    Benzene| 4.8387 | -2.68
    Hexane | 7585.3 | -2.849
  */

  MQ2.init();

  /*****************************  MQ CAlibration ********************************************/ 
  // Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++){
    MQ2.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
    // Serial.print(".");
  }
  MQ2.setR0(calcR0/10);
  // Serial.println("  done!.");
  
  if(isinf(calcR0)) while(1);
  if(calcR0 == 0) while(1);
  /*****************************  MQ CAlibration ********************************************/ 
}

String get_ppm_status(float ppm){
  String status = NORMAL_STATUS;
  if (ppm >= 500 && ppm < 1000){
    status = WARNING_STATUS;
  } else if (ppm >= 1000) {
    status = HAZARDOUS_STATUS;
  }
  return status;
}