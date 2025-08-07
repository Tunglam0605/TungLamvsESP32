#include <PS4Controller.h>

int   led1 = 2;

void notify()
{
  if (PS4.Left() == 1|| PS4.Down() == 1 || PS4.Right() == 1 || PS4.Up() == 1 || PS4.Square() == 1 ||  PS4.Cross() == 1 ||  PS4.Circle() == 1 ||  PS4.Triangle() == 1){

    digitalWrite(led1, 1);

  }
  else {
    digitalWrite(led1, 0);
    
  }
}



void onConnect()
{
  Serial.println("Connected!.");
}


void onDisConnect()
{
  Serial.println("Disconnected!.");    
}

void setUpPinModes()
{
  pinMode(led1, OUTPUT);
}

void setup() 
{
  setUpPinModes();
  Serial.begin(115200);
  PS4.attach(notify);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisConnect);
  PS4.begin();
  Serial.println("Ready.");
}

void loop() 
{

}
