#include <FEHLCD.h>
#include <FEHImages.h>
#include <FEHUtility.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void drawMenu();
void drawPlay();
void drawStatistics();
void drawInstructions();
void drawCredits();
void introScreen();
void nextGameFrame(bool reset);

class Player {

};

class Coin {
    private:
        int lane = 1; // Default lane
        int x_pos, y_pos;
        int COIN_RADIUS = 10;
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

class Obstacle {

};

int main()
{
    drawMenu();
}

void drawMenu() {
    LCD.Clear();
    int boxWidth = SCREEN_WIDTH / 1.5;
    int boxHeight = SCREEN_HEIGHT / 8;
    
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 50, boxWidth, boxHeight); // Play Button
    LCD.WriteAt("Play", SCREEN_WIDTH / 2 - boxWidth / 4, 50 + 5);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 100, boxWidth, boxHeight); // Instructions
    LCD.WriteAt("Instructions", SCREEN_WIDTH / 2 - boxWidth / 4, 100 + 5);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 150, boxWidth, boxHeight); // Statistics
    LCD.WriteAt("Statistics", SCREEN_WIDTH / 2 - boxWidth / 4, 150 + 5);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - boxWidth / 2, 200, boxWidth, boxHeight); // Credits
    LCD.WriteAt("Credits", SCREEN_WIDTH / 2 - boxWidth / 4, 200 + 5);
    
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

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    bool reset = true; // To reset game state on first frame after entering from menu
    while(!exit)
    {
        // Run game frames until back button is pressed
        while (!LCD.Touch(&x_pos, &y_pos)){
            nextGameFrame(reset);
            Sleep(50); // Frame rate
            reset = false;
        }
        while (LCD.Touch(&x_dummy, &y_dummy)){
            nextGameFrame(reset);
            Sleep(50); // Frame rate
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
    first.Open("Evil Money Guy Updated.png");
    first.Draw(0,0);

    Sleep(4.0);
    FEHImage second;
    second.Open("Planning Guy.png");
    second.Draw(0,0);

    Sleep(4.0);
    FEHImage third;
    third.Open("Heist Happening.png");
    third.Draw(0,0);

    Sleep(4.0);
    FEHImage fourth;
    fourth.Open("Final Intro Screen.png");
    fourth.Draw(0,0);

}

void nextGameFrame(bool reset){
    static Coin coin1(1);
    static Coin coin2(2);
    static Coin coin3(3);
    if (reset) {
        coin1 = Coin(1);
        coin2 = Coin(2);
        coin3 = Coin(3);
    }
    coin1.nextFrame();
    coin2.nextFrame();
    coin3.nextFrame();
}

void drawStatistics()
{
    LCD.Clear();
    LCD.WriteLine("Coins Collected: 19");
    LCD.WriteLine("Distance traveled: 0 m");
    LCD.WriteLine("Questions Answered Correctly: 0/0 (0%)");
    
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 100, 100, 50);
    LCD.WriteAt("Back", SCREEN_WIDTH / 2 - 25, 140);

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    while(!exit)
    {
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        if(x_pos > SCREEN_WIDTH / 2 - 50  && x_pos < SCREEN_WIDTH /2 + 50  && y_pos > SCREEN_HEIGHT - 100  && y_pos < SCREEN_HEIGHT - 50){
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

     LCD.WriteAt("Back", SCREEN_WIDTH / 2 - 25, 140);

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    while(!exit)
    {
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        if(x_pos > SCREEN_WIDTH / 2 - 50  && x_pos < SCREEN_WIDTH /2 + 50  && y_pos > SCREEN_HEIGHT - 100  && y_pos < SCREEN_HEIGHT - 50){
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
    
    LCD.WriteAt("Made by: Audrey Malcuit and Luke Butcher", 20, 50);
    LCD.SetFontScale(0.5);
    LCD.DrawRectangle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 100, 100, 50);
    LCD.SetFontScale(1.0);
    LCD.WriteAt("Back", SCREEN_WIDTH / 2 - 25, 140);    
    LCD.SetFontScale(1.0);

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    while(!exit)
    {
        while (!LCD.Touch(&x_pos, &y_pos));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        while (LCD.Touch(&x_dummy, &y_dummy));
        if(x_pos > SCREEN_WIDTH / 2 - 50  && x_pos < SCREEN_WIDTH /2 + 50  && y_pos > SCREEN_HEIGHT - 100  && y_pos < SCREEN_HEIGHT - 50){
            exit = true;
        }
    }
    LCD.Clear();
    drawMenu();
}
