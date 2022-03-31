#include <LiquidCrystal.h>

LiquidCrystal lcd(7,6,5,4,3,2);

//button pin, state variables 
int button1Pin = 12;
long button1Timer = 0;
long button1LongPressTime = 3000;
boolean button1Active = false;
boolean button1LongPressActive = false;

int button2Pin = 11;
long button2Timer = 0;
long button2LongPressTime = 3000;
boolean button2Active = false;
boolean button2LongPressActive = false;

int button3Pin = 10;
long button3Timer = 0;
boolean button3Active = false;

int button4Pin = 9;
long button4Timer = 0;
boolean button4Active = false;

//buzzer and sensor pin, sensor values
int buzzerPin = 13;

int tempPin = A0;
float tempRead,tempVal;

//default modes
bool isCelcius = true;
bool is24HourFormat = true;
bool isAlarmOff = true;

bool isInClockSetup = false;
int clockSetupMode = 0; //0 = min, 1 = hour

bool isInAlarmSetup = false;
int alarmSetupMode = 0; //0 = min, 1 = hour

//default clock and alarm
int second = 0,minute = 0,hour = 0;
int alarmMinute = 0, alarmHour = 0;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  
  pinMode(button1Pin,INPUT);
  pinMode(button2Pin,INPUT);
  pinMode(button3Pin,INPUT);
  pinMode(button4Pin,INPUT);

  //setting up timer1 for 1 second interrupt
  noInterrupts();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 15624;
  TCCR1B |= (1<<WGM12);
  TCCR1B |= (1<<CS12) | (1<<CS10);
  TIMSK1 |= (1<<OCIE1A);

  interrupts();
}

void loop(){
  handleButton1();
  handleButton2();
  handleButton3();
  handleButton4();
  delay(100); //only used to stabilize Tinkercad system, otherwise simulator time cannot keep up
}

//Timer ISR generated every second
ISR(TIMER1_COMPA_vect){
  if(isInClockSetup == false && isInAlarmSetup == false){
    updateClock();
    calculateTemperature();
    alarmTime();
    alarmIndicator();
  }
}

//Alarm setup functions
void setupAlarm(){
  if(alarmSetupMode == 0){
    setupAlarmMinute(); 
  }
  else if(alarmSetupMode == 1){
    setupAlarmHour(); 
  }
}

void setupAlarmMinute(){
  alarmMinute++;
    if(alarmMinute == 60){
      alarmMinute = 0;
    }
  lcd.setCursor(14,1); 
    if(alarmMinute < 10){ 
   	  lcd.print(0); 
    } 
  lcd.print(alarmMinute); 
}

void setupAlarmHour(){
   alarmHour++;
    if(alarmHour == 24){
      hour = 0;
    }
  lcd.setCursor(11,1); 
    if(alarmHour < 10){ 
   	  lcd.print(0); 
    } 
  lcd.print(alarmHour); 
}

//Clock setup functions
void setupClock(){
  if(clockSetupMode == 0){
    setupClockMinute(); 
  }
  else if(clockSetupMode == 1){
    setupClockHour(); 
  }
}

void setupClockMinute(){
  minute++;
    if(minute == 60){
      minute = 0;
    }
  lcd.setCursor(3,0); 
    if(minute < 10){ 
   	  lcd.print(0); 
    } 
  lcd.print(minute); 
}

void setupClockHour(){
   hour++;
    if(hour == 24){
      hour = 0;
    }
  lcd.setCursor(0,0); 
    if(hour < 10){ 
   	  lcd.print(0); 
    } 
  lcd.print(hour); 
}

void updateClock(){
  second++;
  if(second == 60){
    second = 0;
    minute++;
    if(minute == 60){
      minute = 0;
      hour++;
      if(hour == 24){
        hour = 0;
      }
    }
  }
  printClock();
}

void printClock(){
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,0); 
  if(hour < 10){ 
    lcd.print(0); 
  } 
  int formattedHour = hour;
  if(!is24HourFormat){
    if(hour>12){
      formattedHour = hour-12;
    }
  }

  lcd.print(formattedHour); 
  lcd.setCursor(2,0); 
  lcd.print(":"); 
  lcd.setCursor(3,0); 
  if(minute < 10){ 
    lcd.print(0); 
  } 
  lcd.print(minute); 
  if(!is24HourFormat){
    bool isAM = hour<12 ? true : false;
    lcd.setCursor(6,0);
    lcd.print(isAM ? "AM" : "PM");
  }
}

//Temperature functions
/*Getting analog read to voltage, converting from 10 mv 
  per degree with 500 mV offset, then converting to 
  Fahrenheit if desired.
*/
void calculateTemperature(){
  tempRead = analogRead(tempPin);
  tempVal = (tempRead*5/1023);
  tempVal = (tempVal-0.5)*100;
  if(!isCelcius){
    tempVal = (tempVal*9/5)+32;
  }
  updateTemperature();
}

void updateTemperature(){
  lcd.setCursor(0,1);
  lcd.print(tempVal);
  lcd.setCursor(5,1);
  if(isCelcius){
    lcd.print("C");
  } else{
    lcd.print("F");
  }
}

//Alarm functions
void alarmIndicator(){
  lcd.setCursor(13,0);
  if(isAlarmOff){
    lcd.print("OFF");
  } else{
    lcd.print("ON"); 
  }
}

void alarmTime(){
  lcd.setCursor(11,1);
  if(alarmHour < 10){ 
    lcd.print(0); 
  } 
  lcd.print(alarmHour); 
  lcd.setCursor(13,1); 
  lcd.print(":"); 
  lcd.setCursor(14,1); 
  if(alarmMinute < 10){ 
    lcd.print(0); 
  } 
  lcd.print(alarmMinute); 
  if(alarmHour == hour && alarmMinute == minute && isAlarmOff == false){
    tone(buzzerPin,3000,500);
    delay(700);
  }
}

void alarmSnooze(){
  if(isAlarmOff == false){;
  	if(alarmMinute < 55){
    	alarmMinute = alarmMinute + 5;
  	}
  	else if(alarmMinute >= 55){
    	alarmHour = hour +1;
    	alarmMinute =  abs(alarmMinute-60);
  	}
  }
}

//Button handles for short, long presses
void handleButton1(){
  if (digitalRead(button1Pin) == HIGH) {
		if (button1Active == false) {
			button1Active = true;
			button1Timer = millis();
		}
		if ((millis() - button1Timer > button1LongPressTime) && (button1LongPressActive == false)) {
			button1LongPressActive = true;
			//but1 long press
      isInClockSetup = !isInClockSetup;
      lcd.setCursor(6,0);
      lcd.print("C.Set");
		}
  } else {
		if (button1Active == true) {
			if (button1LongPressActive == true) {
				button1LongPressActive = false;
			} else {
        if(isInClockSetup){
          if(clockSetupMode == 0){
            clockSetupMode = 1;
          }
          else if(clockSetupMode == 1){
            clockSetupMode = 0;
            isInClockSetup = false;
            second = 0; //When clock is set, second return to zero
          }
        } else{
            is24HourFormat = !is24HourFormat;
        }
			}
			button1Active = false;
		}
	}
}

void handleButton2(){
  if (digitalRead(button2Pin) == HIGH) {
		if (button2Active == false) {
			button2Active = true;
			button2Timer = millis();
		}
		if ((millis() - button2Timer > button2LongPressTime) && (button2LongPressActive == false)) {
			button2LongPressActive = true;
			//long but2 press
      isInAlarmSetup = !isInAlarmSetup;
      lcd.setCursor(6,0);
      lcd.print("A.Set");
		}
  } else {
		if (button2Active == true) {
			if (button2LongPressActive == true) {
				button2LongPressActive = false;
			} else {
        if(isInAlarmSetup){
          if(alarmSetupMode == 0){
            alarmSetupMode = 1;
          }
          else if(alarmSetupMode == 1){
            alarmSetupMode = 0;
            isInAlarmSetup = false;
            second = 0; //When alarm is set, second return to zero
          }
        } else{
            isAlarmOff = !isAlarmOff;
        }
			}
			button2Active = false;
		}
	}
}

void handleButton3(){
   	if (digitalRead(button3Pin) == HIGH) {
		if (button3Active == false) {
			button3Active = true;
		}
	} else{
		if (button3Active == true) {
      if(isInClockSetup == false && isInAlarmSetup ==false){
        isCelcius = !isCelcius;
        calculateTemperature();
      } 
      else if(isInClockSetup == true && isInAlarmSetup == false){
        setupClock();
      }
      else if(isInAlarmSetup == true && isInClockSetup == false){
        setupAlarm();
      }
      button3Active = false;
		}
	}
}


void handleButton4(){
  if (digitalRead(button4Pin) == HIGH) {
		if (button4Active == false) {
			button4Active = true;
		}
	} else{
		if (button4Active == true) {
			alarmSnooze();
			button4Active = false;
		}
	}
}