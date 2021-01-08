float vref=1.088; //meassured ADC internal voltage referance
float corr=vref/1.11187;  //ADC gain error
float R1=10.11; //
float R2=0.986; //
int offseterror=3; //ADCs offset error
float varduino=5030.0; //Arduino measured 5V dv voltage source
///////////////////////////////////////////////////
int n = 4;  // nuumber of extra bits of ADC resolution with oversampling
int q=1;
void setup() {
  // initialize serial comm
  Serial.begin(9600);
  analogReference(INTERNAL);
  for (int k=0; k<6;k++){
      analogRead(A0);
  }
} 

void loop() {
  float divfactor=R2/(R1+R2);
  int n_samples=1<<(2*n);
  
  if (Serial.available()>0){
    char  a=Serial.read();
    Serial.flush();
    switch(a){
       case '1':
       uint32_t oversampled=0;
       for (int i=1; i<n_samples; i++){
        oversampled=oversampled+(offseterror+analogRead(A0));
       }
       double scaled = (oversampled>>n);
       int sf=1<<n;
       double result=corr*scaled*vref/sf;
       result=(result/divfactor)-varduino;
       Serial.print(q);
       q=q+1;
       break;
       default:
       break;   
            }
           }
}
