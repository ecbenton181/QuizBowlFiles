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

char r[32] = "R";
char s[32] = "S"; //for controller 5
char t[32] = "T"; //for controller 6
char text[32];
char ack[32];
char testControllerVar[32];

int teamNumber = 6;
const uint16_t this_node = 01; //id for this node
const uint16_t master00 = 00;  //set CPU node
RF24NetworkHeader header32(master00);
RF24NetworkHeader header; //placeholder for incoming prompts

bool answerMode = false;
bool wasSent = false;
unsigned long startTime = 0;
unsigned long answerTime = 0;
unsigned long delayTime = 0;
//char answer[32];     //stores 
char answerText[32]; //stores Answer
int loopCount = 0;
int maxRetries = 30;


//packaging data
typedef struct
{
  int teamNumber;
  char answer;
  int time;
}
answerPackage;

answerPackage teamAnswer;


void setup() {
  // put your setup code here, to run once:
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
  // put your main code here, to run repeatedly:
  //Serial.println("hey i'm working");

  // network.update();
  // while (network.available())
  // {
  //   uint16_t payloadSize = network.peek(header);
  //   network.read(header, &text, sizeof(payloadSize)); // Read the incoming data
  //   debugln(text);

  //   if(strcmp(text,r)==0){
  //     startTime = millis();
  //     answerMode = true;
  //     digitalWrite(GREENLED,HIGH);
  //     break;
  //   }
  //   // if(strcmp(text,s)==0){
  //   //   startTime = millis();
  //   //   answerMode = true;
  //   //   digitalWrite(GREENLED,HIGH);
  //   // }
  //   if(strcmp(text,t)==0){
  //     startTime = millis();
  //     answerMode = true;
  //     digitalWrite(GREENLED,HIGH);
  //   }
  // }
  // while(answerMode){
  //   if(digitalRead(8) == 0){
  //     teamAnswer.answer = 'A';
  //     sendAnswer();
  //   }
  //   if(digitalRead(7) == 0){
  //     teamAnswer.answer = 'B';
  //     sendAnswer();
  //   }
  //   if(digitalRead(6) == 0){
  //     teamAnswer.answer = 'C';
  //     sendAnswer();
  //   }
  //   if(digitalRead(5) == 0){
  //     teamAnswer.answer = 'D';
  //     sendAnswer();
  //   }
  //   if(digitalRead(4) == 0){
  //     teamAnswer.answer = 'E';
  //     sendAnswer();
  //   }
  // }

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

void sendAnswer(){
  answerTime = millis();
  unsigned long loopTime =  millis();
  answerMode = false; //prevent duplicate answers
  
  teamAnswer.time = 1000;
  for(int i = 0; i < 5; i++){
    wasSent = network.write(header32,&teamAnswer,sizeof(teamAnswer));
  }
}

void errorAlert(){
  for(int i = 0; i < 5; i++){
    digitalWrite(REDLED, LOW);
    delay(200);
    digitalWrite(REDLED, HIGH);
    delay(200);
  }
}

void errorLoop(){
  digitalWrite(GREENLED, LOW);
  delay(200);
  digitalWrite(GREENLED, HIGH);
  delay(200);
}

void resendingAnswer(){

  wasSent = false;
  while(!wasSent && loopCount < maxRetries){
    wasSent = network.write(header32, &teamAnswer, sizeof(teamAnswer));
    if(!wasSent){
      errorLoop();
      loopCount++;
      delay(100);
    }
  }
  if(!wasSent){
    errorAlert();
  }else{
    digitalWrite(GREENLED,LOW);
  }
}

