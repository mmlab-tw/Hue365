#include <CapacitiveSensor.h>

/* capacitive sensing threshold and intervals */
#define TOUCH_THRES 40
#define PROX_LOW_THRES 10
#define PROX_HIGH_THRES 250
#define SENSE_INTERVAL 40
#define PROX_SENSE_INTERVAL 120

/* light mode intervals */
#define BLINK_VEL 10
#define BREATH_VEL 800

/* pins on attiny84 */
#define PIN_R 6
#define PIN_G 7
#define PIN_B 8

#define PIN_BASE 2
#define PIN_MODE 4
#define PIN_BRIGHT 3
#define PIN_HUE 5
#define PIN_PROX 9

/* 1: still, 2: blink, 3: breath */
#define MMODE 3 


CapacitiveSensor _cap_hue=CapacitiveSensor(PIN_BASE,PIN_HUE); 
CapacitiveSensor _cap_bright=CapacitiveSensor(PIN_BASE,PIN_BRIGHT); 
CapacitiveSensor _cap_mode=CapacitiveSensor(PIN_BASE,PIN_MODE); 
CapacitiveSensor _cap_prox=CapacitiveSensor(PIN_BASE,PIN_PROX); 

int _mode=0;
int _vhue;
int _vbright=255;
int _vthres;


void changeMode(){  
  _mode=(_mode+1)%MMODE;
}


/* transform hue and brightness into r,g,b values */
void hueToColor(float h_,float br_,int &r_,int &g_,int &b_){

  /* white */
  if(h_==0){
    r_=g_=b_=br_;
    return;
  }
 
  int hi=(int)(floor(h_/60.0))%6;
  
  int c=br_;
  float x=(1-abs(fmod(h_/60.0,2)-1))*c;
  int m=br_-c;
  
  
  switch(hi){
    case 0:
      r_=c; g_=x; b_=0;
      break;
    case 1:
      r_=x; g_=c; b_=0;
      break;
    case 2:
      r_=0; g_=c; b_=x;
      break;
    case 3:
      r_=0; g_=x; b_=c;
      break;
    case 4:
      r_=x; g_=0; b_=c;
      break;
    case 5:
      r_=c; g_=0; b_=x;
      break;      
  }
  r_+=m;
  g_+=m;
  b_+=m;
  
  
}



void setup(){

  pinMode(PIN_R,OUTPUT);
  pinMode(PIN_G,OUTPUT);
  pinMode(PIN_B,OUTPUT);

  delay(1000);
  _cap_hue.set_CS_AutocaL_Millis(0xFFFFFFFF);
  _cap_bright.set_CS_AutocaL_Millis(0xFFFFFFFF);
  _cap_mode.set_CS_AutocaL_Millis(0xFFFFFFFF);
  _cap_prox.set_CS_AutocaL_Millis(0xFFFFFFFF);

}

void loop(){
  
  
  long thue_=_cap_hue.capacitiveSensor(SENSE_INTERVAL);
  long tbright_=_cap_bright.capacitiveSensor(SENSE_INTERVAL);
  long tmode_=_cap_mode.capacitiveSensor(SENSE_INTERVAL);
  long tprox_=_cap_prox.capacitiveSensor(PROX_SENSE_INTERVAL);
  
  if(thue_>TOUCH_THRES){    
   
    if(tprox_>=PROX_LOW_THRES && tprox_<=PROX_HIGH_THRES)
      _vhue=constrain(map(tprox_,PROX_LOW_THRES,PROX_HIGH_THRES,0,360),0,360);              
   
  }else if(tbright_>TOUCH_THRES){                
    
    if(tprox_>=PROX_LOW_THRES && tprox_<=PROX_HIGH_THRES)
      _vbright=constrain(map(tprox_,PROX_LOW_THRES,PROX_HIGH_THRES,255,0),0,255);           
   
  }else if(tmode_>TOUCH_THRES){
       
      changeMode();
   
  }

  int r,g,b;
  hueToColor(_vhue,_vbright,r,g,b);
  float t=1;
  switch(_mode){
    case 1:
      if(millis()%BLINK_VEL>BLINK_VEL/2) t=0;
      break;  
    case 2:
      t=sin((float)(millis()%BREATH_VEL)/BREATH_VEL*PI);
      break;  
  }
  
  analogWrite(PIN_R,(float)r*t);
  analogWrite(PIN_G,(float)g*t);
  analogWrite(PIN_B,(float)b*t);
  
  delay(2);

}



