#include <FEHLCD.h>
#include <FEHImages.h>
#include <FEHUtility.h>
#include <FEHKeyboard.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define PIXELS_PER_FRAME 5

void drawMenu();
void drawPlay();
void drawStatistics();
void drawInstructions();
void drawCredits();
void introScreen();
void nextGameFrame(bool reset);
void endScreen();

// Make compiler happy
class Coin;
class Car;
class Bus;
void generateNewRow(std::vector<Coin> *coins, std::vector<Car> *cars, std::vector<Bus> *buses);

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
        x_pos -= PIXELS_PER_FRAME; // Move left
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
        static const int CAR_WIDTH = 45;
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
        LCD.FillRectangle(x_pos, y_pos, CAR_WIDTH , 30); // Erase old car
        LCD.SetFontColor(WHITE);

        x_pos -= PIXELS_PER_FRAME;
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

        x_pos -= PIXELS_PER_FRAME;
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
    // Reminder, the object constructors take in lane number (1 = left, 2 = center, 3 = right)
    static std::vector<Coin> coins;
    static std::vector<Car> cars;
    static std::vector<Bus> buses;

    static Player player(2);

    // Count frames for object generating timing
    static int frameCount = -1;
    frameCount++;

    // Reset game when coming back from menu - this needs updated
    if (reset) {
        // Recreate all objects
        frameCount = 0;
        coins.clear();
        cars.clear();
        buses.clear();
        player = Player(2);
    }

    // Handle random generation of obstacles/coins
    // 5 is how many pixels per frame objects move - every time a row of objects moves a car's width, generate new row
    // I don't like C :( - why can't I use the dot operator with static variables?
    if ((frameCount % (Car::CAR_WIDTH / PIXELS_PER_FRAME)) == 0) {
        generateNewRow(&coins, &cars, &buses);
    }

    // Move all objects down/next animation frame
    for (int i = 0; i < coins.size(); i++) {
        coins[i].nextFrame();
    }
    for (int i = 0; i < cars.size(); i++) {
        cars[i].nextFrame();
    }
    for (int i = 0; i < buses.size(); i++) {
        buses[i].nextFrame();
    }
    
    // Move when arrow keys pressed
    if (Keyboard.isPressed(KEY_LEFT) || Keyboard.isPressed(KEY_UP)){
        player.moveUp();
    } else if (Keyboard.isPressed(KEY_RIGHT) || Keyboard.isPressed(KEY_DOWN)){
        player.moveDown();
    }

    // TODO: Delete objects that go off screen and create new ones at top (prob do this with class methods?)
}

void generateNewRow(std::vector<Coin> *coins, std::vector<Car> *cars, std::vector<Bus> *buses){
    (*coins).push_back(Coin(1));
    (*cars).push_back(Car(2));
    (*buses).push_back(Bus(3));
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
    LCD.WriteAt("hero in the middle of a heist!", 20, 90);
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