// This #include statement was automatically added by the Particle IDE.
#include "GamePiece.h"

#include "application.h"
#include "neopixel/neopixel.h"

SYSTEM_MODE(AUTOMATIC);

#define LED_ROWS 16
#define LED_COLUMNS 8
const int LED_COUNT = LED_ROWS * LED_COLUMNS;

const int GAME_COLUMNS = LED_COLUMNS;
const int GAME_ROWS = LED_ROWS;

int DLY = 210;

#define RED       0xFF0000
#define BLUE      0x0000FF
#define GREEN     0x00FF00
#define WHITE     0xFFFFFF
#define BLACK     0x000000
#define YELLOW	  0xFFFF00
#define SALMON	  0x0000AA
#define MAGENTA	  0xAA0000
#define INDIGO 	  0x00AA00

byte gameField[LED_COUNT];

// NEO PIXEL SETUP
#define PIXEL_PIN 5
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIXEL_PIN, PIXEL_TYPE);

// JOYSTICK SETUP
#define BTN_PIN 6
#define XIN_PIN A0
#define YIN_PIN A1

// MVMT SETUP
int prevBtn = 1;
int prevX = 0;
int prevY = 0;
int bri = 32;

int j = 0;
int delayTime = 20;



byte p1[4] = {1, 1, 1, 1};  
byte p2[6] = {2, 2, 2, 0, 2, 0};
byte p3[6] = {3, 0, 3, 0, 3, 3};
byte p4[6] = {0, 4, 0, 4, 4, 4};
byte p5[6] = {5, 5, 0, 0, 5, 5};
byte p6[6] = {0, 6, 6, 6, 6, 0};
byte p7[4] = {7, 7, 7, 7 };

GamePiece  _gamePieces[7] = 
{
  GamePiece(2, 2, p1 ),
  GamePiece(3, 2, p2 ),
  GamePiece(3, 2, p3 ),
  GamePiece(2, 3, p4 ),
  GamePiece(2, 3, p5 ),
  GamePiece(2, 3, p6 ),
  GamePiece(4, 1, p7 )
};

unsigned long colors[7] =
{
  RED,
  BLUE,
  GREEN,
  YELLOW,
  MAGENTA,
  SALMON,
  INDIGO  
};

GamePiece * fallingPiece = NULL;
GamePiece * rotated = NULL;
GamePiece * nextPiece = NULL;

byte gameLevel = 1;
byte currentRow = 0;
byte currentColumn = 0;
byte gameLines = 0;
boolean gameOver = false;
boolean yPressed = false;
boolean aPressed = false;
boolean bPressed = false;
boolean dPressed = false;
boolean lPressed = false;
boolean rPressed = false;

int leftStickY = 0;
int leftStickX = 0;


unsigned long loopStartTime = 0;

float Normalize(int min, int max, int value)
{
  float result = -1.0 + (float)((value - min) << 1) / (max - min);
  return result < -1 ? -1 : result > 1 ? 1 : result;
}

void clearLEDs()
{
   for (int i=0; i<LED_COUNT; i++) leds.setPixelColor(i, YELLOW);
}



void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(XIN_PIN, INPUT);
  pinMode(YIN_PIN, INPUT);
  
  leds.begin();
  clearLEDs();
  leds.setBrightness(bri);
  leds.show();


  randomSeed(analogRead(A0));

  startGame();
  leds.show();
  
}

void loop() {
    
    // handle button click
    int btnval = digitalRead(BTN_PIN);
    if(prevBtn != btnval && !btnval) {
        // TODO: ADD BTN CLICK EVENT
        drop();
    }
    prevBtn = btnval;
    
    
    // handle x change
    int xval = analogRead(XIN_PIN);
    if(xval < 300) { xval = -1; } else if(xval > 3800) { xval = 1; } else { xval = 0; }
    if(xval != prevX && xval != 0) {
        if(xval == 1) {
            // TODO: ADD LEFT EVENT
            moveLeft();
        } else {
            // TODO: ADD LEFT EVENT
            moveRight();
        }
    }
    prevX = xval;
    
    
    // handle x change
    int yval = analogRead(YIN_PIN);
    if(yval < 300) { yval = -1; } else if(yval > 3800) { yval = 1; } else { yval = 0; }
    if(yval != prevY && yval != 0) {
        if(yval == 1) {
            // TODO: ADD TOP EVENT
            rotateRight();
        } else {
            // TODO: ADD BOTTOM EVENT
            moveDown();
            delay(DLY);
        }
    }
    prevY = yval;
    
    if( millis() - loopStartTime > (300 / (gameLevel * 0.40)) )  {
    if( !gameOver )
    {        
      moveDown();
      gameOver = !isValidLocation(*fallingPiece, currentColumn, currentRow);       
      if(gameOver) exec_gameOver();
    }
    loopStartTime = millis();
    
    }
    
    render();
}


void setColor( int r, int c, unsigned long color)
{
    
    int row = 7-c;
    int column = 7-r;
    
    int odd = row%2;
    int led = 0;
    
    if( !odd )
        led = (row<<4)+column; 
    else
        led= ((row+1)<<4)-1 - column;
    
    //	leds.setPixelColor(led,color);
    int Lx = row+1;
    int Cx = column+1;
    if(Lx <= 8)  leds.setPixelColor((LED_COLUMNS-Cx)*LED_COLUMNS+Lx-1, color); 
    else  leds.setPixelColor((LED_ROWS-Cx-1)*LED_COLUMNS+Lx-1, color);

}


void render()
{
  int value = 0;
  unsigned long color = 0;

  //render game field first
  for( int row = 0; row < GAME_ROWS; row++)
  {
    for( int col = 0; col < GAME_COLUMNS; col++)
    {
      color = 0;
      value = gameField[row * GAME_COLUMNS+col];
      if( value > 0) color = colors[value-1];
      setColor(row,col, color);
    }
  }

  //render falling piece
  for( int row = 0; row < fallingPiece->Rows; row++)
  {
    for( int col = 0; col < fallingPiece->Columns; col++)
    {
        value = fallingPiece->getV(row,col);
        if( value > 0) setColor(currentRow+row,currentColumn+col, colors[value-1]);
    }
  }

  /*
	//render divider line
   	for( int row = 0; row < LED_ROWS; row++)
   	{
   		for( int col = GAME_COLUMNS; col < LED_COLUMNS; col ++)
   		{
   			if( col == GAME_COLUMNS )
   				setColor(row,col, WHITE);
   			else
   				setColor(row,col, 0);
   		}
   	}
   
   	//render next piece
   	for( int row = 0; row < nextPiece->Rows; row++)
   	{
   		for( int col = 0; col < nextPiece->Columns; col++)
   		{
   			value = (*nextPiece)(row,col);
   			if( value > 0)    
   				setColor(7+row,12+col, colors[value-1]);
   			else
   				setColor(7+row,12+col, 0);
   		}
   	}  
   */


  leds.show();
}



void startGame()
{
  //	Serial.println("Start game");

  clearLEDs();


  for(int i = 15;i >= 0;i--)
  {
    for(int j = 0;j < 8;j++)  setColor(i, j, 255);
    leds.show();
    delay(20);
    //tone(2,i*300);
    delay(10);
    for(int j = 0;j < 8;j++)  setColor(i, j, 0);
    //noTone(2);
  }
  leds.show();

  //bar.clear();
  //bar.writeDisplay();

  nextPiece=NULL;
  gameLines = 0;
  loopStartTime = 0;
  newLevel(1);
  gameOver = false;
  render();
  /*tone(2,500);
  delay(200);
  tone(2,1000);
  delay(200);
  tone(2,500);
  delay(200);
  noTone(2);*/

}


void newLevel(uint8_t level)
{
  gameLevel = level;

  //bar.clear();
  //for(int b = 12;b > 12-level;b--) //bar.setBar(b-1, LED_RED);
  //bar.writeDisplay();

  if(gameLevel == 1)
  {
    emptyField();
  }

  newPiece();
}


void emptyField()
{
  for(int i = 0; i < GAME_ROWS * GAME_COLUMNS; i++ ) gameField[i] = 0;
}


void newPiece()
{
  int next;

  currentColumn = 4;
  currentRow = 0;

  if (nextPiece == NULL)
  {
    next = random(0,7);
    nextPiece = &_gamePieces[next];
  }

    if(fallingPiece != NULL) {free(fallingPiece->_data);delete fallingPiece;};
    fallingPiece = new GamePiece(*nextPiece);

  next = random(0,7);
  nextPiece = &_gamePieces[next];  
}


boolean isValidLocation(GamePiece & piece, byte column, byte row)
{
  for (int i = 0; i < piece.Rows; i++)
    for (int j = 0; j < piece.Columns; j++)
    {
      int newRow = i + row;
      int newColumn = j + column;                    

      //location is outside of the fieled
      if (newColumn < 0 || newColumn > GAME_COLUMNS - 1 || newRow < 0 || newRow > GAME_ROWS - 1)
      {
        //piece part in that location has a valid square - not good
        if (piece(i, j) != 0) return false;
      }
      else
      {
        //location is in the field but is already taken, pice part for that location has non-empty square 
        if (gameField[newRow*GAME_COLUMNS + newColumn] != 0 && piece(i, j) != 0) return false;
      }
    }

  return true;  
}


void moveDown()
{
  if (isValidLocation(*fallingPiece, currentColumn, currentRow + 1))
  {
    currentRow +=1;
    return;
  }


  //The piece can't be moved anymore, merge it into the game field
  for (int i = 0; i < fallingPiece->Rows; i++)
  {
    for (int j = 0; j < fallingPiece->Columns; j++)
    {
      byte value = (*fallingPiece)(i, j);
      if (value != 0) gameField[(i + currentRow) * GAME_COLUMNS + (j + currentColumn)] = value;
    }
  }

  //Piece is merged update the score and get a new pice
  updateScore();            
  newPiece();  
}


void drop()
{
  while (isValidLocation(*fallingPiece, currentColumn, currentRow + 1)) moveDown();
  loopStartTime = -500;
  delay(DLY);  
}

void moveLeft()
{
  if (isValidLocation(*fallingPiece, currentColumn - 1, currentRow)) currentColumn--;
  delay(DLY);  
}



void moveRight()
{
  if (isValidLocation(*fallingPiece, currentColumn + 1, currentRow)) currentColumn++;
  delay(DLY);  

}


void rotateRight()
{
    rotated = fallingPiece->rotateRight();

  if (isValidLocation(*rotated, currentColumn, currentRow)) 
  {
      free(fallingPiece->_data);
      delete fallingPiece;

      fallingPiece = rotated;
  } else {free(rotated->_data);delete rotated;};

  
  delay(DLY);  
}


void rotateLeft()
{
  GamePiece * rotated = fallingPiece->rotateLeft();

  if (isValidLocation(*rotated, currentColumn, currentRow)) 
  {
     delete fallingPiece;
     fallingPiece = rotated;
  }
  delay(DLY);  
}


void updateScore()
{
  int count = 0;
  for(int row = 1; row < GAME_ROWS; row++)
  {
    boolean goodLine = true;
    for (int col = 0; col < GAME_COLUMNS; col++) if(gameField[row *GAME_COLUMNS + col] == 0) goodLine = false;

    if(goodLine)
    {
      count++;
      for (int i = row; i > 0; i--)
      {
        //tone(2,300+(i*100));
        digitalWrite(4,HIGH);  // liga motor vibracao
        delay(10);
        for (int j = 0; j < GAME_COLUMNS; j++) gameField[i *GAME_COLUMNS +j] = gameField[(i - 1)*GAME_COLUMNS+ j];
        //noTone(2);
        digitalWrite(4,LOW);
        render();
      }
    }
  }


  if (count > 0)
  {
    //_gameScore += count * (_gameLevel * 10);
    gameLines += count;


    int nextLevel = (gameLines / GAME_ROWS) + 1;
    int t = gameLines-(gameLines/GAME_ROWS*16);
    t = 24 - map(t,1,16,1,13);
    //for(int b = 24;b >= t;b--) //bar.setBar(b, LED_GREEN);
    //bar.writeDisplay();


    if (nextLevel > gameLevel)
    {
      gameLevel = nextLevel;
      newLevel(gameLevel);
      /*tone(2,300);
      delay(100);
      tone(2,500);
      delay(200);
      tone(2,300);
      delay(100);
      tone(2,500);
      delay(200);
      noTone(2);*/
    }
  }
}

void exec_gameOver()
{
  delay(100);
  clearLEDs();

  for(int i = 0;i < 16;i++)
  {
    for(int j = 0;j < 8;j++)  setColor(i, j, 255);
    leds.show();
    delay(20);
    //tone(2,i*300);
    delay(10);
    for(int j = 0;j < 8;j++)  setColor(i, j, 0);
    //noTone(2);
  }
  leds.show();

  startGame();
}


int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}