#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>

#define DEBUG 0

#if DEBUG == 1
  #define debug(x) Serial.print(x)
  #define debugln(x) Serial.println(x)
#else
  #define debug(x)
  #define debugln(x)
#endif

RF24 radio(9, 10); // CE, CSN
RF24Network network(radio);
RF24NetworkHeader header; //initialize variables to store data

char text[32] = "";
bool oneReceived    = true;
bool twoReceived    = true;
bool threeReceived  = true;
bool fourReceived   = true;
bool fiveReceived   = true;
bool sixReceived    = true;

const uint16_t master00 = 00;
RF24NetworkHeader header1 = 01;
RF24NetworkHeader header2 = 02;

//defining answer structure
typedef struct{
  int teamNumber;
  char teamAnswer;
  int time;
}
answerPackage;
answerPackage teamAnswer; //initializing answer for receiving

int teamID;
char r = 'R'; 

unsigned long startTime = 0;
unsigned long delayTime = 0;
int count = 0;

void setup()
{
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setRetries(15,30);
  radio.setAutoAck(true);
  SPI.begin();
  network.begin(90, master00);
  Serial.begin(9600);
}

void loop() {

  network.update();
  while (network.available())
  {
    network.read(header, &teamAnswer, sizeof(teamAnswer)); // Read the incoming data
    debugln(teamAnswer.teamAnswer);

    teamID = teamAnswer.teamNumber;
    debugln(teamID);
    count++;
    debugln(count);

    switch(teamID){
      case 1:
          checkReceived(teamAnswer,oneReceived);
          break;
      case 2:
          debugln("Team 2");
          checkReceived(teamAnswer,twoReceived);
          break;
      case 3:
          checkReceived(teamAnswer,threeReceived);
          break;
      case 4:
          checkReceived(teamAnswer,fourReceived);
          break;
      case 5:
          checkReceived(teamAnswer,fiveReceived);
          break;
      case 6:
          checkReceived(teamAnswer,sixReceived);
          break;
      default: break;    
    }
  }

    switch (Serial.read()) { 
      case 'R': {
        // Sending the "R" command to all nodes (answer units)
        startTime = millis(); // Store the start time
        resetAllReceived();
        debugln("resetting all");
        break;
      }

      //cases A-F allow for individual prompting
      case 'A': {
        resetSingleReceived(oneReceived);
        break;
      }
      case 'B': {
        resetSingleReceived(twoReceived);
        break;
      }
      case 'C': {
        resetSingleReceived(threeReceived);
        break;
      }
      case 'D': {
        resetSingleReceived(fourReceived);
        break;
      }
      case 'E': {
        resetSingleReceived(fiveReceived);
        break;
      }
      case 'F': {
        resetSingleReceived(sixReceived);
        break;
      }
      //sets team number for BackUp controller 1
      case 'G': {
          r = 'A';
          header = 02;
          break;
      }
      case 'H': {
          r = 'B';
          header = 02;
          break;
      }
      case 'I': {
          r = 'C';
          header = 02;
          break;
      }
      case 'J': {
          r = 'D';
           header = 02;
          break;
      }
      case 'K': {
          r = 'E';
          header = 02;
          break;
      }
      case 'L': {
          r = 'F';
          header = 02;
          break;
      }

      //sets team number for BackUp controller 1
      case 'M': {
          r = 'A';
          header = 03;
          break;
      }
      case 'N': {
          r = 'B';
          header = 03;
          break;
      }
      case 'O': {
          r = 'C';
          header = 03;
          break;
      }
      case 'P': {
          r = 'D';
          header = 03;
          break;
      }
      case 'Q': {
          r = 'E';
          header = 03;
          break;
      }
      case 'S': {
          r = 'F';
          header = 03;
          break;
      }
      case 'Z': {
        Serial.println("Connected");
        break;
      }

      default: break;
  }
  if (r != 'R'){
    setTeam(header);
    r = 'R';
  }

  if(startTime == 60000){ //prevent any answers from coming in after time
    lockAnswers();
  }
}

void setTeam(RF24NetworkHeader header)
{
  bool ok = network.write(header, &r,sizeof(r));
  debugln("Writing new team number");
}


void checkReceived(answerPackage teamAnswer, bool &isReceived){ //this function will prevent the CPU from printing teams twice it receieves it twice.
  if(!isReceived){
    unsigned long elapsedTime = millis() - startTime;
    teamAnswer.time = elapsedTime;
    String answerString = String(teamAnswer.teamNumber) + teamAnswer.teamAnswer + String(teamAnswer.time);
    Serial.println(answerString); //the only real serial print here
    isReceived = true;
  }
}

void resetAllReceived(){
  oneReceived = false;
  twoReceived = false;
  threeReceived = false;
  fourReceived = false;
  fiveReceived = false;
  sixReceived = false;
}

void resetSingleReceived(bool &isReceived){
  isReceived = false;
}

void lockAnswers(){
  oneReceived = true;
  twoReceived = true;
  threeReceived = true;
  fourReceived = true;
  fiveReceived = true;
  sixReceived = true;
}



