// Formula copied from http://tw.taobao.com/item/44139051810.htm
// 警告: 這裡的程式 license 不確定是哪種。
//
int measurePin = 0; //Connect dust sensor to Arduino A0 pin
int ledPower = 2;   //Connect 3 led driver pins of dust sensor to Arduino D2
  
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
  
float dustVal = 0;
float calcVoltage = 0;
float dustDensity = 0;
  
void setup() {
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
}
  
void loop() {
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  dustVal = analogRead(measurePin); // read the dust value
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = dustVal * (5.0 / 1024.0);
  
  // Check datasheet
  dustDensity = 0.1724 * (calcVoltage - 0.6) * 1000.0;

  Serial.print("Raw input voltage: ");
  Serial.print(calcVoltage);

  Serial.print("   dustVal: ");
  Serial.print(dustVal);

  if (dustVal>36.455) {
    Serial.print("   PM2.5: ");
    Serial.print((float(dustVal/1024)-0.0356)*120000*0.035);
    Serial.print("   Dest Density: ");
    Serial.println(dustDensity);
    Serial.println(" ug/m3 ");
  } else {
    Serial.print("   Invalid voltage: ");
    Serial.println(dustVal);
  }
  delay(1000);
}
