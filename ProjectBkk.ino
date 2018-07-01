
#include <MsTimer2.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "SPI.h"  
#include "RTClib.h"
RTC_DS3231 RTC;
#define I2C_ADDR 0x3F 
#define BACKLIGHT_PIN 3
LiquidCrystal_I2C lcd(I2C_ADDR,2,1,0,4,5,6,7);
#define DIR1_PIN 9
#define STEP1_PIN 8
#define Ms_F 10
#define Lock 12
#define Focus A0
#define Take A1



//// Rotary Encoder ///
int encoder0PinA = 6;
int encoder0PinB = 5;
int num_rotary;
int encoder0PinALast = LOW;
int n = LOW;
//////// End Rotary Encoder ////

int sw1 = 2;
int sw2 = 3;
int sw3 = 4;
int state,btn1,btn2,btn3,select_cursor;
int num_loop;

int cnt_rotate;

String strDate;

int Auto_status,Manual_status,EditTime_status;
int HH,MM,S;
int Day,Month,Year;
int TimeAuto_Hour,TimeAuto_Minute;
int num_running;



int sleep = 1;


void setup() {
   
   Serial.begin(9600);
   pinMode(DIR1_PIN, OUTPUT);
   pinMode(STEP1_PIN, OUTPUT);
   pinMode(Ms_F,OUTPUT);
   pinMode(Lock,OUTPUT);
   pinMode(Focus,OUTPUT);
   pinMode(Take,OUTPUT);
   Rotary_init();
   lcd.begin (16,2); 
   RTC.begin();
   //RTC.adjust(DateTime(2018, 06, 10, 15, 53, 00));   //ตั้งเวลาและวันที่ ให้เอา Comment ออก
   if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
   }
   lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
   lcd.setBacklight(HIGH);
   lcd.home (); 
   
   
   Button_init(); 
   
   state = 1;
   HH=0;
   MM=0;
   S=0;
   Day = 15;
   Month = 6;
   Year = 2018;
   num_loop = 0; 
   TimeAuto_Hour = 0;
   TimeAuto_Minute = 10;

   EditTime_status = 0;
   Auto_status = 0;
   Manual_status =0;

   cnt_rotate = 1;
   num_running = 5;
   digitalWrite(Ms_F,LOW);
   digitalWrite(Lock,HIGH);
   digitalWrite(Focus,HIGH);
   digitalWrite(Take,HIGH);

   
   lcd.setCursor(0,0);
   lcd.print("Hello Auduino");
   delay(2000);
   
}
/////////////////////////////////Main Function//////////////////////////////////////

void loop() {
  
   Button_read_1();
   if(num_loop == 0){
      PrintTime_lcd();
   }
   else  if(num_loop == 1){
        state = 1;
        while(1){
          Button_read_1();
          Loop1();
          /////// LOOP 2 //////////////////////
          if(num_loop == 2){
            lcd.clear();
            ///////////////// Auto  Mode /////////////////////
            if(state == 1){
               Auto_status = 1;
               lcd.setCursor(0,0);
               lcd.print("Select Time...");
               lcd.setCursor(2,1);
               lcd.print("5");
               lcd.setCursor(6,1);
               lcd.print("Minute");
               while(1){
                  Button_read_2();
                  Loop2_AutoMode();
                  if(num_loop == 3){
                      lcd.clear();
                      digitalWrite(Lock,LOW);
                      delay(1000);
                      while(1){
                          Button_read_2();
                          lcd.setCursor(0,0);
                          lcd.print("Running...");
                          
                          On_Step();        /////////////////////   Hardware Start ///////////////////////////
                          num_loop--;
                          if(num_loop == 2){
                               digitalWrite(Lock,HIGH);
                               lcd.setCursor(0,0);
                               lcd.print("Select Time...");
                               lcd.setCursor(1,1);
                               lcd.print(num_running);
                               lcd.setCursor(6,1);
                               lcd.print("Minute");
                               break;
                          }
                      } 
                      
                  }
                  if(num_loop == 1){
                      Auto_status = 0;
                      state = 1;
                      break;
                  }
               }
               
            }
            //////////////// End Auto Mode ///////////////////
            /////// Edite Date Time ////////////////////
            if(state == 2){
              state = 1;
              EditTime_status = 1;
              lcd.setCursor(0,0);
              if( HH < 10 )
                  lcd.print("0");
              lcd.print(HH);
              lcd.print(":");
              lcd.print("0");
              lcd.print(MM);
              
              lcd.setCursor(0,1);
              lcd.print(Day);
              lcd.print("/");
              lcd.print(Month);
              lcd.print("/");
              lcd.print(Year);
              while(1){
                  Button_read_2();
                  Loop2_EditTime();
                  if(num_loop == 3 ){
                      lcd.clear();
                      RTC.adjust(DateTime(Year, Month, Day, HH, MM, 00));///  Function Set RTC
                      lcd.setCursor(2,0);
                      lcd.print("Successfully");
                      delay(2000);
                      num_loop = 1;
                  }
                  if(num_loop == 1){
                      EditTime_status = 0;
                      state = 2;
                      break;
                  }
               }
             }
             /////////End Edit Date Time ///////////////////
             
          }
          ///////////End Loop 2 ////////////
        if(num_loop == 0)
            break;
     }
   }
   else{
          Serial.println("Error");
       }
  
}
//////////////////////////////End  Main Function /////////////////////////////////////////

void Loop1(){
  
          if(state == 1){
                 lcd.setCursor(0,0);
                 lcd.print("->Program       ");
                 lcd.setCursor(0,1);
                 lcd.print("  Edit Real Time      "); 
           }
           else if(state == 2){
                 lcd.setCursor(0,0);
                 lcd.print("  Program       ");
                 lcd.setCursor(0,1);
                 lcd.print("->Edit Real Time       ");
           }
          
          
}
void Loop2_EditTime(){
      
         if(state == 1 ){
               Rotary_set(00,23,HH);
         }
         else if(state == 2 ){
               Rotary_set(00,59,MM);
         }
         
         else if(state == 3)
               Rotary_set(1,31,Day);
         else if(state == 4)
               Rotary_set(1,12,Month);
         else if(state == 5)
               Rotary_set(2018,2030,Year);
}
void Loop2_AutoMode(){
         if(state == 1 ){
               Rotary_set(5,600,num_running);
         }
        
         
  
}
void Loop2_ManualMode(){
  
}


void Button_read_1(){
 
  btn1 = digitalRead(sw1);
  btn2 = digitalRead(sw2);
  btn3 = digitalRead(sw3);
  if(btn1 == 0){
          if(state == 1){
             state = 2;
          }
          else if(state == 2){
              state = 1;
          }
         
          delay(200);
  }
  if(btn2 == 0){
       if(num_loop != 0)
            num_loop--;
       delay(200);
  }
  if(btn3 == 0){
             num_loop++;
       delay(200);
   }
  //return state_btn;
}
void Button_read_2(){
     btn1 = digitalRead(sw1);
     btn2 = digitalRead(sw2);
     btn3 = digitalRead(sw3);
     if(btn1 == 0){
       if(EditTime_status == 1){    ////// Edit Time  Mode////////          
          if(state == 1){
             state++;
          }
          else if(state > 1 && state < 5 ){
              state++;
          }
          else if(state == 5){
              state = 1;
          }
          delay(200);
       }
       else if(Auto_status == 1){   ////////   Auto Mode  //////////////
           state = 1;
       }
       else if(Manual_status == 1){        //////////// Manual Mode /////////////////////

       }
     }
      if(btn2 == 0){
       if(num_loop != 0)
            num_loop--;
       delay(200);
      }
      if(btn3 == 0){
                 num_loop++;
           delay(200);
      }
}
void Button_read_2_AutoMode(){
     btn1 = digitalRead(sw1);
     btn2 = digitalRead(sw2);
     btn3 = digitalRead(sw3);
     if(btn1 == 0){          
          if(state == 1){
             state++;
          }
          else if(state == 2){
              state = 1;
          }
          delay(200);
     }
      if(btn2 == 0){
       if(num_loop != 0)
            num_loop--;
       delay(200);
      }
      if(btn3 == 0){
                 num_loop++;
           delay(200);
      }
}



void Button_init(){
  Serial.begin(9600);
  pinMode(sw1,INPUT_PULLUP);
  pinMode(sw2,INPUT_PULLUP);
  pinMode(sw3,INPUT_PULLUP);
  state = 0;
  btn1 = 0;
  btn2 = 0;
  btn3 = 0;
}




void Rotary_init(){
   pinMode (encoder0PinA, INPUT);
   pinMode (encoder0PinB, INPUT);
   num_rotary = 0; 
}
int Rotary_set(int Min,int Max,int value){
  num_rotary = value;
  n = digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoder0PinB) == LOW) {
          if(num_rotary == Max){
              num_rotary = Max;
          }
          else
              num_rotary++;
      } 
      else {
          if(num_rotary == Min){
               num_rotary = Min;
          }
          else
              num_rotary--;
      }
    Serial.print (num_rotary);
    Serial.println();
    if(EditTime_status){
          if(state == 1){
             HH = num_rotary;
          }
          else if(state == 2){
             MM = num_rotary;
          }
          else if(state == 3)
             Day = num_rotary;
          else if(state == 4)
             Month = num_rotary;
          else if(state == 5)
             Year = num_rotary;
      
             
          if(state ==1 || state == 2){
              lcd.setCursor(0,0);
              if(HH < 10)
                 lcd.print("0");
              lcd.print(HH);
              lcd.print(":");
              if(MM<10)
                 lcd.print("0");
              lcd.print(MM);
              lcd.print("   ");
             
              
          }
          else if(state >2 && state < 6){
              lcd.setCursor(0,1);
              if(Day < 10)
                  lcd.print("0");
              lcd.print(Day);
              lcd.print("/");
              if(Month<10)
                  lcd.print("0");
              lcd.print(Month);
              lcd.print("/");
              lcd.print(Year);
              lcd.print("   ");
              
          }
    }
    else if(Auto_status){
         num_running = num_rotary;
         lcd.setCursor(0,1);
         if(num_running<10)
             lcd.print("  ");
         else if(num_running < 100 && num_running > 9)
             lcd.print(" ");
         lcd.print(num_running);
    }
    
    
  }
  encoder0PinALast = n;
 
}




void PrintTime_lcd(){
  DateTime now = RTC.now();
  lcd.setCursor(0,0);
  lcd.print("Date:");
  if(now.day()<10){
         //num++;
          lcd.print(0, DEC);
          lcd.print(now.day(), DEC);
  }
  else{
         //num++;
          lcd.print(now.day(), DEC);
  }
  lcd.print("/");
  if(now.month()<10){
         //num++;
          lcd.print(0, DEC);
          lcd.print(now.month(), DEC);
  }
  else{
         //num++;
          lcd.print(now.month(), DEC);
  }
  lcd.print("/");
  if(now.year()<10){
         //num++;
          lcd.print(0, DEC);
          lcd.print(now.year(), DEC);
           lcd.print(" ");
  }
  else{
         //num++;
          lcd.print(now.year(), DEC);
           lcd.print(" ");
  }
 
  lcd.setCursor(0,1);
  lcd.print("Time:");
  
  if(now.hour()<10){
         //num++;
          lcd.print(0, DEC);
          lcd.print(now.hour(), DEC);
  }
  else{
         //num++;
          lcd.print(now.hour(), DEC);
  }
  lcd.print(':');
  if(now.minute()<10){
         //num++;
          lcd.print(0, DEC);
          lcd.print(now.minute(), DEC);
  }
  else{
         //num++;
          lcd.print(now.minute(), DEC);
  }
  lcd.print(':');
  if(now.second()<10){
         //num++;
          lcd.print(0, DEC);
          lcd.print(now.second(), DEC);
          lcd.print("   ");
  }
  else{
         //num++;
          lcd.print(now.second(), DEC);
           lcd.print("   ");
  }
}

void On_Step(){
//       
//     
   for(int j=0;j<4;j++){
          
   
       for(int i=0;i<6;i++){
             
           delay(2000);
           digitalWrite(Focus,LOW);
           delay(3000);
           digitalWrite(Take,LOW);
           delay(3000);
           digitalWrite(Focus,HIGH);
           digitalWrite(Take,HIGH);
           delay(2000);
           Step_Control('F',i);
       }
     delay(2000);
     digitalWrite(Ms_F,LOW);
    
       
//      
//       

           
           
       
       
       digitalWrite(Lock,HIGH);
       delay(300000);
   }
}

void Step_Control(char ch,int count){
    int numstep = (count%3 == 0) ? 1066 : 1067;
    if(cnt_rotate %4 == 0){
        numstep--;
        cnt_rotate = 1;
    }
   
    if(ch == 'f'){
       digitalWrite(Ms_F,HIGH);
       //digitalWrite(LED,HIGH);
       digitalWrite(DIR1_PIN,LOW);
       
       for(int i=0;i<numstep;i++){
          
          //if(digitalRead(SW)==1){
           digitalWrite(STEP1_PIN,HIGH);
           delay(sleep);
           digitalWrite(STEP1_PIN,LOW);
           delay(sleep);
           
           
           
         // }
       }
       //delay(200);
       
    }
   if(ch == 'F'){
       digitalWrite(Ms_F,HIGH);
       //digitalWrite(LED,LOW);
       digitalWrite(DIR1_PIN,HIGH);
       
       for(int i=0;i<numstep;i++){
          //if(digitalRead(SW)==1){
           digitalWrite(STEP1_PIN,HIGH);
           delay(sleep);
           digitalWrite(STEP1_PIN,LOW);
           delay(sleep);
           
           
           
         // }
       }
       //delay(200);
    }
}









