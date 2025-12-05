#include <FEHLCD.h>
#include <FEHImages.h>
#include <FEHUtility.h>
#include <FEHKeyboard.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void drawMenu();
void drawPlay();
void drawStatistics();
void drawInstructions();
void drawCredits();
void introScreen();
void nextGameFrame(bool reset);
void userInput();
void endScreen();

//Generative AI was used to create images throughout the game - for more thorough accreditation, please see our website

/*
Audrey Malcuit
Creation of the player class
Declaration of instance variables and instantiation of coordinates and initial appearance on screen according to input of lane
This class creates the player and also creates two functions (moveUp and moveDown) for player movement between lanes which will be called in response to user input
*/
class Player {
    private:
        int lane;
        int x_pos, y_pos;
        int PLAYER_RADIUS = 10;
        FEHImage image;
        int SPRITE_W = 40;
        int SPRITE_H = 40;
    public:
    Player(int inputLane)
    {
        image.Open("player updated.png");
        x_pos = 5; // Start at left
        if (inputLane == 1){
            lane = 1;
            y_pos = 50;
            image.Draw(x_pos, y_pos);
        }
        else if (inputLane == 2){
            lane = 2;
            y_pos =  98;
            image.Draw(x_pos, y_pos);
        } 
        else if(inputLane == 3){
            lane = 3;
            y_pos = 146;
            image.Draw(x_pos, y_pos);
        }
    }
    //X Position will be top left corner of picture to draw;
    // Would be cool if we can make these smooth

    void moveUp()
    {
        LCD.SetFontColor(BLACK);
        int old_y = y_pos;
      
        if(lane == 2)
        {
            lane = 1;
            y_pos = 50;

            // Erase old sprite then draw at new position
            LCD.FillRectangle(x_pos, old_y, SPRITE_W, SPRITE_H);
            image.Draw(x_pos, y_pos);
        }
        else if(lane == 3)
        {
            lane = 2;
            y_pos = 98;

            // Erase old sprite then draw at new position
            LCD.FillRectangle(x_pos, old_y, SPRITE_W, SPRITE_H);
            image.Draw(x_pos, y_pos);
        } 

        
    }
    void moveDown()
    {
        LCD.SetFontColor(BLACK);
        int old_y = y_pos;

        if(lane == 2)
        {
            lane = 3;
            y_pos = 146;
            // Erase old sprite then draw at new position
            LCD.FillRectangle(x_pos, old_y, SPRITE_W, SPRITE_H);
            image.Draw(x_pos, y_pos);
        }
        else if(lane == 1)
        {
            lane = 2;
            y_pos = 98;
            // Erase old sprite then draw at new position
            LCD.FillRectangle(x_pos, old_y, SPRITE_W, SPRITE_H);
            image.Draw(x_pos, y_pos);
        } 
    }
};

/*
Luke Butcher
Creation of Coin class - radius of coin for dimension and plotting purposes, coordinates to draw and lane for logic with player and game in general
Constructor: depends on the lane input and then will update coordinates accordingly
nextFrame: function to move the coin across the screen
*/
class Coin {
    private:
        int lane = 1; // Default lane
        int x_pos, y_pos;
        #define COIN_RADIUS 10
    public:
    // Constructor - lane 1 is top, 2 is center, 3 is bottom
    Coin(int lane){
        if (lane == 1){
            y_pos = SCREEN_HEIGHT / 4 + COIN_RADIUS;
        }
        else if (lane == 2){
            y_pos = SCREEN_HEIGHT / 2;
        } 
        else if(lane == 3){
            y_pos = 3 * SCREEN_HEIGHT / 4 - COIN_RADIUS;
        }
        x_pos = 320; // Start at right of screen

        // Until we get graphics, represent coin as a circle
        LCD.SetFontColor(YELLOW);
        LCD.FillCircle(x_pos, y_pos, COIN_RADIUS);
        LCD.SetFontColor(WHITE);
    }
    void nextFrame(){
        // Move coin left across the screen
        LCD.SetFontColor(BLACK);
        LCD.FillCircle(x_pos, y_pos, COIN_RADIUS); // Erase old position
        x_pos -= 5; // Move left 5 pixels
        LCD.SetFontColor(YELLOW);
        LCD.FillCircle(x_pos, y_pos, COIN_RADIUS); // Draw at new position
        LCD.SetFontColor(WHITE);
    }
};

/*
Audrey Malcuit
Creation of scrollImage class to make the setting seem as if it was moving at the top and bottom of the screen
Boolean to determine if it is at the top or bottom (for coordinate purposes), the actual image, and the coordinates
nextFrame: function to move the images across the screen
*/
class scrollImage
{
    private:
        boolean top;
        FEHImage image;
        int x_pos, y_pos;
    public:
    //Will have to create starting images with x as input

    //Empty constructor for array
    scrollImage()
    {

    }
    scrollImage(boolean top, int x)
    {
        if(top)
        {
            image.Open("topSideWalk.png");
            x_pos = x;
            y_pos = 0; //top y coordinate
        }
        else
        {
            image.Open("bottomSidewalk.png"); //inverted image of top
            x_pos = x;
            y_pos = 192; //bottom y coordinate
        }
        image.Draw(x_pos, y_pos);
    }
    void nextFrame()
    {
        x_pos = x_pos - 3; //Move left 3 units
        if(x_pos < 0)
        {
            x_pos = 320; //Don't draw past 0
        }
        image.Draw(x_pos, y_pos);


    }
};

/*
Luke Butcher
Creation of car class: lane, coordinates, and actual image
Constructor: update coordinates and placement of image according to input of lane
nextFrame: move the car across the screen
*/
class Car {
    private:
        int lane;
        int x_pos, y_pos;
        FEHImage carSprite;
    public:
    Car(int lane){
        if (lane == 1){
            y_pos = SCREEN_WIDTH / 5;
        } else if (lane == 2){
            y_pos = SCREEN_WIDTH / 5 + 42;
        } else if (lane == 3){
            y_pos = SCREEN_WIDTH / 5 + 92;
        }
        x_pos = 320;

        carSprite.Open("Car.png");
        carSprite.Draw(x_pos, y_pos);
    }
    void nextFrame(){
        // Move car left across the screen
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x_pos, y_pos, 60 , 30); // Erase old car
        LCD.SetFontColor(WHITE);

        x_pos -= 5;
        carSprite.Draw(x_pos, y_pos);
    }
};

class Bus {
    private:
        int lane;
        int x_pos, y_pos;
        FEHImage busSprite;
    public:
    Bus(int lane){
        if (lane == 1){
            y_pos = SCREEN_WIDTH / 5;
        } else if (lane == 2){
            y_pos = SCREEN_WIDTH / 5 + 42;
        } else if (lane == 3){
            y_pos = SCREEN_WIDTH / 5 + 92;
        }
        x_pos = 320;

        busSprite.Open("Bus.png");
        busSprite.Draw(x_pos, y_pos);
    }
    void nextFrame(){
        // Move bus left across the screen
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x_pos, y_pos, 80, 30); // Erase old bus
        LCD.SetFontColor(WHITE);

        x_pos -= 5;
        busSprite.Draw(x_pos, y_pos);
    }
};

int main()
{
    drawMenu();
}

/*
Audrey Malcuit and Luke Butcher
Displays the menu screen and allows for buttons to be clicked and reroutes users to those screens
*/
void drawMenu() {
    LCD.Clear(); //Clear screen
    //Constants for boxes
    int boxWidth = SCREEN_WIDTH / 1.5;
    int boxHeight = SCREEN_HEIGHT / 8;
    FEHImage menu;
    menu.Open("Menu Screen.png"); //Menu background image
    menu.Draw(0,0);
    

    //Create boxes of same dimensions, alter y coordinates throughout
    LCD.SetFontColor(BLACK);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 50, boxWidth, boxHeight); // Play Button
    LCD.FillRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 50, boxWidth, boxHeight);\
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Play", 130, 50 + 5);
    LCD.SetFontColor(BLACK);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 100, boxWidth, boxHeight); // Instructions
    LCD.FillRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 100, boxWidth, boxHeight);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Instructions", 100, 100 + 5);
    LCD.SetFontColor(BLACK);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 150, boxWidth, boxHeight); // Statistics
    LCD.FillRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 150, boxWidth, boxHeight);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Statistics", SCREEN_WIDTH / 2 - boxWidth / 4, 150 + 5);
    LCD.SetFontColor(BLACK);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 200, boxWidth, boxHeight); // Credits
    LCD.FillRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 200, boxWidth, boxHeight);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Credits", 120, 200 + 5);
    
    LCD.Update();

    float x_pos, y_pos, x_dummy, y_dummy;
    bool boxTouched = false;

    //Loop until a button is pressed then respond to input and call respective function
    while (!boxTouched){
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        if (x_pos > SCREEN_WIDTH / 2 - boxWidth / 2 && x_pos < SCREEN_WIDTH / 2 + boxWidth / 2) {
            if (y_pos > 50 && y_pos < 50 + boxHeight) {
                boxTouched = true;
                drawPlay();
            }
            else if (y_pos > 100 && y_pos < 100 + boxHeight) {
                boxTouched = true;
                drawInstructions();
            }
            else if (y_pos > 150 && y_pos < 150 + boxHeight) {
                boxTouched = true;
                drawStatistics();
            } else if (y_pos > 200 && y_pos < 200 + boxHeight) {
                boxTouched = true;
                drawCredits();
            }
        }
    }
}

/*
Audrey Malcuit and Luke Butcher
Finish this comment later
*/
void drawPlay()
{
    LCD.Clear();

    // Draw back button
    LCD.DrawRectangle(5, 5, 20, 20);
    LCD.WriteAt("<", 5, 5);

    // Draw lanes - TODO: Replace with dashed lines
    LCD.DrawLine(0, SCREEN_HEIGHT / 5, SCREEN_WIDTH, SCREEN_HEIGHT / 5);
    LCD.DrawLine(0, SCREEN_HEIGHT * 2 / 5, SCREEN_WIDTH, SCREEN_HEIGHT * 2 / 5);
    LCD.DrawLine(0, SCREEN_HEIGHT * 3 / 5, SCREEN_WIDTH, SCREEN_HEIGHT * 3 / 5);
    LCD.DrawLine(0, SCREEN_HEIGHT * 4 / 5, SCREEN_WIDTH, SCREEN_HEIGHT * 4 / 5);

    //TO DO: Intro screens

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    bool reset = true; // To reset game state on first frame after entering from menu
  
    while(!exit)
    {
        // Run game frames until back button is pressed
        while (!LCD.Touch(&x_pos, &y_pos)){
            nextGameFrame(reset);
            Sleep(100); // Frame rate - should be faster as time goes on eventually
            reset = false;
        }
        while (LCD.Touch(&x_dummy, &y_dummy)){
            nextGameFrame(reset);
            Sleep(100); // Frame rate - should be faster as time goes on eventually
            reset = false;
        }
        
        if(x_pos > 5  && x_pos < 25  && y_pos > 5  && y_pos < 25){
            exit = true;
        }
    }
    LCD.Clear();
    drawMenu();
}

/*
Audrey Malcuit
Creation of AI images to display a beginning storyline to set up user for gameplay
*/
void introScreen()
{
    //Cycle through storyline screens
    LCD.Clear();
    FEHImage first;
    first.Open("Evil Money Guy Updated2.png");
    first.Draw(0,0);

    Sleep(2.5);
    FEHImage second;
    second.Open("Planning Guy Updated.png");
    second.Draw(0,0);

    Sleep(2.5);
    FEHImage third;
    third.Open("Heist Happening Updated 1.png");
    third.Draw(0,0);

    Sleep(2.5);
    FEHImage fourth;
    fourth.Open("Final Intro Screen Updated.png");
    fourth.Draw(0,0);

}

void nextGameFrame(bool reset){
    // All objects should be declared static so their locations/states are maintained
    // Should probably implement with object arrays
    /* This is for TESTING right now, eventually will need to randomly generate coins and cars, check collisions, delete off-screen objects, etc.*/
    // Reminder, the object constructors take in lane number (1 = left, 2 = center, 3 = right)
    static Coin coin1(1);
    static Coin coin2(2);
    static Coin coin3(3);

    static Car car1(1);
    static Car car2(2);
    static Car car3(3);
    
    static Bus bus1(3);
    static Bus bus2(3);
    static Bus bus3(3);

    static Player player(2);

    // reset game when coming back from menu - this needs updated
    if (reset) {
        coin1 = Coin(1);
        coin2 = Coin(2);
        coin3 = Coin(3);
        car1 = Car(1);
        car2 = Car(2);
        bus1 = Bus(3);
        player = Player(2);
    }

    // Move all objects down/next animation frame
    coin1.nextFrame();
    coin2.nextFrame();
    coin3.nextFrame();
    car1.nextFrame();
    car2.nextFrame();
    car3.nextFrame();
    bus1.nextFrame();
    bus2.nextFrame();
    bus3.nextFrame();
    
    // Move when arrow keys pressed
    if (Keyboard.isPressed(KEY_LEFT) || Keyboard.isPressed(KEY_UP)){
        player.moveUp();
    } else if (Keyboard.isPressed(KEY_RIGHT) || Keyboard.isPressed(KEY_DOWN)){
        player.moveDown();
    }

    // TODO: Delete objects that go off screen and create new ones at top (prob do this in class methods)
}

/*
Audrey Malcuit and Luke Butcher
Update this once statistic tracking is figured out
*/
void drawStatistics()
{
    LCD.Clear();
    
    FEHImage menu;
    menu.Open("Menu Screen.png");
    menu.Draw(0,0);
    //Print out stats
    LCD.SetFontColor(BLACK);
    LCD.DrawRectangle(10, 40, 300, 220);
    LCD.FillRectangle(10, 40, 300, 220);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt( "Coins Collected: 19", 20, 50);
    LCD.WriteAt("Distance traveled: 0 m", 20, 100);
    LCD.WriteAt("Questions Answered", 20, 150);
    LCD.WriteAt("Correctly: 0/0 (0%)", 20, 200);
    
    

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;

    //Draw back button
    LCD.DrawRectangle(5, 5, 20, 20);
    LCD.WriteAt("<", 5, 5);
    while(!exit)
    {
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        if(x_pos > 5  && x_pos < 25  && y_pos > 5  && y_pos < 25){
            exit = true;
        }
        
    }
    LCD.Clear();
    drawMenu();
}

/*
Audrey Malcuit and Luke Butcher
Displays instructions screen and allows user to navigate back to menu screen
*/
void drawInstructions()
{
    LCD.Clear();
    FEHImage instructions;
    instructions.Open("fixed_instructions.png"); //Instructions picture
    instructions.Draw(0,0);

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;

    //Draw back button
    LCD.DrawRectangle(5, 5, 20, 20);
    LCD.WriteAt("<", 5, 5);

    //Loop until button is pressed then navigate back to menu
    while(!exit)
    {
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        if(x_pos > 5  && x_pos < 25  && y_pos > 5  && y_pos < 25){
            exit = true;
        }
    }
    LCD.Clear();
    LCD.SetFontScale(1.);
    drawMenu();

    
}

void drawCredits()
{
    LCD.Clear();
    //Fix this - tested intro screens
    
    //LCD.SetFontScale(0.5);
    //LCD.WriteAt("Made by: Audrey Malcuit and Luke Butcher", 20, 50);
   


    
      
    LCD.SetFontScale(1.0);
    
    //Fix this - tested intro screens
   
    //LCD.SetFontScale(0.5);
   
    //This is test code for the scrolling image 
    
    scrollImage top[12];
    scrollImage bottom[12];
    int temp = 25;
    int i = 0;
    for(int i = 0; i < 12; i++)
    {
            top[i] = scrollImage(true, temp);
            bottom[i] = scrollImage(false, temp);
            temp = temp + 25;
    }
    while(i < 1000)
    {
       


        for(int i = 0; i < 12; i++)
        {
            top[i].nextFrame();
            bottom[i].nextFrame();
        }
       
        LCD.Update();
        i++;
    }
     

    
    FEHImage credits;
    credits.Open("creditScreen.png");
    credits.Draw(0,0);
    
 
    LCD.SetFontColor(WHITE);  
    LCD.DrawRectangle(5, 5, 20, 20);
    LCD.WriteAt("<", 5, 5);
    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    while(!exit)
    {
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
         if(x_pos > 5  && x_pos < 25  && y_pos > 5  && y_pos < 25){
            exit = true;
        }
    }
    LCD.Clear();
    drawMenu();
}



//parameter of stats
/*
Audrey Malcuit
finish this comment later once stats are figured out
*/
void endScreen()
{
    LCD.Clear();
    FEHImage end;
    end.Open("Menu Screen.png");
    end.Draw(0,0);
    LCD.SetFontColor(BLACK);
    LCD.DrawRectangle(10, 40, 300, 220);
    LCD.FillRectangle(10, 40, 300, 220);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Game Over!", 20, 50);
    LCD.WriteAt( "Coins Collected: 19", 20, 80);
    LCD.WriteAt("Distance traveled: 0 m", 20, 110);
    LCD.WriteAt("Questions Answered", 20, 140);
    LCD.WriteAt("Correctly: 0/0 (0%)", 20, 170);
    
    LCD.SetFontColor(WHITE);  
    LCD.DrawRectangle(5, 5, 20, 20);
    LCD.WriteAt("<", 5, 5);
    LCD.Update();
    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    while(!exit)
    {
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
         if(x_pos > 5  && x_pos < 25  && y_pos > 5  && y_pos < 25){
            exit = true;
        }
    }
    LCD.Clear();
    drawMenu();

}