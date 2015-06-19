//
int measurePin = A0;    // Pin 2 VO of dust sensor to Arduino A0
int mq9Sig = A1;        // MQ9 SIG to Arduino A1
int ledPower = 2;       // Pin 4 LED VCC of dest sensor to Arduino D2
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
  
void setup() {
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
}
  
void loop() {
  float MQ9Value;
  float dustVal = 0;
  float calcVoltage = 0;
  float dustDensity = 0;
  
  MQ9Value = readMQ9();

  dustVal = read_dn7c3ca006();
  calcVoltage = dustVal * (5.0 / 1024.0);
  // Check datasheet
  dustDensity = 0.1724 * (calcVoltage - 0.6) * 1000.0;

  Serial.print("MQ9_volt = ");
  Serial.print(MQ9Value);
  Serial.println("V");

  Serial.print("dn7c3ca006_volt = ");
  Serial.print(calcVoltage);

  Serial.print("  dustVal: ");
  Serial.print(dustVal);

  if (dustVal>36.455) {
    Serial.print("  PM2.5: ");
    // 公式複製自 http://tw.taobao.com/item/44139051810.htm
    Serial.print((float(dustVal/1024)-0.0356)*120000*0.035);
    Serial.print("  Dust Density: ");
    Serial.print(dustDensity);
    Serial.println(" ug/m3 ");
  } else {
    Serial.print("   Invalid voltage: ");
    Serial.println(dustVal);
  }
  delay(1000);
}

float readMQ9() {
  float sensor_volt; 
  float sensorValue;
  sensorValue = 0;
  for(int x = 0 ; x < 100 ; x++) {
    sensorValue += analogRead(mq9Sig);
  }
  sensorValue = sensorValue/100.0;
  sensor_volt = sensorValue/1024*5.0;
  return sensor_volt;
}

float read_dn7c3ca006() {
  float dust;
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
  dust = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  return dust;
}
