/*
ADXL335
note:vcc--&gt;5v ,but ADXL335 Vs is 3.3V
The circuit:
      5V: VCC
analog 0: x-axis
analog 1: y-axis
analog 2: z-axis
*/

const int xpin = A1;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A3;                  // z-axis (only on 3-axis models)

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);
}

void loop()
{
  int x = analogRead(xpin);  //read from xpin
  delay(1); //
  int y = analogRead(ypin);  //read from ypin
  delay(1);  
  int z = analogRead(zpin);  //read from zpin
   
  float zero_G = 512.0; //ADC is 0~1023  the zero g output equal to Vs/2
                        //ADXL335 power supply by Vs 3.3V
  float scale = 102.3;  //ADXL335330 Sensitivity is 330mv/g
                         //330 * 1024/3.3/1000  
                         
  
//  x /= 3300.0; 
//  y /= 3300.0;
//  z /= 3300.0;  
                         
  Serial.print("x:");
  Serial.print(x); 
  Serial.print("\ty:");
  Serial.print(y);
  Serial.print("\tz:");
  Serial.print(z);  
  Serial.print("\n");
  
  // Output is mV/g, min270 typical300 max330
  // 3300 mV/g
  
  /*Serial.print(((float)x - 331.5)/65*9.8);  //print x value on serial monitor
  Serial.print("\t");
  Serial.print(((float)y - 329.5)/68.5*9.8);  //print y value on serial monitor
  Serial.print("\t");
  Serial.print(((float)z - 340)/68*9.8);  //print z value on serial monitor
  Serial.print("\n");
  */
  
  delay(20);  //wait for 1 second 
}

	


	

