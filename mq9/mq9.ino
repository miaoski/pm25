void setup() {
  Serial.begin(9600);
}
 
void loop() {
  float sensor_volt; 
  float RS_air; //  Get the value of RS via in a clear air
  float R0;  // Get the value of R0 via in LPG
  float sensorValue;
 
  sensorValue = 0;
  for(int x = 0 ; x < 100 ; x++) {
    sensorValue += analogRead(A0);
  }
  sensorValue = sensorValue/100.0;
  sensor_volt = sensorValue/1024*5.0;
/*
  RS_air = (5.0-sensor_volt)/sensor_volt; // omit *RL
  R0 = RS_air/9.9; // The ratio of RS/R0 is 9.9 in LPG gas
*/
  Serial.print("sensor_volt = ");
  Serial.print(sensor_volt);
  Serial.println("V");
/*
  Serial.print("R0 = ");
  Serial.println(R0);

  Serial.print("RS_ratio = ");
  Serial.println(RS_air);

  Serial.print("Rs/R0 = ");
  Serial.println(RS_air / 2.85);  // 0.17V as baseline
*/
  delay(1000);
 
}
