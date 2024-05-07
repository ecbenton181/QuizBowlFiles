#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
#include <avr/io.h>
#include <avr/wdt.h>

#define DEBUG 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

const int REDLED = 3;
const int GREENLED = 2;

RF24 radio(9,10);
RF24Network network(radio);

char text[32];

int teamNumber = 0;
const uint16_t this_node = 03; //id for this node
const uint16_t master00 = 00;  //set CPU node
RF24NetworkHeader header32(master00);
RF24NetworkHeader header; //placeholder for incoming prompts

//packaging data
typedef struct
{
  int teamNumber;
  char answer;
  int time;
}
answerPackage;
answerPackage teamAnswer; //initialize answer 

void setup() {  
  teamAnswer.teamNumber = teamNumber;

  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);

  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(10,OUTPUT);

  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setRetries(30,15);
  radio.setAutoAck(true);

  SPI.begin();
  network.begin(90, this_node);
  Serial.begin(9600);

  digitalWrite(REDLED,HIGH);
}

void loop() {

  //listens for team number
  if(teamNumber == 0){
    network.update();
    while (network.available()){
      uint16_t payloadSize = network.peek(header);
      network.read(header, &text, sizeof(payloadSize)); // Read the incoming data
      debugln(text);
      setTeam(text);
    }
  }

  //handle button presses
  if(digitalRead(8) == 0){
      teamAnswer.answer = 'A';
      sendAnswer();
    }
    if(digitalRead(7) == 0){
      teamAnswer.answer = 'B';
      sendAnswer();
    }
    if(digitalRead(6) == 0){
      teamAnswer.answer = 'C';
      sendAnswer();
    }
    if(digitalRead(5) == 0){
      teamAnswer.answer = 'D';
      sendAnswer();
    }
    if(digitalRead(4) == 0){
      teamAnswer.answer = 'E';
      sendAnswer();
    }
}

void sendAnswer(){//sends answer 5 times to guarantee CPU receives answer
  teamAnswer.time = 1000;
  for(int i = 0; i < 5; i++){
    bool send = network.write(header32,&teamAnswer,sizeof(teamAnswer));
  }
}

void setTeam(char* text){ //sets replacement controller teamNumber
  switch(*text){
    case 'A':
      teamNumber = 1;
      break;
    case 'B':
      teamNumber = 2;
      break;
    case 'C':
      teamNumber = 3;
      break;
    case 'D':
      teamNumber = 4;
      break;
    case 'E':
      teamNumber = 5;
      break;
    case 'F':
      teamNumber = 6;
      break;
  }
  teamAnswer.teamNumber = teamNumber;
  blinkTeam(teamNumber);
}

void blinkTeam(int blinkCount){
  for(int i = 0; i < blinkCount; i++){
    digitalWrite(GREENLED,HIGH);
    delay(500);
    digitalWrite(GREENLED,LOW);
    delay(500);
  }
}

