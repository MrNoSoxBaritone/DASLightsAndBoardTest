#include <DASLights-pins.h>

/*
  DASLightsBoardtest
  Archery timing lights on Arduino
 
  Diagnostic routine for checking switch functions 
 */

#include <DASLights-pins.h>

// Lamp flags - could set these to the same as the input pins
#define redlamp 4
#define yellowlamp 2
#define greenlamp 1
#define autolamp 0

// Current system state
#define stateWait 1
#define stateSequence 2
#define stateEmergencyStop 3
#define stateAttract 4

// Sequences
#define sequenceFITA_3_1 0
#define sequenceFITA_3_2 1
#define sequenceFITA_6_1 2
#define sequenceFITA_6_2 3
#define sequenceGNAS     4
#define sequenceCONTINUAL 5


// Global variables because I'm too lazy not to use them
int beeplength = 500;

int sequence = 0;
int selectedlamp = 0;
int sequenceStage = 0;
long sequenceStart = 0;
long detaillength = 0;
int currentlamp = 0;
int oldlamp = 0;
long beeptime = 0;
int beepcount = 0;
int dimmer = 0;
int state = 0;
int pressingnextdetailpin = 0;
int beepstate = 0;
int beepdisabled = 0;
int lampSelectReading = 0;
int sequenceSelectReading = 0;
int emergencySetting = 0;
int resetSetting = 0;
int nextSetting = 0;
int startSetting = 0;
int beepSetting = 0;


/*
    The setup routine runs once at startup
*/    
void setup() {                
  // Start serial I/O
  Serial.begin(9600);

  // Initialize all pins as an output.
  pinMode(redpin, OUTPUT);     
  pinMode(yellowpin, OUTPUT);     
  pinMode(greenpin, OUTPUT); 
  pinMode(beeper, OUTPUT);
  
  pinMode(emergencypin, INPUT);
  pinMode(resetpin, INPUT);
  pinMode(nextdetailpin, INPUT);
  pinMode(startpin, INPUT);
  pinMode(beepswitch, INPUT);
  
  pinMode(lampselect, INPUT);  
  pinMode(dimmerPin, INPUT);  
  pinMode(sequenceSelectPin, INPUT);  
  
  //Set wait state of the system
  Serial.println("Starting...");
 
  lightson(redlamp);
  delay(300);
  lightson(yellowlamp);
  delay(300);
  lightson(greenlamp);
  delay(300);
  
  checkSensors();
  currentlamp = redlamp;
  lightson(currentlamp);
  state = stateWait;
 

}




/* 
    The loop routine runs over and over again forever
*/
void loop() {
 
  checkSensors();                               //Check buttons and switches
//  if (state == stateSequence) doSequence();     //Service the sequence loop
  dobeeper();                                   //Check the beep loop
  lightson(currentlamp);                        //And check lights

  delay(100);                                   //Then wait and do it all again
}



/*
    Beep loop
    The service loop for the automatic beep
*/
void dobeeper() {
  if(beepSetting)
  {
        digitalWrite(beeper, LOW);     // beepswitch to OFF
  }
  else 
  {
    digitalWrite(beeper, HIGH);     // beepswitch to ON
  }
}



/*
    Check the auto/manual light setting
*/
void checklampselect() {
  lampSelectReading = analogRead(lampselect);
  if (state != stateSequence) { // don't care about lamp state if running a sequence
    if (lampSelectReading >700) {
      resetAll();
      selectedlamp = greenlamp;
      currentlamp = greenlamp;
    }
    else
    {
      if (lampSelectReading >400) {
        resetAll();
        selectedlamp = yellowlamp;
        currentlamp = yellowlamp;
      }
      else
      {
        if (lampSelectReading >100) {
          resetAll();
          selectedlamp = redlamp;
          currentlamp = redlamp;
        }
        else
        {
          selectedlamp = autolamp;
        }
      }
    }
  }
  // 4th switch setting (0) is no specific light
  // so leave currentlamp to be set by sequence, emergency or whatever
}



/*
    Check the sequence setting
*/
void checksequenceselect() {
  sequenceSelectReading = analogRead(sequenceSelectPin);
  
}



/*
    Check all buttons and act accordingly
*/
void checkSensors() {

  // Check whether auto or specific lamp
  checklampselect();
  checksequenceselect();
  // Check buttons
 
  // Emergency stop - red light and three beeps
  emergencySetting = digitalRead(emergencypin);
  
  // GO!!
  startSetting = digitalRead(startpin);
  
  //  This one must be checked for a high-low cycle so it only fires once
  nextSetting = digitalRead(nextdetailpin);
    
  beepSetting = digitalRead(beepswitch);
  // Reset
  resetSetting = digitalRead(resetpin);
 
 Serial.print("ES:") ;
 Serial.print(emergencySetting) ;
 Serial.print(" GO:") ;
 Serial.print(startSetting) ;
 Serial.print(" RE:") ;
 Serial.print(resetSetting) ;
 Serial.print(" NX:") ;
 Serial.print(nextSetting) ;
 Serial.print(" BE:") ;
 Serial.print(beepSetting) ;
 Serial.print(" LI:") ;
 Serial.print(lampSelectReading) ;
 Serial.print(" SQ:") ;
 Serial.println(sequenceSelectReading);
 
}

/*
    Set box back to wait state
*/
void resetAll() {
    currentlamp = redlamp;
    state = stateWait;
}


/*
    All lights off
*/
void lightsoff() {
  analogWrite(redpin, 0);
  analogWrite(yellowpin, 0);
  analogWrite(greenpin, 0);
}


/*
    Warning - briefly flash lights
*/
void lightswarn() {
  lightson(redlamp & yellowlamp & greenlamp);
  delay(100);
  lightson(currentlamp);
}


/*
    Set lights according to bitset
*/
void lightson(int lamp) {
  if (oldlamp != lamp) {
    oldlamp = lamp;
    switch (lamp) {
      case redlamp:
      Serial.println("RED");
    break;
      case yellowlamp:
      Serial.println("YELLOW");
    break;
      case greenlamp:
      Serial.println("GREEN");
    break;
    }
  }  
  if (lamp & redlamp) {
    digitalWrite(redpin, HIGH);
  }
  else
  {
    digitalWrite(redpin, LOW);
  }
  if (lamp & yellowlamp) {
    digitalWrite(yellowpin, HIGH);
  }
  else
  {
    digitalWrite(yellowpin, LOW);
  }
  if (lamp & greenlamp) {
    digitalWrite(greenpin, HIGH);
  }
  else
  {
    digitalWrite(greenpin, LOW);
  }    
}



