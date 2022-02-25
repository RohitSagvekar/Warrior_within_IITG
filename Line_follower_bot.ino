

/*  11-04-21
  Rohit V. Sagvekar
  Electronics club - covid line follower labyrith solver bot
  Now in dark mode.
  Toh abhi we have hosted this on a github repo.
*/

/*LIMITATIONS
 * no loops
 * no dead ends(no nodes without beds)
 *
 */

/* detection of a decision junction T or X
 *
 *
 *
 */

#include <LiquidCrystal.h>

String lcdg;

char bedMap[50][50]; // Ha toh bedSize yaha declare karenge. Will change that manually for bigger maps.
                     // Here there's another assumption that there arent a lot of unnecessary turns in the path.

int length(char arr1[])
{
  for (int i = 0; i < 50; i++)
  {
    if (arr1[i] == 'A')
    {
      return i;
    }
  }
}

char presentLocation[50];

char tempAddress[50];

int targetLocations[] = {1, 4, 7, 8, 9, 42, 69}; // Input taken from NodeMCU

int numberTarget; // len(targetLocations)
int currentTurn = 0;
int currentBed = 0;
int Bn = 0;
int Tn = 0;

// Pins
int motor1pin1 = 3; // Motorpins
int motor1pin2 = 2;
int motor2pin1 = 4;
int motor2pin2 = 5;
int IRSensor1 = 6; // Haanji toh this is the 5 IRSensor array
int IRSensor2 = 7;
int IRSensor3 = 8;
int IRSensor4 = 9;
int IRSensor5 = 10;
int Dmotorpin1; // Drawer Motor Pins
int Dmotorpin2;

LiquidCrystal lcd(11, 12, 7, 6, 5, 4); // initializing the LCD object

String screen_print1 = "MEDICINES"; // The strings to be displayed on LCD screen
String screen_print2 = "temperature";

int led1 = 3;
int temppin = A1;
int temp;

int dur = 10000;

// Path status variables
int statusSensor1;
int statusSensor2;
int statusSensor3;
int statusSensor4;
int statusSensor5;
int prevSensor1;
int prevSensor2;
int prevSensor3;
int prevSensor4;
int prevSensor5;

// Time constants
int stopTime = 50; // These are the time constants
int turnTime = 50; // toh this is a new variable we have declared. We will fine tune this once the bot is built. it represets the milliseconds required for the bot to take a right turn.
int nodeTime = 50; // This is for differentiation b/w station, junction and terminus

// Flags//
// int nodeFlag = 0; //the node status flags
int stationFlag = 0;
int terminusFlag = 0;
int mapFlag = 0; // the two MODE flags. mapMode and moveMode
int moveFlag = 0;
int xFlag = 0;
int tStraightFlag = 0;
int tLeftFlag = 0;
int tRightFlag = 0;
int lLeftFlag = 0;
int lRightFlag = 0;
int shortpathFlag = 0;

void readSensor()
{
  statusSensor1 = digitalRead(IRSensor1); // This is for chechking what is under the sensor array in this moment
  statusSensor2 = digitalRead(IRSensor2);
  statusSensor3 = digitalRead(IRSensor3);
  statusSensor4 = digitalRead(IRSensor4);
  statusSensor5 = digitalRead(IRSensor5);
}

// Ha abhi we'll define functions for left right straight.
void turnLeft()
{
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  delay(turnTime);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
}

void turnRight()
{
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(turnTime);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void turnStraight()
{
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(turnTime);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}
void turnBack()
{
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(2 * turnTime);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void goAhead()
{
  if (statusSensor2 == 1)
  {
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
  }
  else if (statusSensor4 == 1)
  {
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
  }
  else
  {
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
  }
  delay(stopTime);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void goBack()
{
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  delay(stopTime);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
  turnStraight;
}

void straightCheck(int checkDummy)
{
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
  delay(checkDummy);
  readSensor();
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void backCheck(int checkDummy)
{
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
  delay(checkDummy);
  readSensor();
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void detectNode()
{ // This set of functions detect nodes at which the decision making will occur
  if (statusSensor1 == 1 or statusSensor5 == 1)
  {
    prevSensor1 = statusSensor1; // storing IR states as soon as it detects a junction
    prevSensor2 = statusSensor2;
    prevSensor3 = statusSensor3;
    prevSensor4 = statusSensor4;
    prevSensor5 = statusSensor5;
    straightCheck(3 * nodeTime);
    if (statusSensor1 == 1 && statusSensor2 == 1 && statusSensor3 == 1 && statusSensor4 == 1 && statusSensor5 == 1)
    {
      terminusFlag = 1;
    }
    backCheck(3 * nodeTime);
    straightCheck(nodeTime);
    if (statusSensor1 == 1 && statusSensor2 == 1 && statusSensor3 == 1 && statusSensor4 == 1 && statusSensor5 == 1)
    {
      stationFlag = 1;
    }
    else if (statusSensor3 == 0 and prevSensor1 == 0 and prevSensor5 == 1)
    {
      lRightFlag = 1;
    }
    else if (statusSensor3 == 0 and prevSensor1 == 1 and prevSensor5 == 0)
    {
      lLeftFlag = 1;
    }
    else if (statusSensor3 == 0 and prevSensor1 == 1 and prevSensor5 == 1)
    {
      tStraightFlag = 1;
    }
    else if (statusSensor3 == 1 and prevSensor1 == 1 and prevSensor5 == 1)
    {
      xFlag = 1;
    }
    else if (statusSensor3 == 1 and prevSensor1 == 1 and prevSensor5 != 1)
    { // can be removed
      tLeftFlag = 1;
    }
    else if (statusSensor3 == 1 and prevSensor1 != 1 and prevSensor5 == 1)
    { // can be removed
      tRightFlag = 1;
    }
    backCheck(nodeTime);
  }
}

void shortpath()
{
  if (bedMap[Bn][Tn] == 'L' && bedMap[Bn][Tn - 2] == 'L')
  {
    bedMap[Bn][Tn - 2] = 'S';
    bedMap[Bn][Tn - 1] = '_';
    bedMap[Bn][Tn] = '_';
    Tn = Tn - 2;
  }
  else if (bedMap[Bn][Tn] == 'L' && bedMap[Bn][Tn - 2] == 'R')
  {
    bedMap[Bn][Tn - 2] = 'B';
    bedMap[Bn][Tn - 1] = '_';
    bedMap[Bn][Tn] = '_';
    Tn = Tn - 2;
  }
  else if (bedMap[Bn][Tn] == 'L' && bedMap[Bn][Tn - 2] == 'S')
  {
    bedMap[Bn][Tn - 2] = 'R';
    bedMap[Bn][Tn - 1] = '_';
    bedMap[Bn][Tn] = '_';
    Tn = Tn - 2;
  }
  else if (bedMap[Bn][Tn] == 'S' && bedMap[Bn][Tn - 2] == 'L')
  {
    bedMap[Bn][Tn - 2] = 'R';
    bedMap[Bn][Tn - 1] = '_';
    bedMap[Bn][Tn] = '_';
    Tn = Tn - 2;
  }
  else if (bedMap[Bn][Tn] == 'S' && bedMap[Bn][Tn - 2] == 'S')
  {
    bedMap[Bn][Tn - 2] = 'B';
    bedMap[Bn][Tn - 1] = '_';
    bedMap[Bn][Tn] = '_';
    Tn = Tn - 2;
  }
  else if (bedMap[Bn][Tn] == 'R' && bedMap[Bn][Tn - 2] == 'L')
  {
    bedMap[Bn][Tn - 2] = 'B';
    bedMap[Bn][Tn - 1] = '_';
    bedMap[Bn][Tn] = '_';
    Tn = Tn - 2;
  }
  shortpathFlag = 0;
}

String screen_print(String screen)
{
  lcd.print(screen);
}

void Drawer_out(int dur)
{
  digitalWrite(Dmotorpin1, HIGH);
  digitalWrite(Dmotorpin2, LOW);
  delay(dur);
  digitalWrite(Dmotorpin1, LOW);
  digitalWrite(Dmotorpin2, LOW);
}
void Drawer_in(int dur)
{
  digitalWrite(Dmotorpin1, LOW);
  digitalWrite(Dmotorpin2, HIGH);
  delay(dur);
  digitalWrite(Dmotorpin1, LOW);
  digitalWrite(Dmotorpin2, LOW);
}
void ledblink(int led)
{
  digitalWrite(led, HIGH);
  delay(2000);
  digitalWrite(led, LOW);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(IRSensor1, INPUT);
  pinMode(IRSensor2, INPUT);
  pinMode(IRSensor3, INPUT);
  pinMode(IRSensor4, INPUT);
  pinMode(IRSensor5, INPUT);

  lcd.begin(16, 2);
  pinMode(Dmotorpin1, OUTPUT);
  pinMode(Dmotorpin2, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(temppin, INPUT);

  for (int i = 0; i < 50; i++)
  { // Putting all 'A's in the bedmap Matrix.
    for (int j = 0; j < 50; j++)
    {
      bedMap[i][j] = 'A';
    }
  }
}

// the loop function runs over and over again forever
void loop()
{

  // One digitalRead statement here to check for mapFlag and moveFlag;

  readSensor();

  if (mapFlag == 1)
  {            // THIS IS THE MAPPING LOOP.
               // By the end of this, bedMap should be generated
               // So the exit condition of this loop would be when detectTerminus flag is raised.
    goAhead(); // Start of LSRB
    detectNode();
    if (xFlag == 1 or tStraightFlag == 1 or tLeftFlag == 1)
    {
      turnLeft();
      bedMap[Bn][Tn] = 'L';
      Tn++;
      if (shortpathFlag = 1)
      {
        shortpath();
        shortpathFlag = 0;
      }
    }
    else if (tRightFlag == 1)
    {
      turnStraight();
      bedMap[Bn][Tn] = 'S';
      Tn++;
      if (shortpathFlag = 1)
      {
        shortpath();
        shortpathFlag = 0;
      }
    }
    else if (terminusFlag == 1)
    {
      for (int i = 0; i < 100; i++)
      {
        bedMap[Bn][i] = 0;
      }

      Tn = 0;
      mapFlag = 0;
      turnBack();
      // LCD pe put "Mapping is complete"
    }
    else if (stationFlag == 1)
    {
      turnBack();
      Bn++;
      for (int i = 0; i < 100; i++)
      {
        bedMap[Bn][i] = bedMap[Bn - 1][i];
      }
      Tn++;
      bedMap[Bn][Tn] = 'B';
      shortpathFlag = 1;
      // On LCD print "Bed N"
    }
    // end of LSRB
    // an if statement to check stationFlag and terminusFlag
  }

  if (moveFlag == 1)
  { // THIS IS THE MOVING LOOP
    // This will have spaces where we will insert lakshyaTarang code
    // Append terminus as last target location.
    goAhead();
    detectNode();

    if (currentBed == 0)
    {
      int i = 0;
      while (bedMap[currentBed][i] != 'A')
      {
        tempAddress[i] = bedMap[targetLocations[0]][i];
        i++;
      }
    }

    else if (currentBed != 0)
    { // find out last common node, then negate the entries after that.
      int i = 0;
      int k = 0;
      while (bedMap[targetLocations[currentBed - 1]][i] == tempAddress[i])
      {
        i++;
      }
      // at this point i is the last +1 common node.
      for (int j = length(bedMap[currentBed - 1]) - 1; j > i - 1; j--)
      {
        if (bedMap[currentBed - 1][Tn] == 'L')
        {
          tempAddress[k] = 'R';
        }
        if (bedMap[currentBed - 1][Tn] == 'S')
        {
          tempAddress[k] = 'B';
        }
        if (bedMap[currentBed - 1][Tn] == 'R')
        {
          tempAddress[k] = 'L';
        }
        if (bedMap[currentBed - 1][Tn] == 'B')
        {
          tempAddress[k] = 'S';
        }
        k++;
      }
      for (int j = i; i < length(bedMap[currentBed]); i++)
      {
        tempAddress[k] = bedMap[currentBed][j];
        k++;
      }
    }

    if (xFlag == 1 or tLeftFlag == 1 or tRightFlag == 1 or tStraightFlag == 1)
    { // This part makes bot take turn decisions based on tempAddress matrix
      if (tempAddress[currentTurn] == 'L')
      { // repeat for 4 instances
        turnLeft();
      }
      if (tempAddress[currentTurn] == 'S')
      {
        turnStraight();
      }
      if (tempAddress[currentTurn] == 'R')
      {
        turnRight();
      }
      if (tempAddress[currentTurn] == 'B')
      {
        turnBack();
      }
      Tn++;
    }
    if (stationFlag == 1)
    {
      // state condition
      lcd.setCursor(0, 0);
      screen_print(screen_print1);
      Drawer_out(dur);
      ledblink(led1);
      delay(500);
      lcd.clear();
      delay(500);

      lcd.setCursor(0, 1);
      screen_print(screen_print2);
      Drawer_in(dur);
      delay(200);
      int reading = analogRead(temppin);
      lcd.clear();
      lcd.print(reading);
      delay(200);
      // state change

      currentBed++;
      currentTurn = 0;
    }
  }

  // Here we will include code to deactivate the flags which have been activated.
  delay(stopTime);
  stationFlag = 0;
  terminusFlag = 0;
  xFlag = 0;
  tStraightFlag = 0;
  tLeftFlag = 0;
  tRightFlag = 0;
  lLeftFlag = 0;
  lRightFlag = 0;
}
