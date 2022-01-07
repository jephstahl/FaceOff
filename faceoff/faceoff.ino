//#include <blinklib.h>
//#include "face_value.h"
//#include "Serial.h"

/* Face Off (4-?) players

Goal:
Earn 6? points to win the game.

Gameplay:
Each player hold one blink. Double click to start
During the game you can single click your blink to see what colour you need to match.
The colour is displayed only for a moment to hide it from other players.
Each colour corresponds to an emotion you must portray on your face.

  Red = Love (you are in love and your face shows it)
  Blue = Contentment (everything is good in your live)
  Green = Greed (you see something your really really want and you are so close to getting)
  Yellow = Surprise (you get that think that you really really want as a surprise!)

Players portray their emotion on their face and move around the group looking for a match.
If both players agree with a nod…(or kissy face if players are comfortable with that )
… they connect the white lit edge of their blinks.
If they are correct the Blinks will flash GREEN and both players will earn a point.
If they are incorrect the Blinks will flash RED and both players will not earn a point (or maybe lose a point if they have any).
If you can not find a match, you can single click your blink to pick a new colour. 
Once you earn 6(?) points your Blink will celebrate (Flash green) and you yell FACE OFF! and you win if you are the first!
Other player can double click thier blink to find thier score.
All players can then double click to start a new game.
BLINK:
one edge of the blink will always shine white… this is your connection side.
SINGLE clicking the blink will temporarily light the 3 sides opposite the white side and show a new random colour.
DOUBLE clicking the blink will reset your score to 0 and put the blink in READY mode
READY mode: the blink will flash white showing all players are ready to start the game. 
A SINGLE click will start each blick into the game (see above)
TRIPLE click will show you your score after someone has declared FACE OFF and ended the game.

The concept is based off of my DUDE card game and variations of it.
*/

////// VARIABLES //////
Color gameColors[] = {WHITE, RED, BLUE, GREEN, YELLOW};
int maxScore = 6;
int yourColourIndex = 5;
int otherColourIndex = 5;
int score = 0;

////// GAME STATES //////
enum gameStates {SETUP, GAME, SHOW, END, CHECK, MATCH, MISS, WIN};
byte gameState = SETUP;

////// TIMERS //////
Timer nextFlash;
Timer showTimer;
bool onFlag = false;

////// SETUP //////
void setup() {
  randomize();
}
////// LOOP //////
void loop() {
  switch (gameState) {
  case SETUP: // make sure blinks are in playable config
    setupLoop();
    break; 
  case GAME:
    gameLoop();
    break;
  case SHOW:
    showLoop();
    break;
  case CHECK:
    checkLoop();
    break;
  case END:
    endLoop();
    break;
  case MATCH:
    matchLoop();
    //setColor( YELLOW );
    break;
  case MISS:
    missLoop();
    //setColor( RED );
    break;
  case WIN:
    winLoop();
    break;
  }
  //clear button presses
  buttonSingleClicked();

  //send data
  byte sendData = yourColourIndex;
  setValueSentOnFace(sendData, 0);
}

////// SETUP LOOP //////
void setupLoop(){
  pickColour();
  if (nextFlash.isExpired()) {
    setColor( WHITE );      
    nextFlash.set(300);   // Flash again in 300 millseconds  
  } else {
    setColor( OFF ); 
  }
  if (buttonDoubleClicked()){
    gameState = GAME;
  }
}

////// GAME LOOP //////
void gameLoop(){
  setColorOnFace(WHITE, 0);

  // WANT A NEW COLOUR
  if ( buttonSingleClicked() ){
    gameState = SHOW;
    showTimer.set(1200);
    pickColour();
  }

  // WANT TO CHECK SCORE AT GAME END
  if ( buttonDoubleClicked() ){
    gameState = END;
  }

  // WANT TO COMPARE
  if ( !isValueReceivedOnFaceExpired( 0 ) ) {
    gameState = CHECK; 
  }
}

////// SHOW LOOP //////
void showLoop(){
  if (showTimer.isExpired()) {
    setColor( OFF );      
    gameState = GAME;    
  } else {
    setColorOnFace( gameColors[yourColourIndex], 2 );
    setColorOnFace( gameColors[yourColourIndex], 3 );
    setColorOnFace( gameColors[yourColourIndex], 4 );          
  }
}

////// CHECK LOOP //////
void checkLoop(){
  otherColourIndex = getLastValueReceivedOnFace(0);
  if (otherColourIndex == yourColourIndex) {
    gameState = MATCH;
    score++;
    //yourColourIndex = 5
  } else {
    gameState = MISS;
    //minus(); //????
  }
}

////// MATCH LOOP //////
void matchLoop(){
  if (nextFlash.isExpired()) {
    setColorOnFace( GREEN, 0 );      
    nextFlash.set(100);   // Flash again in 300 millseconds  
  } else {
    setColor( OFF ); 
  }
  if (buttonSingleClicked()){  // single click to get a new colour and keep playing
    pickColour(); 
    gameState = GAME;
  }
  if (score >= maxScore) {    // Check to see if game is WON
    gameState = WIN;
  }
}

////// MISS LOOP //////
void missLoop(){
  if (nextFlash.isExpired()) {
    setColorOnFace( RED, 0 );      
    nextFlash.set(100);   // Flash again in 300 millseconds  
  } else {
    setColor( OFF ); 
  }
  if (buttonSingleClicked()){  // single click to get a new colour and keep playing
    pickColour(); 
    gameState = GAME;
  }
}

////// END LOOP //////
void endLoop(){

  if (nextFlash.isExpired()) {
    if (onFlag) {
      setColor( OFF );
      onFlag = false;
    } else {
      for (int x=0; x++; x<=score){
        setColorOnFace( MAGENTA, x);
      }
      onFlag = true; 
    }
    nextFlash.set(500);   
  }
  
  if (buttonDoubleClicked()){  // single click to get a new colour and keep playing
    pickColour(); 
    score = 0;
    gameState = GAME;
  }
  
}

////// WIN LOOP ///////
void winLoop(){

  if (nextFlash.isExpired()) {
    if (onFlag) {
      setColor( OFF );
      onFlag = false;
    } else {
      setColor( GREEN );
      onFlag = true; 
    }
    nextFlash.set(500);   
  }

  if (buttonDoubleClicked()){  // single click to get a new colour and keep playing
    pickColour(); 
    score = 0;
    gameState = GAME;
  }
}

////// RANDOM //////
void pickColour(){
  yourColourIndex = random(4)+1; // I want to keep 0 out of the valid indexes
}
