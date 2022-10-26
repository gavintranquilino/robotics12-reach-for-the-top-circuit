/*
 	Reach for the Top Gameshow circuit
	By: Gavin T.
    
    Fully scalable with every hard-coded value can be changed 
    in the variable setup
*/

// ------------------- VARIABLE SETUP -------------------
#include <math.h>
// Needed for the floor function to tell which team is which

const int buzzerPin = 13;
const int resetPin = 12;

const int buzzerDelay = 200;

/* main array containing all the input sensors and outputs 
   handles both teams 
   easily add more button inputs and outputs if necessary
   made into a 2D array for easy scalability
*/
int arr[][3] = {
  
  // Team 1
  {0, 4, 0},
  {1, 5, 0},

  // Team 2
  {2, 6, 0}, 
  {3, 7, 0}, 
};

// to differentiate teams
int lenPlayersPerTeam = 2;

// could add this to arr, but easier to read as separate lists
int wrongButton[] = {10, 8, 0};
int correctButton[] = {11, 9, 0};

// lengths of rows and columns of array to iterating through 
int lenRows = (sizeof(arr)/sizeof(arr[0])); 
int lenColumns = sizeof(arr[0])/sizeof(arr[0][0]);

int lenMasterButton = sizeof(wrongButton)/sizeof(wrongButton[0]);

// ------------------- MAIN FUNCTIONS -------------------

void setup() {
  digitalWrite(resetPin, HIGH);
  
  Serial.begin(9600);
  Serial.println("[+] Ready!");
  initPlayers();
  initMaster();
  pinMode(buzzerPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
}

void loop() { 
  for (int b=0; b<lenRows; b++) {
    if (!arr[b][2]) { // if button is off
      onLed(b, returnLightLvl(b));
    }
    else {
      listenReset(); // can be reset at any time
    }
  }
}

// ------------------- USER-DEFINED FUNCTIONS -------------------

void initPlayers() {
  // Initialize all player pinouts in main array
  // Similar for loop iteration used for iterating through the main array
  for (int i=0; i<lenRows; i++ ) {
    for (int j=0; j<lenColumns-1; j++) {        
      if (j % 2 == 0) { // inputs
        pinMode(arr[i][j], INPUT);
      }
      else { // outputs
        pinMode(arr[i][j], OUTPUT);
      }
    }
  }
}

void initMaster() {
  // Separate function to set pinouts for master buttons
  for (int i=0; i<lenMasterButton-1; i++) {
    if (i % 2 != 0) { 
      pinMode(wrongButton[i], OUTPUT);
      pinMode(correctButton[i], OUTPUT);
    }
    else {
      pinMode(wrongButton[i], INPUT);
      pinMode(correctButton[i], INPUT);
    }
  }
}

void onAll() {
  // Turn on all LEDs
  // Used for testing the real circuit
  for (int i=0; i<lenRows; i++) {
    for (int j=0; j<lenColumns-1; j++) {
      if (j % 2 != 0) { 
        digitalWrite(arr[i][j], HIGH);
      }
    }
  }
}

void offAll() {
  // Turn off all LEDs
  // Used for testing the real circuit
  for (int i=0; i<lenRows; i++) {
    for (int j=0; j<lenColumns-1; j++) {
      if (j % 2 != 0) { 
        digitalWrite(arr[i][j], LOW);
      }
    }
  }
}

int returnLightLvl(int buttonNum) {  
  int lightLvl = analogRead(arr[buttonNum][0]);
  
  /* measured range of photoresistor
     NOTE: Tinkercad Photoresistor values default as ON
     map(value, fromLow, fromHigh, toLow, toHigh)
  */  
  lightLvl = map(lightLvl, 2, 404, 0, 255);
  
  lightLvl = constrain(lightLvl, 0, 255);
  
  return lightLvl;
}

void onLed(int buttonNum, int lightAmt) {
  
  if (lightAmt == 255 && arr[buttonNum][2] == 0) { // tinkercad defaults to dark
    digitalWrite(arr[buttonNum][1], HIGH);
    toggleButtonState(arr[buttonNum]);
    buzzerFlash(buzzerDelay);
    
 	// BASICALLY THE MAIN LOOP
    while (!wrongButton[2]) { //while not wrong and after a team answered
      listenWrong();
      listenCorrect();
    }
    
    if (wrongButton[2]) {
      // if wrong answer for team
      disableTeam(returnTeamNum(buttonNum));
    }
  }
  
  else {   
    digitalWrite(arr[buttonNum][1], LOW);
  }
}

void toggleButtonState(int array[]) {
  //changes the button state to its opposite value
  array[2] = !array[2];  
}

void listenWrong(){
  int buttonInput = digitalRead(wrongButton[0]);
  int buttonOutput = wrongButton[1];
  
  if (!wrongButton[2]) {
    // if wrong button pressed by master
    if (buttonInput == HIGH) { 
      digitalWrite(buttonOutput, HIGH);
      wrongButton[2] = 1;
    }
    else {
      digitalWrite(buttonOutput,LOW);
    }
  }
}

void listenCorrect() {
  int buttonInput = digitalRead(correctButton[0]);
  int buttonOutput = correctButton[1];
  
  if (buttonInput == HIGH) {
    digitalWrite(buttonOutput, HIGH);
    
    while (1) {
      Serial.println("[+] Correct Answer");
      digitalWrite(buzzerPin, HIGH);
      delay(1500);
      resetAll(); // Reset circuit 
      break;
    }
  }
  
  else {
    digitalWrite(buttonOutput, LOW);
  }
}

void listenReset() {
  int buttonInput = digitalRead(correctButton[0]);
  int buttonOutput = correctButton[1];
  
  if (buttonInput == HIGH) {
    digitalWrite(buttonOutput, HIGH);
    
    while (1) {
      Serial.println("[+] Resetting");
      digitalWrite(buzzerPin, HIGH);
      delay(1500);
      resetAll();
      break;
    }
  }
  
  else {
    digitalWrite(buttonOutput, LOW);
  }
}

void resetAll() {
  // Reset through the reset pins
  Serial.println("[-] Resetting...");
  delay(1000);
  digitalWrite(resetPin, LOW);
}

void disableTeam(int teamNum) { 
  // Using the button states, disable teams
  int firstBtn = teamNum*lenPlayersPerTeam;
  int lastBtn = firstBtn+lenPlayersPerTeam;

  for (int btn=firstBtn; btn<lastBtn; btn++) {
    if (!arr[btn][2]) {
      toggleButtonState(arr[btn]);
      digitalWrite(arr[btn][1], HIGH);
    }
  }
  
  buzzerFlash(buzzerDelay);
  wrongButton[2] = 0;
  digitalWrite(wrongButton[1], LOW);
} 

void buzzerFlash(int delayTime) {
  // Turn on buzzer for buzzerDelay amt of time
  digitalWrite(buzzerPin, HIGH);
  delay(delayTime);
  digitalWrite(buzzerPin, LOW);
}

int returnTeamNum(int buttonNum) {
  // Some math to return the team number in the array
  if (buttonNum == 0) {
    buttonNum = 1; // remove 0 ceil case
  }
  float teamVal = buttonNum/lenPlayersPerTeam;
  return floor(teamVal);
}
