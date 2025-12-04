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

class Player {
    private:
        int lane;
        int x_pos, y_pos;
        int PLAYER_RADIUS = 10;
        FEHImage image;
    public:
    Player(int inputLane)
    {
        if (inputLane == 1){
            lane = 1;
            x_pos = 71;
            image.Open("player.png");
            image.Draw(71, 190);
        }
        else if (inputLane == 2){
            lane = 2;
            x_pos =  135;
            image.Open("player.png");
            image.Draw(135, 190);
        } 
        else if(inputLane == 3){
            lane = 3;
            x_pos = 199;
            image.Open("player.png");
            image.Draw(199, 190);
        }
        y_pos = 300; // Start at bottom of screen
    }
    //X Position will be top left corner of picture to draw;
    // Would be cool if we can make these smooth
    void moveRight()
    {
        LCD.SetFontColor(BLACK);
      
        if(lane == 2)
        {
            lane = lane + 1;
            LCD.DrawRectangle(135, 190, 50, 50);
            LCD.FillRectangle(135, 190, 50, 50);
            x_pos = 199;
            FEHImage image;
            image.Open("player.png");
            image.Draw(x_pos, 190);
        }
        else if(lane == 1)
        {
            lane = lane + 1;
            LCD.DrawRectangle(71, 190, 50, 50);
            LCD.FillRectangle(71, 190, 50, 50);
            x_pos = 135;
            FEHImage image;
            image.Open("player.png");
            image.Draw(135, 190);
            
        }
    }
    void moveLeft()
    {
        LCD.SetFontColor(BLACK);
        if(lane == 2)
        {
            lane = lane - 1;
            x_pos = 71;
            FEHImage image;
            image.Open("player.png");
            image.Draw(x_pos, 190);
            LCD.DrawRectangle(135, 190, 50, 50);
            LCD.FillRectangle(135, 190, 50, 50);
            
        }
        else if(lane == 3)
        {
            lane = lane - 1;
            x_pos = 135;
            FEHImage image;
            image.Open("player.png");
            image.Draw(x_pos, 190);
            LCD.DrawRectangle(199, 190, 50, 50);
            LCD.FillRectangle(199, 190, 50, 50);
        }
    }
};

class Coin {
    private:
        int lane = 1; // Default lane
        int x_pos, y_pos;
        #define COIN_RADIUS 10
    public:
    // Constructor - lane 1 is left, 2 is center, 3 is left
    Coin(int lane){
        if (lane == 1){
            x_pos = SCREEN_WIDTH / 4 + COIN_RADIUS + COIN_RADIUS/2;
        }
        else if (lane == 2){
            x_pos = SCREEN_WIDTH / 2;
        } 
        else if(lane == 3){
            x_pos = 3 * SCREEN_WIDTH / 4 - COIN_RADIUS - COIN_RADIUS/2;
        }
        y_pos = 0; // Start at top of screen

        // Until we get graphics, represent coin as a circle
        LCD.SetFontColor(YELLOW);
        LCD.FillCircle(x_pos, y_pos, COIN_RADIUS);
        LCD.SetFontColor(WHITE);
    }
    void nextFrame(){
        // Move coin down the screen
        LCD.SetFontColor(BLACK);
        LCD.FillCircle(x_pos, y_pos, COIN_RADIUS); // Erase old position
        y_pos += 5; // Move down 5 pixels
        LCD.SetFontColor(YELLOW);
        LCD.FillCircle(x_pos, y_pos, COIN_RADIUS); // Draw at new position
        LCD.SetFontColor(WHITE);
    }
};

class Car {
    private:
        int lane;
        int x_pos, y_pos;
        FEHImage carSprite;
    public:
    Car(int lane){
        if (lane == 1){
            x_pos = SCREEN_WIDTH / 4 + 4;
        } else if (lane == 2){
            x_pos = SCREEN_WIDTH / 2 - 12;
        } else if (lane == 3){
            x_pos = 3 * SCREEN_WIDTH / 4 - 26;
        }
        y_pos = 0;

        carSprite.Open("Car.png");
        carSprite.Draw(x_pos, y_pos);
    }
    void nextFrame(){
        // Move car down the screen
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x_pos, y_pos, 30 , 60); // Erase old car
        LCD.SetFontColor(WHITE);

        y_pos += 5;
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
            x_pos = SCREEN_WIDTH / 4 + 4;
        } else if (lane == 2){
            x_pos = SCREEN_WIDTH / 2 - 12;
        } else if (lane == 3){
            x_pos = 3 * SCREEN_WIDTH / 4 - 26;
        }
        y_pos = 0;

        busSprite.Open("Bus.png");
        busSprite.Draw(x_pos, y_pos);
    }
    void nextFrame(){
        // Move car down the screen
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(x_pos, y_pos, 30 , 60); // Erase old bus
        LCD.SetFontColor(WHITE);

        y_pos += 5;
        busSprite.Draw(x_pos, y_pos);
    }
};

int main()
{
    drawMenu();
}

void drawMenu() {
    LCD.Clear();
    int boxWidth = SCREEN_WIDTH / 1.5;
    int boxHeight = SCREEN_HEIGHT / 8;
    FEHImage menu;
    menu.Open("Menu Screen.png");
    menu.Draw(0,0);
    
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

void drawPlay()
{
    LCD.Clear();

    // Draw back button
    LCD.DrawRectangle(5, 5, 20, 20);
    LCD.WriteAt("<", 5, 5);

    // Draw lanes - TODO: Replace with dashed lines
    LCD.DrawLine(SCREEN_WIDTH / 5, 0, SCREEN_WIDTH / 5, SCREEN_HEIGHT);
    LCD.DrawLine(SCREEN_WIDTH * 2 / 5, 0, SCREEN_WIDTH * 2 / 5, SCREEN_HEIGHT);
    LCD.DrawLine(SCREEN_WIDTH * 3 / 5, 0, SCREEN_WIDTH * 3 / 5, SCREEN_HEIGHT);
    LCD.DrawLine(SCREEN_WIDTH * 4 / 5, 0, SCREEN_WIDTH * 4 / 5, SCREEN_HEIGHT);

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


void introScreen()
{
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
    
    static Bus bus1(3);

    static Player player(2);

    // reset game when coming back from menu
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
    bus1.nextFrame();
    
    // Move when arrow keys pressed
    if (Keyboard.isPressed(KEY_LEFT)){
        player.moveLeft();
    } else if (Keyboard.isPressed(KEY_RIGHT)){
        player.moveRight();
    }

    // TODO: Delete objects that go off screen and create new ones at top (prob do this in class methods)
}

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

void drawInstructions()
{
    LCD.Clear();
    LCD.SetFontScale(0.5);
    LCD.WriteAt("Instructions", 20, 30);
    LCD.WriteAt("1. Watch the storyline to see how you come to be a", 20, 60);
    LCD.WriteAt("hero and in the middle of a heist!", 20, 90);
    LCD.WriteAt("2. Begin the chase once the officer catche", 20, 120);
    LCD.WriteAt("to be a hero and in the middle of a heist!", 20, 150);     
    LCD.WriteAt("3. Use your keyboard to navigate around obstacles" , 20, 180);

    

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



