#include <memorysaver.h>
#include <UTFT.h>
#include <avr/pgmspace.h>

//////////////////////////////////// Screen square class //////////////////////////////////
//
// This class is used to represent a 10x10 screen area
// A square will represent a possible spaces on the screen
class ScreenSquare
{
  public:
  // X and Y position of the square
  int squareX;
  int squareY;
  
  // Boolean to determine is the square is empty
  boolean squareEmpty;
  
  // String to store the colour of the square
  String squareColour;
  
  // method for initialising the square values
  virtual void initialiseSquare(int x, int y, boolean empty) 
  { 
     squareX = x;
     squareY = y;
     squareEmpty = empty;
     squareColour = "White";
  }
};;
///////////////////////////////////////////////////////////////////////////////////////////

// This creates a 2D array of screen squares
ScreenSquare screenSpaceArray[23][13];

// Declare the fonts 
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Setup the LCD
UTFT myLCD(ITDB32S,38,39,40,41); 

// Button pins
const int downButtonPin = 12; // A button
const int leftButtonPin = 13; // X button
const int upButtonPin = 10; // Y button
const int rightButtonPin = 11; // B button

// Button states
int leftButtonState = 0;         
int rightButtonState = 0; 
int upButtonState = 0; 
int downButtonState = 0; 

// Variables for the individual squares
int squareX = 80;
int squareY = 0;
int squareWidth = 10;
int squareHeight = 10;

// Variables for the shapes
String currentShape = "Line";
String currentColour = "Green";
String nextColour = "Blue";
int row = 0;
int column = 6;
int shapeOrientation = 0;
int firstChange = 0;

// Random numbers
long randNextShape;
long randShapeColour;

// Array for each possible space
int spaces[23][13];

// Variables for the game
int score = 0;
int lines = 0;
int gameSpeed = 200;

//********************************** Setup metheod **********************************
void setup()
{
  // Gets a random seed by reading the value of an un-connected pin
  randomSeed(analogRead(0));
  
  // Initialise the button pins as an input
  pinMode(leftButtonPin, INPUT);  
  pinMode(rightButtonPin, INPUT); 
  pinMode(upButtonPin, INPUT); 
  pinMode(downButtonPin, INPUT); 
  
  // Initialises the LCD and sets the colours
  myLCD.InitLCD();
  myLCD.fillScr(255, 255, 255);
  myLCD.setColor(0, 0, 0);
  
  // Draws the barriers
  drawBarriers();
  
  // X and Y positions for each of the squares
  int spaceX = 20;
  int spaceY = 0;
  
  // Initialise the 2D array of screen squares
  for(int i=0; i<23; i++) 
  {
    for(int j=0; j<13; j++)
    {
      // Row 23 is the very bottom line of the playing area
      // Row 23 squares are set false to create the initial bottom bounds
      if (i == 23)
      {
        screenSpaceArray[i][j] = ScreenSquare();
        screenSpaceArray[i][j].initialiseSquare(spaceX,spaceY, false);
      }
      else
      {
        // All of the other squares are initialised true to indicate the the space is empty and available
        screenSpaceArray[i][j] = ScreenSquare();
        screenSpaceArray[i][j].initialiseSquare(spaceX,spaceY, true);
      }
      // X position is incremented
      spaceX += 10;
    }
    // X position is reset
    spaceX = 20;
    
    // Y position is incremented
    spaceY += 10;
  }

  // Creates the initial shape
  if (currentShape == "Block")
  {
    drawBlockShape(currentColour);
  }
  else if (currentShape == "Line")
  {
    drawLineShape(currentColour);
  }
  
  displayNextShape();
}


//********************************** Loop method **********************************
void loop()
{
  // Draw the game text
  drawGameText();
  
  // If shape is a block
  if (currentShape == "Block")
  {
    if (column < 12)
    {
      // Checks the array to determine if shape can move down
      if (screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 2][column].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 2][column + 1].squareEmpty == true)
      {
        moveShapeDown();
        row += 1;
      }
      else 
      {
        // Sets the array values as 1 (That means that the space is occupied)
        screenSpaceArray[row][column].squareEmpty = false;
        screenSpaceArray[row][column].squareColour = currentColour;
        screenSpaceArray[row][column + 1].squareEmpty = false;
        screenSpaceArray[row][column + 1].squareColour = currentColour;
        screenSpaceArray[row +1][column].squareEmpty = false;
        screenSpaceArray[row +1][column].squareColour = currentColour;
        screenSpaceArray[row +1][column + 1].squareEmpty = false;
        screenSpaceArray[row +1][column + 1].squareColour = currentColour;
        
        // Creates a new shape
        shapeOrientation = 0;
        createNewShape();
        score += 10;
        
        // Checks for complete lines
        for(int i=22; i>0; i--)
         {
            checkLines(i);
         }
         
         checkGameOver();
      }
    }
  }
  else if (currentShape == "Line")
  {
    if (shapeOrientation == 0)
    {
      // Checks the array to determine if shape can move down
      if (screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 1][column + 3].squareEmpty == true)
      {
        moveShapeDown();
        row += 1;
      }
      else 
      {
        // Sets the array values as 1 (That means that the space is occupied)
        screenSpaceArray[row][column].squareEmpty = false;
        screenSpaceArray[row][column].squareColour = currentColour;
        screenSpaceArray[row][column + 1].squareEmpty = false;
        screenSpaceArray[row][column + 1].squareColour = currentColour;
        screenSpaceArray[row][column + 2].squareEmpty = false;
        screenSpaceArray[row][column + 2].squareColour = currentColour;
        screenSpaceArray[row][column + 3].squareEmpty = false;
        screenSpaceArray[row][column + 3].squareColour = currentColour;
        
        // Creates a new shape
        shapeOrientation = 0;
        createNewShape();
        score += 10;
        
        for(int i=22; i>0; i--) //This loops on the columns
         {
            checkLines(i);
         }
         
         checkGameOver();
      }
    }
    else if (shapeOrientation == 1)
    {
      if (screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 2][column].squareEmpty == true && screenSpaceArray[row + 3][column].squareEmpty == true && screenSpaceArray[row + 4][column].squareEmpty == true)
      {
        moveShapeDown();
        row += 1;
      }
      else 
      {
        // Sets the array values as 1 (That means that the space is occupied)
        screenSpaceArray[row][column].squareEmpty = false;
        screenSpaceArray[row][column].squareColour = currentColour;
        screenSpaceArray[row + 1][column].squareEmpty = false;
        screenSpaceArray[row + 1][column].squareColour = currentColour;
        screenSpaceArray[row + 2][column].squareEmpty = false;
        screenSpaceArray[row + 2][column].squareColour = currentColour;
        screenSpaceArray[row + 3][column].squareEmpty = false;
        screenSpaceArray[row + 3][column].squareColour = currentColour;
        
        // Creates a new shape
        shapeOrientation = 0;
        createNewShape();
        score += 10;
        
        for(int i=22; i>0; i--) //This loops on the columns
         {
            checkLines(i);
         }
         
         checkGameOver();
      }
    }
  }
  else if (currentShape == "Z")
  {
    if (shapeOrientation == 0)
    {
      // Checks the array to determine if shape can move down
      if (screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 2][column].squareEmpty == true && screenSpaceArray[row + 2][column + 1].squareEmpty == true && screenSpaceArray[row + 3][column + 1].squareEmpty == true)
      {
        moveShapeDown();
        row += 1;
      }
      else 
      {
        // Sets the array values as 1 (That means that the space is occupied)
        screenSpaceArray[row][column].squareEmpty = false;
        screenSpaceArray[row][column].squareColour = currentColour;
        screenSpaceArray[row + 1][column].squareEmpty = false;
        screenSpaceArray[row + 1][column].squareColour = currentColour;
        screenSpaceArray[row + 1][column + 1].squareEmpty = false;
        screenSpaceArray[row + 1][column + 1].squareColour = currentColour;
        screenSpaceArray[row + 2][column + 1].squareEmpty = false;
        screenSpaceArray[row + 2][column + 1].squareColour = currentColour;
        
        // Creates a new shape
        shapeOrientation = 0;
        createNewShape();
        score += 10;
        
        for(int i=22; i>0; i--)
         {
            checkLines(i);
         }
         
         checkGameOver();
      }
    }
    else if (shapeOrientation == 1)
    {
      if (screenSpaceArray[row + 2][column].squareEmpty == true && screenSpaceArray[row + 2][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 2].squareEmpty == true)
      {
        moveShapeDown();
        row += 1;
      }
      else 
      {
        // Sets the array values as 1 (That means that the space is occupied)
        screenSpaceArray[row + 1][column].squareEmpty = false;
        screenSpaceArray[row + 1][column].squareColour = currentColour;
        screenSpaceArray[row][column + 1].squareEmpty = false;
        screenSpaceArray[row][column + 1].squareColour = currentColour;
        screenSpaceArray[row + 1][column + 1].squareEmpty = false;
        screenSpaceArray[row + 1][column + 1].squareColour = currentColour;
        screenSpaceArray[row][column + 2].squareEmpty = false;
        screenSpaceArray[row][column + 2].squareColour = currentColour;
        
        // Creates a new shape
        shapeOrientation = 0;
        createNewShape();
        score += 10;
        
        for(int i=22; i>0; i--)
         {
            checkLines(i);
         }
         
         checkGameOver();
      } 
    }
  }
  else if (currentShape == "L")
  {
    if (shapeOrientation == 0)
    {
      // Checks the array to determine if shape can move down
      if (screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 2][column].squareEmpty == true && screenSpaceArray[row + 3][column].squareEmpty == true && screenSpaceArray[row + 3][column + 1].squareEmpty == true)
      {
        moveShapeDown();
        row += 1;
      }
      else 
      {
        // Sets the array values as 1 (That means that the space is occupied)
        screenSpaceArray[row][column].squareEmpty = false;
        screenSpaceArray[row][column].squareColour = currentColour;
        screenSpaceArray[row + 1][column].squareEmpty = false;
        screenSpaceArray[row + 1][column].squareColour = currentColour;
        screenSpaceArray[row + 2][column].squareEmpty = false;
        screenSpaceArray[row + 2][column].squareColour = currentColour;
        screenSpaceArray[row + 2][column + 1].squareEmpty = false;
        screenSpaceArray[row + 2][column + 1].squareColour = currentColour;
        
        // Creates a new shape
        shapeOrientation = 0;
        createNewShape();
        score += 10;
        
        for(int i=22; i>0; i--) //This loops on the columns
         {
            checkLines(i);
         }
         
         checkGameOver();
      }
    }
    else if (shapeOrientation == 1)
    {
      // Checks the array to determine if shape can move down
      if (screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row][column + 2].squareEmpty == true)
      {
        moveShapeDown();
        row += 1;
      }
      else 
      {
        // Sets the array values as 1 (That means that the space is occupied)
        screenSpaceArray[row][column].squareEmpty = false;
        screenSpaceArray[row][column].squareColour = currentColour;
        screenSpaceArray[row][column + 1].squareEmpty = false;
        screenSpaceArray[row][column + 1].squareColour = currentColour;
        screenSpaceArray[row][column + 2].squareEmpty = false;
        screenSpaceArray[row][column + 2].squareColour = currentColour;
        screenSpaceArray[row - 1][column + 2].squareEmpty = false;
        screenSpaceArray[row - 1][column + 2].squareColour = currentColour;
        
        // Creates a new shape
        shapeOrientation = 0;
        createNewShape();
        score += 10;
        
        for(int i=22; i>0; i--) //This loops on the columns
         {
            checkLines(i);
         }
         
         checkGameOver();
      }
    }
  }
  
  // Check the button state
  checkButtonInput();
}


//********************************** Draw Block shape **********************************
// This method draws the square block shape

void drawBlockShape(String shapeColour)
{
  // Draws the shape from individual squares
  drawSingleSquare(squareX, squareY, shapeColour);
  drawSingleSquare(squareX + 10, squareY, shapeColour);
  drawSingleSquare(squareX, squareY + 10, shapeColour);
  drawSingleSquare(squareX + 10, squareY + 10, shapeColour);
}


//********************************** Draw Line shape **********************************
// this method draws the straight line shape

void drawLineShape(String shapeColour)
{
  if (shapeOrientation == 0)
  {
    // Draws the shape from individual squares
    drawSingleSquare(squareX, squareY, shapeColour);
    drawSingleSquare(squareX + 10, squareY, shapeColour);
    drawSingleSquare(squareX + 20, squareY, shapeColour);
    drawSingleSquare(squareX + 30, squareY, shapeColour);
  }
  else if (shapeOrientation == 1)
  {
    // Draws the shape from individual squares
    drawSingleSquare(squareX, squareY, shapeColour);
    drawSingleSquare(squareX, squareY + 10, shapeColour);
    drawSingleSquare(squareX, squareY + 20, shapeColour);
    drawSingleSquare(squareX, squareY + 30, shapeColour);
  } 
}


//********************************** Draw Z shape **********************************
// This method drws the Z shape

void drawZShape(String shapeColour)
{
  if (shapeOrientation == 0)
  {
    // Draws the shape from individual squares
    drawSingleSquare(squareX, squareY, shapeColour);
    drawSingleSquare(squareX, squareY + 10, shapeColour);
    drawSingleSquare(squareX + 10, squareY + 10, shapeColour);
    drawSingleSquare(squareX + 10, squareY + 20, shapeColour);
  }
  else if (shapeOrientation == 1)
  {
    // Draws the shape from individual squares
    drawSingleSquare(squareX, squareY + 10, shapeColour);
    drawSingleSquare(squareX + 10, squareY, shapeColour);
    drawSingleSquare(squareX + 10, squareY + 10, shapeColour);
    drawSingleSquare(squareX + 20, squareY, shapeColour);
  } 
}


//********************************** Draw L shape **********************************
// This method drwas the L shape

void drawLShape(String shapeColour)
{
  if (shapeOrientation == 0)
  {
    // Draws the shape from individual squares
    drawSingleSquare(squareX, squareY, shapeColour);
    drawSingleSquare(squareX, squareY + 10, shapeColour);
    drawSingleSquare(squareX, squareY + 20, shapeColour);
    drawSingleSquare(squareX + 10, squareY + 20, shapeColour);
  }
  else if (shapeOrientation == 1)
  {
    // Draws the shape from individual squares
    drawSingleSquare(squareX, squareY, shapeColour);
    drawSingleSquare(squareX + 10, squareY, shapeColour);
    drawSingleSquare(squareX + 20, squareY, shapeColour);
    drawSingleSquare(squareX + 20, squareY - 10, shapeColour);
  } 
}


//********************************** Create new shape **********************************
// This method cretaes the next shape that will be generated

void createNewShape()
{
  squareX = 80;
  squareY = 0;
  
  // Sets the shapes colour
  currentColour = nextColour;
  
  // Creates the new shape depending on the random number
  if (randNextShape == 1)
  {
    currentShape = "Block";
    drawBlockShape(currentColour);
  }
  else if (randNextShape == 2)
  {
    currentShape = "Line";
    drawLineShape(currentColour);
  }  
  else if (randNextShape == 3)
  {
    currentShape = "Z";
    drawZShape(currentColour);
  }  
  else if (randNextShape == 4)
  {
    currentShape = "L";
    drawLShape(currentColour);
  }  
  
  // Sets the start location for the new shape
  column = 6;
  row = 0;
  
  // Generates random values for the next shape
  setRandomShape();
}


//********************************** Draws a single square **********************************
// This method draws a 10x10 square which is used as a piece of a shape in the game

void drawSingleSquare(int x, int y, String shapeColour)
{
    // Set colour black
    myLCD.setColor(0, 0, 0);
  
    // Draws the outer square
    myLCD.fillRect(x,y,x + squareWidth,y + squareHeight);
  
    // Sets the squares colour
    if (shapeColour == "Red")
    {
      // Set colour red
      myLCD.setColor(247, 78, 78);
    }
    else if (shapeColour == "Blue")
    {
      // Set colour blue
      myLCD.setColor(96, 96, 248);
    }
    else if (shapeColour == "Green")
    {
      // Set colour blue
      myLCD.setColor(108, 245, 94);
    }
    else if (shapeColour == "Yellow")
    {
      // Set colour yellow
      myLCD.setColor(236, 238, 41);
    }
    else if (shapeColour == "Purple")
    {
      // Set colour purple
      myLCD.setColor(210, 101, 241);
    }
    
    // Draws the inner square
    myLCD.fillRect(x + 2,y + 2,x + squareWidth - 2,y + squareHeight - 2);
    
    // Set colour back to black
    myLCD.setColor(0, 0, 0);
}


//********************************** Moves shape down **********************************
// This method moves the current shape down the screen until it hits the bottom or another shape
// Each of the shapes need to be treated differently
  
void moveShapeDown()
{
  // Set colour white
  myLCD.setColor(255, 255, 255);
  
  // If the shape is a block
  if (currentShape == "Block")
  {
    // Draws over the previous shape with white pixels
    // If there is currently a shape on the left hand side but not on the right hand side
    if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column +2].squareEmpty == true)
    {
      myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth,squareY + squareHeight);
      myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 10,squareY + squareHeight);
    }
    // If there is currently a shape on the right hand side but not on the left hand side
    else if (screenSpaceArray[row][column -1].squareEmpty == true && screenSpaceArray[row][column +2].squareEmpty == false)
    {
      myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
      myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 9,squareY + squareHeight);
    }
    // If there is currently a shape on the left and right hand side
    else if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column +2].squareEmpty == false)
    {
      myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth,squareY + squareHeight);
      myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 9,squareY + squareHeight);
    }
    else
    {
      myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
      myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 10,squareY + squareHeight);
    }
    
     // Increments the Y position
     squareY += 10;
      
     // Draws the shape in  the new location
     drawBlockShape(currentColour);
  }
  // if the shape is a line
  else if (currentShape == "Line")
  {
    if (shapeOrientation == 0)
    {
      if (firstChange == 0)
      {
        // Draws over the previous shape after the orientation has just changed
        myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight * 4);
        firstChange += 1;
      }
  
       // If there is currently a shape on the left hand side but not on the right hand side
       if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 4].squareEmpty == true)
       {
         // Draws over the previous shape white
         myLCD.fillRect(squareX + 1,squareY,squareX + 40,squareY + squareHeight);
       }
       // If there is currently a shape on the right hand side but not on the left hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == true && screenSpaceArray[row][column + 4].squareEmpty == false)
       {
         // Draws over the previous shape white
         myLCD.fillRect(squareX,squareY,squareX + 40 - 1,squareY + squareHeight);
       }
       // If there is currently a shape on the left and right hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 4].squareEmpty == false)
       {
         // Draws over the previous shape white
         myLCD.fillRect(squareX + 1,squareY,squareX + 40 - 1,squareY + squareHeight);
       }
       else
       {
         // Draws over the previous shape white
         myLCD.fillRect(squareX,squareY,squareX + 40,squareY + squareHeight);
       }
      
        // Increments the Y position
        squareY += 10;
        
        // Draws the shape in  the new location
        drawLineShape(currentColour);
    }
    else if (shapeOrientation == 1)
    {
      if (firstChange == 0)
      {
          // Draws over the previous shape after the orientation has just changed
          myLCD.fillRect(squareX,squareY,squareX + 40,squareY + squareHeight);
          firstChange += 1;
      }
      
       // If there is currently a shape on the left hand side but not on the right hand side
       if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 1].squareEmpty == true)
       {
         myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth,squareY + squareHeight * 4);
       }
       // If there is currently a shape on the right hand side but not on the left hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == true && screenSpaceArray[row][column + 1].squareEmpty == false)
       {
         myLCD.fillRect(squareX,squareY,squareX + squareWidth - 1,squareY + squareHeight * 4);
       }
       // If there is currently a shape on the left and right hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 1].squareEmpty == false)
       {
         myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth - 1,squareY + squareHeight * 4);
       }
       else
       {
         myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight * 4);
       }
       
       // Increments the Y position
       squareY += 10;
      
      // Draws the shape in  the new location
      drawLineShape(currentColour);
      }
    }
    else if (currentShape == "Z")
    {
      if (shapeOrientation == 0)
      {
        if (firstChange == 0)
        {  
           myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
           myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 20,squareY + squareHeight);
           firstChange += 1;
        }
        // If there is currently a shape on the left hand side but not on the right hand side
       if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 2].squareEmpty == true)
       {
         myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth,squareY + squareHeight);
         myLCD.fillRect(squareX +10,squareY + 10,squareX + squareWidth + 10,squareY + squareHeight + 10);
       }
       // If there is currently a shape on the right hand side but not on the left hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == true && screenSpaceArray[row + 1][column + 2].squareEmpty == false)
       {
         myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
         myLCD.fillRect(squareX + 10,squareY + 10,squareX + squareWidth + 9,squareY + squareHeight + 10);
       }
       // If there is currently a shape on the left and right hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 2].squareEmpty == false)
       {
         myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth,squareY + squareHeight);
         myLCD.fillRect(squareX + 10,squareY + 10,squareX + squareWidth + 9,squareY + squareHeight + 10);
       }
       else
       {
         myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
         myLCD.fillRect(squareX +10,squareY + 10,squareX + squareWidth + 10,squareY + squareHeight + 10);
       }
       
        // Increments the Y position
        squareY += 10;
        
        // Draws the shape in  the new location
        drawZShape(currentColour);
      }
      else if (shapeOrientation == 1)
      {
        if (firstChange == 0)
        {  
           myLCD.fillRect(squareX,squareY - 10,squareX + squareWidth,squareY + squareHeight);
           firstChange += 1;
        }
        // If there is currently a shape on the left hand side but not on the right hand side
       if (screenSpaceArray[row + 1][column -1].squareEmpty == false && screenSpaceArray[row][column + 3].squareEmpty == true)
       {
         myLCD.fillRect(squareX + 1,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
         myLCD.fillRect(squareX +10,squareY,squareX + squareWidth + 20,squareY + squareHeight);
       }
       // If there is currently a shape on the right hand side but not on the left hand side
       else if (screenSpaceArray[row + 1][column -1].squareEmpty == true && screenSpaceArray[row][column + 3].squareEmpty == false)
       {
         myLCD.fillRect(squareX,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
         myLCD.fillRect(squareX +10,squareY,squareX + squareWidth + 19,squareY + squareHeight);
       }
       // If there is currently a shape on the left and right hand side
       else if (screenSpaceArray[row + 1][column -1].squareEmpty == false && screenSpaceArray[row][column + 3].squareEmpty == false)
       {
         myLCD.fillRect(squareX + 1,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
         myLCD.fillRect(squareX +10,squareY,squareX + squareWidth + 19,squareY + squareHeight);
       }
       else
       {
         myLCD.fillRect(squareX,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
         myLCD.fillRect(squareX +10,squareY,squareX + squareWidth + 20,squareY + squareHeight);
       }
    
        // Increments the Y position
        squareY += 10;
        
        // Draws the shape in  the new location
        drawZShape(currentColour);
      }
    }
    else if (currentShape == "L")
    {
      if (shapeOrientation == 0)
      {
        if (firstChange == 0)
        {  
          myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 20,squareY + squareHeight);
          myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 20,squareY + squareHeight - 10);
          firstChange += 1;
        }

       // If there is currently a shape on the left hand side but not on the right hand side
       if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row + 2][column + 2].squareEmpty == true)
       {
         myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth,squareY + squareHeight * 3);
         myLCD.fillRect(squareX + 10,squareY + 20,squareX + squareWidth + 10,squareY + squareHeight + 20);
       }
       // If there is currently a shape on the right hand side but not on the left hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == false)
       {
         myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight * 3);
         myLCD.fillRect(squareX + 10,squareY + 20,squareX + squareWidth + 9,squareY + squareHeight + 20);
       }
       // If there is currently a shape on the left and right hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row + 2][column + 2].squareEmpty == false)
       {
          myLCD.fillRect(squareX + 1,squareY,squareX + squareWidth,squareY + squareHeight * 3);
          myLCD.fillRect(squareX + 10,squareY + 20,squareX + squareWidth + 9,squareY + squareHeight + 20);
       }
       else
       {
         myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight * 3);
         myLCD.fillRect(squareX + 10,squareY + 20,squareX + squareWidth + 10,squareY + squareHeight + 20);
       }
        
         // Increments the Y position
         squareY += 10;
          
         // Draws the shape in  the new location
         drawLShape(currentColour);
    }
    else if (shapeOrientation == 1)
    {
      if (firstChange == 0)
      {
         // Draws over the previous shape after the orientation has just changed
         myLCD.fillRect(squareX,squareY + 20,squareX + squareWidth + 10,squareY + squareHeight + 20);
         firstChange += 1;
      }
      
       // If there is currently a shape on the left hand side but not on the right hand side
       if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 3].squareEmpty == true)
       {
         // Draws over the previous shape white
         myLCD.fillRect(squareX + 1,squareY,squareX + 30,squareY + squareHeight);
         myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 20,squareY + squareHeight);
       }
       // If there is currently a shape on the right hand side but not on the left hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == true && screenSpaceArray[row][column + 3].squareEmpty == false)
       {
         if (screenSpaceArray[row - 1][column + 3].squareEmpty == false)
         {
           // Draws over the previous shape white
           myLCD.fillRect(squareX,squareY,squareX + 29,squareY + squareHeight);
           myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 19,squareY + squareHeight);
         }
         else
         {
           // Draws over the previous shape white
           myLCD.fillRect(squareX,squareY,squareX + 30,squareY + squareHeight);
           myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 20,squareY + squareHeight);
         }
       }
       // If there is currently a shape on the left and right hand side
       else if (screenSpaceArray[row][column -1].squareEmpty == false && screenSpaceArray[row][column + 3].squareEmpty == false)
       {
         if (screenSpaceArray[row - 1][column + 3].squareEmpty == false)
         {
           // Draws over the previous shape white
           myLCD.fillRect(squareX + 1,squareY,squareX + 29,squareY + squareHeight);
           myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 19,squareY + squareHeight);
         }
         else
         {
           myLCD.fillRect(squareX + 1,squareY,squareX + 30,squareY + squareHeight);
           myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 20,squareY + squareHeight);
         }
       }
       else
       {
         // Draws over the previous shape white
         myLCD.fillRect(squareX,squareY,squareX + 30,squareY + squareHeight);
         myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 20,squareY + squareHeight);
       }
       
       // Increments the Y position
       squareY += 10;
      
      // Draws the shape in  the new location
      drawLShape(currentColour);
      }
    }

    // Adds a delay to control the game speed
    delay(gameSpeed);
  }


//********************************** Moves shape left or right **********************************
// This method enables the current shape to be moved left or right if the buttons has been pressed

void moveShape(int direction)
{
  // Set colour white
  myLCD.setColor(255, 255, 255);
  
  // If moving left
  if (direction == 1)
  {
     // If the shape is a block
     if (currentShape == "Block" && squareX > 20 && screenSpaceArray[row][column - 1].squareEmpty == true && screenSpaceArray[row + 1][column - 1].squareEmpty == true)
     {
         // Draws over the previous shape with white pixels
         myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
         myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 10,squareY + squareHeight);
         myLCD.fillRect(squareX,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
         myLCD.fillRect(squareX +10,squareY + 10,squareX + squareWidth + 10,squareY + squareHeight + 10);
         
         squareX -= 10;
         column -=1;
         
         // Draws the shape in  the new location
         drawBlockShape(currentColour);
     }
     // If the shape is a line
     else if (currentShape == "Line" && squareX > 20 && screenSpaceArray[row][column - 1].squareEmpty == true && screenSpaceArray[row + 1][column - 1].squareEmpty == true && 
             screenSpaceArray[row + 2][column - 1].squareEmpty == true && screenSpaceArray[row + 3][column - 1].squareEmpty == true)
       {
       if (shapeOrientation == 0)
       {
          // Draws over the previous shape with white pixels
          myLCD.fillRect(squareX,squareY,squareX + 40,squareY + squareHeight);
          
          squareX -= 10;
          column -=1;
          
          // Draws the shape in  the new location
          drawLineShape(currentColour);
       }
       else if (shapeOrientation == 1)
       {
          // Draws over the previous shape with white pixels
          myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight * 4);
          
          squareX -= 10;
          column -=1;
          
          // Draws the shape in  the new location
          drawLineShape(currentColour);
          }
       }
       // If the shape is a Z
       else if (currentShape == "Z" && squareX > 20 && screenSpaceArray[row][column - 1].squareEmpty == true && screenSpaceArray[row + 1][column - 1].squareEmpty == true)
         {
            if (shapeOrientation == 0)
            {
              myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
              myLCD.fillRect(squareX + 10,squareY + 10,squareX + squareWidth + 10,squareY + squareHeight + 10);
              myLCD.fillRect(squareX + 10,squareY + 20,squareX + squareWidth + 10,squareY + squareHeight + 20);
             
              squareX -= 10;
              column -=1;
             
              // Draws the shape in  the new location
              drawZShape(currentColour);
            }
            else if (shapeOrientation == 1)
            {
              myLCD.fillRect(squareX + 20,squareY,squareX + squareWidth + 20,squareY + squareHeight);
              myLCD.fillRect(squareX + 10,squareY + 10,squareX + squareWidth + 10,squareY + squareHeight + 10);
              
              squareX -= 10;
              column -=1;
             
              // Draws the shape in  the new location
              drawZShape(currentColour);
            } 
         }

         // If the shape is a L
       else if (currentShape == "L")
         {
            if (shapeOrientation == 0 && squareX > 20 && screenSpaceArray[row][column - 1].squareEmpty == true && screenSpaceArray[row + 1][column - 1].squareEmpty == true && screenSpaceArray[row + 2][column - 1].squareEmpty == true)
            {
              myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
              myLCD.fillRect(squareX,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
              myLCD.fillRect(squareX + 10,squareY + 20,squareX + squareWidth + 10,squareY + squareHeight + 20);

              squareX -= 10;
              column -=1;
             
              // Draws the shape in  the new location
              drawLShape(currentColour);
            }
            else if (shapeOrientation == 1)
            {
              myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 20,squareY + squareHeight);
              
              squareX -= 10;
              column -=1;
             
              // Draws the shape in  the new location
              drawLShape(currentColour);
            } 
         } 
     }
     // If moving right
     else if (direction == 2)
     {
       // If the shape is a block
       if (currentShape == "Block" && squareX < 130 && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true  && 
           screenSpaceArray[row][column + 2].squareEmpty == true && screenSpaceArray[row + 1][column + 2].squareEmpty == true)
       {
         // Draws over the previous shape with white pixels
         myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
         myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 10,squareY + squareHeight);
         myLCD.fillRect(squareX,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
         myLCD.fillRect(squareX +10,squareY + 10,squareX + squareWidth + 10,squareY + squareHeight + 10);
         
         squareX += 10;
         column +=1;
         
         // Draws the shape in  the new location
         drawBlockShape(currentColour);
       }
       // If the shape is a line
       else if (currentShape == "Line")
       {
         if (shapeOrientation == 0 && squareX < 110 && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true && 
             screenSpaceArray[row + 2][column + 1].squareEmpty == true && screenSpaceArray[row + 3][column + 1].squareEmpty == true)
           {
            // Draws over the previous shape with white pixels
            myLCD.fillRect(squareX,squareY,squareX + 40,squareY + squareHeight);
            
            squareX += 10;
            column +=1;
            
            // Draws the shape in  the new location
            drawLineShape(currentColour);
         }
         else if (shapeOrientation == 1 && squareX < 140 && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true && 
                 screenSpaceArray[row + 2][column + 1].squareEmpty == true && screenSpaceArray[row + 3][column + 1].squareEmpty == true)
           {
            // Draws over the previous shape with white pixels
            myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight * 4);
            
            squareX += 10;
            column +=1;
            
            // Draws the shape in  the new location
            drawLineShape(currentColour);
            }
         }
         // If the shape is a Z
         else if (currentShape == "Z")
         {
            if (shapeOrientation == 0 && squareX < 130 && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true  && 
                screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 3][column + 2].squareEmpty == true)
             {
              myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
              myLCD.fillRect(squareX,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
              myLCD.fillRect(squareX + 10,squareY + 20,squareX + squareWidth + 10,squareY + squareHeight + 20);
              
              squareX += 10;
              column +=1;
             
              // Draws the shape in  the new location
              drawZShape(currentColour);
            }
            else if (shapeOrientation == 1 && squareX < 120 && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row][column + 2].squareEmpty == true && 
                    screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 1][column + 3].squareEmpty == true)
            {
              myLCD.fillRect(squareX,squareY + 10,squareX + squareWidth,squareY + squareHeight + 10);
              myLCD.fillRect(squareX + 10,squareY,squareX + squareWidth + 10,squareY + squareHeight);
              
              squareX += 10;
              column +=1;
             
              // Draws the shape in  the new location
              drawZShape(currentColour);
            }
         }  
         // If the shape is a line
       else if (currentShape == "L")
       {
         if (shapeOrientation == 0 && squareX < 130 && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 1].squareEmpty == true && 
             screenSpaceArray[row + 2][column + 1].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == true)
          {
            // Draws over the previous shape with white pixels
            myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight * 3);

            squareX += 10;
            column +=1;
            
            // Draws the shape in  the new location
            drawLShape(currentColour);
         }
         else if (shapeOrientation == 1 && squareX < 120 && screenSpaceArray[row][column + 3].squareEmpty == true && screenSpaceArray[row - 1][column + 3].squareEmpty == true && 
                 screenSpaceArray[row][column + 2].squareEmpty == true && screenSpaceArray[row][column + 1].squareEmpty == true)
           {
            // Draws over the previous shape with white pixels
            myLCD.fillRect(squareX,squareY,squareX + squareWidth,squareY + squareHeight);
            myLCD.fillRect(squareX + 20,squareY - 10,squareX + squareWidth + 20,squareY + squareHeight - 10);
            
            squareX += 10;
            column +=1;
            
            // Draws the shape in  the new location
            drawLShape(currentColour);
            }
         }
      }
  }
  
  
//********************************** Generates random values for the next shape **********************************
// This method generates random numbers 
// The random numbers are used to determine the next shape that will be generated an it colour

void setRandomShape()
{
  // Generates a random colour for the new shape
  randShapeColour = random(1, 6);
  switch (randShapeColour) {
    case 1:
      nextColour = "Red";
      break;
    case 2:
      nextColour = "Green";
      break;
    case 3: 
      nextColour = "Blue";
      break;
    case 4:
      nextColour = "Yellow";
      break;
    case 5: 
      nextColour = "Purple";
      break;
  }  
  
  // Generates a random number for random shapes
  randNextShape = random(1, 5);
  
  // Displays the next shape
  displayNextShape();
}


//********************************** Checks if any lines have been filled **********************************
// This method checks to see if any of the lines have been filled
// If a line is full, the variable is incremented and all of the squares are moved down 1 row

void checkLines(int rowCheck)
{
  while (screenSpaceArray[rowCheck][0].squareEmpty == false && screenSpaceArray[rowCheck][1].squareEmpty == false && screenSpaceArray[rowCheck][2].squareEmpty == false && 
        screenSpaceArray[rowCheck][3].squareEmpty == false && screenSpaceArray[rowCheck][4].squareEmpty == false && screenSpaceArray[rowCheck][5].squareEmpty == false &&
        screenSpaceArray[rowCheck][6].squareEmpty == false && screenSpaceArray[rowCheck][7].squareEmpty == false && screenSpaceArray[rowCheck][8].squareEmpty == false &&
        screenSpaceArray[rowCheck][9].squareEmpty == false && screenSpaceArray[rowCheck][10].squareEmpty == false && screenSpaceArray[rowCheck][11].squareEmpty == false &&
        screenSpaceArray[rowCheck][12].squareEmpty == false)
        {
         // Increments the lines variable
         lines += 1;
        
         // Loops through the array backwards
         for(int i=rowCheck; i>2; i--)    
         {
          for(int j=0; j<13; j++) 
          {
            // If the square in the above row is not empty
            if (screenSpaceArray[i -1][j].squareEmpty == false)
            {
              // This draws the square from the above row into this row
              screenSpaceArray[i][j].squareColour = screenSpaceArray[i -1][j].squareColour;
              drawSingleSquare(screenSpaceArray[i][j].squareX, screenSpaceArray[i][j].squareY, screenSpaceArray[i][j].squareColour);
            }
            else if (screenSpaceArray[i -1][j].squareEmpty == true && screenSpaceArray[i -1][j - 1].squareEmpty == false && screenSpaceArray[i -1][j + 1].squareEmpty == true)
            {
              // This clears this square
              myLCD.setColor(255, 255, 255);
              myLCD.fillRect(screenSpaceArray[i][j].squareX + 1,screenSpaceArray[i][j].squareY,screenSpaceArray[i][j].squareX + squareWidth,screenSpaceArray[i][j].squareY + squareHeight - 1);
              screenSpaceArray[i][j].squareEmpty = true;
              screenSpaceArray[i][j].squareColour = "White";
            }
            else if (screenSpaceArray[i -1][j].squareEmpty == true && screenSpaceArray[i -1][j - 1].squareEmpty == true && screenSpaceArray[i -1][j + 1].squareEmpty == false)
            {
              // This clears this square
              myLCD.setColor(255, 255, 255);
              myLCD.fillRect(screenSpaceArray[i][j].squareX,screenSpaceArray[i][j].squareY,screenSpaceArray[i][j].squareX + squareWidth - 1,screenSpaceArray[i][j].squareY + squareHeight - 1);
              screenSpaceArray[i][j].squareEmpty = true;
              screenSpaceArray[i][j].squareColour = "White";
            }
            else if (screenSpaceArray[i -1][j].squareEmpty == true && screenSpaceArray[i -1][j - 1].squareEmpty == false && screenSpaceArray[i -1][j + 1].squareEmpty == false)
            {
              // This clears this square
              myLCD.setColor(255, 255, 255);
              myLCD.fillRect(screenSpaceArray[i][j].squareX + 1,screenSpaceArray[i][j].squareY,screenSpaceArray[i][j].squareX + squareWidth - 1,screenSpaceArray[i][j].squareY + squareHeight - 1);
              screenSpaceArray[i][j].squareEmpty = true;
              screenSpaceArray[i][j].squareColour = "White";
            }
            // If the square in the above row is empty
            else if (screenSpaceArray[i -1][j].squareEmpty == true)
            {
              // This clears this square
              myLCD.setColor(255, 255, 255);
              myLCD.fillRect(screenSpaceArray[i][j].squareX,screenSpaceArray[i][j].squareY,screenSpaceArray[i][j].squareX + squareWidth,screenSpaceArray[i][j].squareY + squareHeight - 1);
              screenSpaceArray[i][j].squareEmpty = true;
              screenSpaceArray[i][j].squareColour = "White";
            } 
         }
      }
   }
}
       

//********************************** Checks the button input **********************************
// This method checks the current stae of the four pushbuttons

void checkButtonInput()
{
  // Read the state of the buttons
  leftButtonState = digitalRead(leftButtonPin);
  rightButtonState = digitalRead(rightButtonPin);
  upButtonState = digitalRead(upButtonPin);
  downButtonState = digitalRead(downButtonPin);
  
  // Move left if the button is pressed
  if (leftButtonState == LOW) // (BROKE)
  {    
     //squareY += 5;
  } 
  // Move right if the button is pressed
  else if (rightButtonState == LOW)
  {
    moveShape(2);
  }
  // Change shape orientation if the button is pressed
  else if (upButtonState == LOW)
  {
    if (shapeOrientation == 1)
    {
      if (currentShape == "Line")
      {
         // Prevents the orientation from being switched if there is another shape in the way
         if (squareX < 120  && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row][column + 2].squareEmpty == true && screenSpaceArray[row][column + 3].squareEmpty == true &&
             screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 1][column + 3].squareEmpty == true &&
             screenSpaceArray[row + 2][column + 1].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == true && screenSpaceArray[row + 2][column + 3].squareEmpty == true &&
             screenSpaceArray[row + 3][column + 1].squareEmpty == true && screenSpaceArray[row + 3][column + 2].squareEmpty == true && screenSpaceArray[row + 3][column + 3].squareEmpty == true)
         {
            shapeOrientation = 0;
            firstChange = 0;
         } 
      }
      else if (currentShape == "Z")
      {
        if (squareY < 230 && screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == true && screenSpaceArray[row + 2][column + 1].squareEmpty == true)
        {
          shapeOrientation = 0;
          firstChange = 0;
        }
      }
      else if (currentShape == "L")
      {
        // Prevents the orientation from being switched if there is another shape in the way
         if (squareY < 200 && screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 2][column].squareEmpty == true && 
             screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 2][column + 1].squareEmpty == true && 
             screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == true)
        {
            shapeOrientation = 0;
            firstChange = 0;
         } 
      }
    }
    else if (shapeOrientation == 0)
    {
      if (currentShape == "Line")
      {
        // Prevents the orientation from being switched if there is another shape in the way
        if (squareY < 200 && screenSpaceArray[row + 1][column].squareEmpty == true && screenSpaceArray[row + 2][column].squareEmpty == true && screenSpaceArray[row + 3][column].squareEmpty == true &&
            screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 2][column + 1].squareEmpty == true && screenSpaceArray[row + 3][column + 1].squareEmpty == true &&
            screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == true && screenSpaceArray[row + 3][column + 2].squareEmpty == true &&
            screenSpaceArray[row + 1][column + 3].squareEmpty == true && screenSpaceArray[row + 2][column + 3].squareEmpty == true && screenSpaceArray[row + 3][column + 3].squareEmpty == true)
        {
          shapeOrientation = 1;
          firstChange = 0;
        } 
      }
      else if (currentShape == "Z")
      {
        if (squareX < 130 && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row][column + 2].squareEmpty == true && 
            screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == true)
        {
          shapeOrientation = 1;
          firstChange = 0;
        }
      }
      else if (currentShape == "L")
      {
        // Prevents the orientation from being switched if there is another shape in the way
        if (squareX < 130  && screenSpaceArray[row][column + 1].squareEmpty == true && screenSpaceArray[row][column + 2].squareEmpty == true && screenSpaceArray[row - 1][column + 2].squareEmpty == true && 
            screenSpaceArray[row + 1][column + 1].squareEmpty == true && screenSpaceArray[row + 1][column + 2].squareEmpty == true && screenSpaceArray[row + 2][column + 2].squareEmpty == true) 
         {
          shapeOrientation = 1;
          firstChange = 0;
        } 
      }
    }
    delay(50); 
  }
  // Move shape down faster
  else if (downButtonState == LOW) // (Temporary Left button)
  {
      moveShape(1);
  }
}

//********************************** Displays the next shape **********************************
// This method draws the next shape in the next shape display window

void displayNextShape()
{
  // Set colour black
  myLCD.setColor(0, 0, 0);
  
  // Outter rectangle
  myLCD.fillRect(192,75,270,140);
  
  // Set colour white
  myLCD.setColor(255, 255, 255);
  
  // Inner rectangle
  myLCD.fillRect(195,78,267,137);
  
   // Set colour black
  myLCD.setColor(0, 0, 0);
  
  // Store  current X and Y positions
  int prevSquareX = squareX;
  int prevSquareY = squareY;
  
  if (randNextShape == 1)
  {
    // Sets the position of the next shape display
    squareX = 220;
    squareY = 95;
  
    // Draws the next shape display
    drawBlockShape(nextColour);
  }
  else if (randNextShape == 2)
  {
    // Sets the position of the next shape display
    squareX = 210;
    squareY = 100;
    
    // Draws the next shape display
    drawLineShape(nextColour);
  }
  else if (randNextShape == 3)
  {
    // Sets the position of the next shape display
    squareX = 220;
    squareY = 94;
    
    // Draws the next shape display
    drawZShape(nextColour);
  }
  else if (randNextShape == 4)
  {
    // Sets the position of the next shape display
    squareX = 224;
    squareY = 94;
    
    // Draws the next shape display
    drawLShape(nextColour);
  }
  
  // Resets the X and Y positions to the previous value
  squareX = prevSquareX;
  squareY = prevSquareY;
}


//********************************** Checks if the game is over **********************************
// This method checks to see if the shapes have reached the top bounds of the screen.
// If the shapes reach the top bounds of the screen the game is over

void checkGameOver()
{
  if (screenSpaceArray[0][0].squareEmpty == false || screenSpaceArray[0][1].squareEmpty == false || screenSpaceArray[0][2].squareEmpty == false || screenSpaceArray[0][3].squareEmpty == false || screenSpaceArray[0][4].squareEmpty == false || 
      screenSpaceArray[0][5].squareEmpty == false || screenSpaceArray[0][6].squareEmpty == false || screenSpaceArray[0][7].squareEmpty == false || screenSpaceArray[0][8].squareEmpty == false || screenSpaceArray[0][9].squareEmpty == false || 
      screenSpaceArray[0][10].squareEmpty == false || screenSpaceArray[0][11].squareEmpty == false || screenSpaceArray[0][12].squareEmpty == false)
      {
        // Set colour black
        myLCD.setColor(0, 0, 0);
         
        // Sets the back colour black
        myLCD.setBackColor(0, 0, 0);
         
        myLCD.fillRect(0,0,319,239);
        
        // Set colour white
        myLCD.setColor(255, 255, 255);
        
        // Sets the font big
        myLCD.setFont(BigFont);
        
        // Prints the score and the number of lines 
        myLCD.print("Game Over!", 90, 60);
        
        // Sets the font small
        myLCD.setFont(SmallFont);
        
        // Converts the variables to strings
        String scoreText = String(score);
  
        // Prints the score and the number of lines 
        myLCD.print("Your final score is : " + scoreText, 70, 150);
        
        delay(9999);
      }
  }


//********************************** Draws the game text **********************************
// This method draws the game display text, such as the score and number of lines completed

void drawGameText()
{
  // Sets the font big
  myLCD.setFont(BigFont);
  
  // Sets the colour red
  myLCD.setColor(255, 0, 0);
  
  // Sets the back colour blue
  myLCD.setBackColor(120, 202, 250);
  
  // Prints the tetris logo
  myLCD.print("TETRIS", 185, 10);
  
  // Sets the colour black
  myLCD.setColor(0, 0, 0);
  
  // Sets the font small
  myLCD.setFont(SmallFont);
  
  // Converts the variables to strings
  String scoreText = String(score);
  String linesText = String(lines);
  
  // Prints the score and the number of lines 
  myLCD.print("Next Shape", 192, 60);
  myLCD.print("Score: " + scoreText, 200, 165);
  myLCD.print("Lines: " + linesText, 200, 180);
  
  // Sets the colour black
  myLCD.setColor(0, 0, 0);
}


//********************************** Draws the barriers **********************************
// This method draws the grey barriers around the game area and the blue background around the surrounding screen area

void drawBarriers()
{
  // Set colour light blue
  myLCD.setColor(120, 202, 250);
  
  // Left
  myLCD.fillRect(0,0,19,239);
  
  // Bottom
  myLCD.fillRect(0,231,200,239);
  
  // Right
  myLCD.fillRect(151,0,319,239);
  
  // Set colour grey
  myLCD.setColor(100, 100, 100);
  
  // Left barrier
  myLCD.fillRect(15,0,19,235);
  
  // Bottom barrier
  myLCD.fillRect(15,231,151,235);
  
  // Right barrier
  myLCD.fillRect(151,0,155,235);
}
