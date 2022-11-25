//Including the wire library allows us to communicate with devices through I2C
//On the Uno we have to use pins A4 for SDA (data) and A5 for SCL (clock)
#include<Wire.h>

//this is the default address to access the MPU6050. This is found by looking at the chips data sheet.
//If we wanted to use an additional MPU6050, we could attach to the AD0 pin and apply 3.3 volts
//This will change that chips address to 0x69
const int MPU_addr=0x68; 

//The following integers will hold the data received from the MPU
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

int minVal=265;
int maxVal=402;

//these will hold our x,y, and z angles in the end
double x;
double y;
double z;

// comment or uncomment to enable/disable output for debugging
#define DEBUG
//#define ADVDEBUG

// the amount of time that we want to pause between reading our values.
const unsigned long delay_time = 400;
unsigned long prevMillis = 0;

void setup(){
  Wire.begin();//this opens our I2C communications and only needs to be called once
  Wire.beginTransmission(MPU_addr); //this sets up the transmission to occur on the given address
  Wire.write(0x6B); //looking at the data sheet we can see that sending this bytw to the MPU resets the device
  Wire.write(0); //after reset, this essentially wakes the MPU
  Wire.endTransmission(true); //end transmission. the device should be ready to go
  
  #ifdef DEBUG || ADVDEBUG
    //begin our serial monitor for data conformation
    Serial.begin(9600);
  #endif
}// ------ End setup()

void loop(){
  if (millis() - prevMillis >= delay_time) {
    readMPU();

    #ifdef DEBUG
       Serial.print("AngleX= ");
       Serial.println(x);
  
       Serial.print("AngleY= ");
       Serial.println(y);
  
       Serial.print("AngleZ= ");
       Serial.println(z);
       Serial.println("-----------------------------------------");
    #endif
  }
  
} // ------ End loop()

void readMPU() { //this function is called each time we want to update out position
  
  Wire.beginTransmission(MPU_addr);//again, we start talking with the MPU
  Wire.write(0x3B); //here we let the MPU know that we want to start reading values from this point, which is on the data sheet. this begins at the accelX value.
  Wire.endTransmission(false);//end our transmission because now we want to read in the values that are going to be sent back by the MPU
  Wire.requestFrom(MPU_addr,14,true);//here we tell the MPU that we want it to return 14 values from where we started
  
  //we're going to read those values in 8 bit sequences, which is the full value of each single piece of data sent back to us
  //we created the following variables as 16 bit integers, so after the first 8 bits are read we shift them to the left 8 spaces
  //the remaining 8 bits are then filled with the other half of the data
  //the bit descriptions on the right are again known through looking at the data sheet for the MPU6050
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
    int xAng = map(AcX,minVal,maxVal,-90,90);
    int yAng = map(AcY,minVal,maxVal,-90,90);
    int zAng = map(AcZ,minVal,maxVal,-90,90);
      
      //convert angle from radians to degrees
       x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
       y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
       z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

     #ifdef ADVDEBUG
     //Serial.print("MPU Val ");
     Serial.println(AcX);//-15766, 3D96, 61 150
     #endif
}// ------ End readMPU()
