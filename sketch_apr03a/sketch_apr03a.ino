
float Vol =0 ,Ampe,K=0,P=0,P_=0,Q=5,R=50,x_hat=0,abc=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  ///////////////////////////////////////////////////////////
  /////////// set lai gia tri doc ADC ///////////////////////
  analogReference(EXTERNAL);

}

void loop()
{
  // put your main code here, to run repeatedly:
  
  concentration_dust();
  

}

void concentration_dust()    
  {
       /////////// do nong do bui
  
   
      
   Vol = analogRead(A0)*2.0/1024.0 ;

   x_hat = Vol*100 + 0.4;
  
  
  ///////////////////// ---------- Kalman filter ---------------------//////////////////////////////
  
 // P_ = P + Q;
 // K = P_/ (P_ + R);
 // x_hat = (x_hat + K*((float) Vol - x_hat)) + 0.4; 
   
//  P = (1 -K)*P_ ;
  
  /////////////////// --------------------------------------------------------------------

  Serial.println(" ADC");
 //Serial.write (0x22);
  Serial.print(x_hat);
// Serial.write(0x22);
 // Serial.write(0xff);
 // Serial.write(0xff);
 //Serial.write(0xff);
   delay(100);
  }

