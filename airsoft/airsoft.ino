#include "IRremote.h"
#include <Wire.h>     // include Arduino Wire library
#include "rgb_lcd.h"  // include Seeed Studio LCD library
rgb_lcd lcd;



/*-----( Global Constants )-----*/
const int receiver = 11;      // Signal Pin of IR receiver to Arduino Digital Pin 11
const int buzzerPin = 2;
const int bigButton = 7;

int changeInput = 99;      //inpute number for ir remote
int cursorMap = 0;         //cursor area

int minutes = 5;   //minute timer
int seconds = 0; //second timer
int milliseconds = 0; //millisecond timer
int defuseSeconds = 0; //defused 1/10 seconds
int totalDefuseSeconds = 7; //amount of time to defuse (7 seconds)
int defuseMilliseconds = 0; //keep track of defuse seconds

boolean isPaused = true;
boolean isPlay = false;
boolean startedDefuse = false;


int stage = 0;

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'



void setup(){   /*----( SETUP: RUNS ONCE )----*/
  Serial.begin(9600);
  Serial.println("Airsoft bomb debug menu");
  Serial.println("IR Receiver Button Decode"); 
  lcd.begin(16, 2);  
  lcd.print("Starting...");
  pinMode(buzzerPin, OUTPUT);
  pinMode(bigButton, INPUT);
  delay(1000);
  irrecv.enableIRIn();           // Start the receiver
  homeInput();

}/*--(end setup )---*/

void loop(){   /*----( LOOP: RUNS CONSTANTLY )----*/
  isPlay = false;
  if(stage == 0){
    homeInput();
  }
  if(stage == 1){
      displayInput();
      cursorInput();
  }
  if(stage == 2){

      timerInput();
      clockTimer();
      defuse();
  }
  if(stage == 3){
      lcd.clear();
      lcd.print("defused");
      digitalWrite(buzzerPin, HIGH);
      delay(3000);
      digitalWrite(buzzerPin, LOW);
      delay(10000);
      stage = 0;
  }

if(stage == 4){
    lcd.clear();
    lcd.print("exploded");
    for(int i = 0; i < 10; i++){
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
    }
    delay(10000);
    stage = 0;
}
  
  if(isPlay) digitalWrite(buzzerPin, HIGH);
  else digitalWrite(buzzerPin, LOW);
  changeInput = 99;
  decodeIR();
  delay(100);
  lcd.clear();
  
}/* --(end main loop )-- */



void homeInput(){
 minutes = 5;   //minute timer
 seconds = 0; //second timer
 milliseconds = 0; //millisecond timer
 defuseSeconds = 0; //defused 1/10 seconds
 totalDefuseSeconds = 7; //amount of time to defuse (7 seconds)
 defuseMilliseconds = 0; //keep track of defuse seconds

 isPaused = true;
 isPlay = false;
 startedDefuse = false;
  lcd.print("(c) William");
  lcd.setCursor(0, 1);
  lcd.print("Ramsay");
  if(changeInput < 99){
    stage = 1;
  }
}

void timerInput(){
    lcd.setCursor(4, 0);
 if(minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if(seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print(":");
  if(milliseconds < 10) lcd.print("0");
  lcd.print(milliseconds);

  if(changeInput == 13) stage = 1;
  
}



void displayInput(){
    lcd.setCursor(4, 0);
 if(minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if(seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print(":");
  if(milliseconds < 10) lcd.print("0");
  lcd.print(milliseconds);
   
  

  
  lcd.setCursor(cursorMap + 4 , 1);
  lcd.print("^");
  
}
void cursorInput(){
  if( cursorMap >=  0 && cursorMap <= 6){
    if(changeInput == 14) cursorMap--;
    if(changeInput == 15) cursorMap++;
  }

if(changeInput < 99){
    if(cursorMap == 0 && changeInput < 6) minutes = changeInput * 10;
    if(cursorMap == 1 && changeInput < 10) minutes += changeInput;
    if(cursorMap == 3 && changeInput < 6) seconds = changeInput * 10;
    if(cursorMap == 4 && changeInput < 10) seconds += changeInput;
    if(cursorMap == 6 && changeInput < 10) milliseconds = changeInput * 10;
    if(cursorMap == 7 && changeInput < 10) milliseconds = changeInput;
}

if(changeInput == 11)stage = 2;
  
}

void clockTimer(){
  if(isPaused){}
  else{
  if(minutes > 0 && seconds <= 0 && milliseconds <= 9){
    minutes--;
    seconds = 59;
    milliseconds = 99;
  }
  if(seconds > 0 && milliseconds <= 9){
    seconds--;
    isPlay = true;
    milliseconds = 99;
  }
  if(milliseconds > 0){
    milliseconds-= 10;
  }
  }
  if(changeInput == 10){
    if(isPaused) isPaused = false;
    else isPaused = true;
  }
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print(":");
  Serial.println(milliseconds);
}


void defuse(){
  if(digitalRead(bigButton)){
    startedDefuse = true;
    defuseMilliseconds++;
  }
  
  if(defuseMilliseconds >= 10){
    defuseSeconds++;
    defuseMilliseconds = 0;
  }
  
  if(startedDefuse){
    lcd.setCursor(0, 1);
    lcd.print("{");
    for(int i = 0; i < defuseSeconds; i++){
      lcd.print("##");
    }
  }
  if(defuseSeconds >= 7){
    stage = 3;
  }
  if(minutes <= 0 && seconds <= 0 && milliseconds <= 0){
    stage = 4;  
  }
  if(digitalRead(bigButton)) Serial.println("BUTTON DETECTED");
  Serial.println(defuseMilliseconds);
  Serial.println(defuseSeconds);
  
}



void decodeIR(){
   if (irrecv.decode(&results))   // have we received an IR signal?
  {
    translateIR(); 
    Serial.print("INPUT DETECTED: ");
    Serial.println(changeInput);
    delay(100);      
    // Do not get immediate repeat
    irrecv.resume();            // receive the next value
  }  
}

void translateIR() {          // takes action based on IR code received
// describing Remote IR codes 

  switch(results.value){
    case 0xFFA25D: changeInput = 11; break;//power 
    case 0xFFE21D: changeInput = 13; break; //function
    case 0xFF629D: changeInput = 12; break; //vol+
    case 0xFF22DD: changeInput = 14; break; //fastback
    case 0xFF02FD: changeInput = 10; break; //pause
    case 0xFFC23D: changeInput = 15; break; //fastforward
    case 0xFFE01F: changeInput = 16; break; //down
    case 0xFFA857: changeInput = 17; break; //vol-
    case 0xFF906F: changeInput = 18; break; //up
    case 0xFF9867: changeInput = 19; break; //eq
    case 0xFFB04F: changeInput = 20; break; //st/repeat
    case 0xFF6897: changeInput = 0; break; //0
    case 0xFF30CF: changeInput = 1; break; //1
    case 0xFF18E7: changeInput = 2; break; //2
    case 0xFF7A85: changeInput = 3; break; //3
    case 0xFF10EF: changeInput = 4; break; //4
    case 0xFF38C7: changeInput = 5; break; //5
    case 0xFF5AA5: changeInput = 6; break; //6
    case 0xFF42BD: changeInput = 7; break; //7
    case 0xFF4AB5: changeInput = 8; break; //8
    case 0xFF52AD: changeInput = 9; break; //9
     case 0xFFFFFFFF: Serial.println(" REPEAT");break;  

 



  default: 
    Serial.print("error at:");
    Serial.println(results.value);

  }

} 
