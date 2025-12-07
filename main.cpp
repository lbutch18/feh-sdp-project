#include <FEHLCD.h>
#include <FEHImages.h>
#include <FEHUtility.h>
#include <FEHKeyboard.h>
#include <FEHRandom.h>
#include <FEHSound.h>
#include <windows.h>

#define SCREEN_WIDTH 319
#define SCREEN_HEIGHT 239
#define DIFFICULTY 1.35 // Changing this affects how fast cars/buses move relative to road/coins
int PIXELS_PER_FRAME = 3;

FEHSound collision("8-bit-explosion-10-340462.wav");
FEHSound coinCollect("retro-coin-4-236671.wav");
FEHSound menuMusic("menu-music.wav");
FEHSound gameMusic("game-music.wav");
FEHSound farMusic("far-music.wav");


void drawMenu();
void drawPlay();
void drawStatistics();
void drawInstructions();
void drawCredits();
void introScreen();
void animateBetweenButton(float x, float y);
void nextGameFrame(bool reset);
void endScreen();
void drawCollision(int );
void drawCollision(int );


// Make compiler happy
class Coin;
class Car;
class Bus;
void generateNewRow(std::vector<Coin> *coins, std::vector<Car> *cars, std::vector<Bus> *buses);
void deleteOffScreenObjects(std::vector<Coin> *coins, std::vector<Car> *cars, std::vector<Bus> *buses);
void collectCoin(std::vector<Coin> *coins, int coinID);

class StatTracker {
    private:
        int distance;
        int coins;
        int score;
        int time;
        int maxDistance;
        int maxCoins;
        int maxScore;
        int maxTime;
        int startTime;
        void updateScore(){
            score = distance + 50*coins;
        }
    public:
        StatTracker(){
            distance = 0;
            coins = 0;
            score = 0;
        }
        void updateDistance(){
            distance++;
            updateScore();
        }
        int getDistance(){
            return distance;
        }
        int* getTime(){
            return &time;
        }
        int* getstartTime(){
            return &startTime;
        }
        void coinCollected(){
            coins++;
            updateScore();
        }
        int getCoins(){
            return coins;
        }
        int getScore(){
            return score;
        }
        int* getMaxDistance(){
            return &maxDistance;
        }
        int* getMaxTime(){
            return &maxTime;
        }
        int* getMaxCoins(){
            return &maxCoins;
        }
        int* getMaxScore(){
            return &maxScore;
        }
        void drawScore(){
            LCD.SetFontScale(0.5);
            LCD.WriteAt("Score: ", SCREEN_WIDTH - 80, 50);
            LCD.WriteAt(score, SCREEN_WIDTH - 42, 50);
            LCD.SetFontScale(1);
        }
        void resetStats(){
            distance = 0;
            coins = 0;
            score = 0;
            time = 0;
        }
        
};

static void updateMaxDistance(int d, int* max)
{
    if(d > *(max))
    {
        *(max) = d;
    }
          
}
static void updateMaxCoins(int c, int* max)
{
    if(c > *(max))
    {
        *(max) = c;
    }
          
}
static void updateMaxScore(int s, int* max)
{
    if(s > *(max))
    {
        *(max) = s;
    }
          
}
static void updateMaxTime(int t, int* max)
{
    if(t > *(max))
    {
        *(max) = t;
    }
          
}

//Instantiate this class globally
StatTracker trackStats = StatTracker();

class LanesDrawer{
    private:
        int laneXPositions[6];
        const int DASH_WIDTH = SCREEN_WIDTH / 10;
    public:
        LanesDrawer(){
            for (int i = 0; i < 6; i++){
                laneXPositions[i] = 2 * i * DASH_WIDTH;
            }
        }
        void drawNextLaneFrame(){
            LCD.SetFontColor(WHITE);
            
            // Solid top edge
            LCD.DrawLine(0, SCREEN_HEIGHT / 5, SCREEN_WIDTH, SCREEN_HEIGHT / 5); 

            // Top dashed lane divider
            for (int i = 0; i < 6; i++){
                LCD.DrawLine(laneXPositions[i], SCREEN_HEIGHT * 2 / 5, laneXPositions[i] + DASH_WIDTH, SCREEN_HEIGHT * 2 / 5);
            }

            // Bottom dashed lane divider
            for (int i = 0; i < 6; i++){
                LCD.DrawLine(laneXPositions[i], SCREEN_HEIGHT * 3 / 5, laneXPositions[i] + DASH_WIDTH, SCREEN_HEIGHT * 3 / 5);
            }

            // Solid bottom edge
            LCD.DrawLine(0, SCREEN_HEIGHT * 4 / 5, SCREEN_WIDTH, SCREEN_HEIGHT * 4 / 5);

            // Move lanes' x position
            for (int i = 0; i < 6; i++){
                laneXPositions[i] -= PIXELS_PER_FRAME;
                if (laneXPositions[i] <= -DASH_WIDTH){
                    laneXPositions[i] = SCREEN_WIDTH + DASH_WIDTH;
                }
            }
        }
};

class Player {
    private:
        int lane;
        int x_pos, y_pos, last_y_pos;
        int SPRITE_W = 35;
        int SPRITE_H = 35;
        int frameCount;
        FEHImage frames[8];
    public:
    Player(int inputLane)
    {
        x_pos = 5; // Start at left
        frameCount = 0;
        if (inputLane == 1){
            lane = 1;
            y_pos = 54;
            last_y_pos = y_pos;
        }
        else if (inputLane == 2){
            lane = 2;
            y_pos =  102;
            last_y_pos = y_pos;
        } 
        else if(inputLane == 3){
            lane = 3;
            y_pos = 150;
            last_y_pos = y_pos;
        }

        for (int i = 0; i < 8; i++){
            frames[i].Open(("player_run_frame_" + std::to_string(i + 1) + ".png").c_str()); // Open each frame
        }
    }
    //X Position will be top left corner of picture to draw;
    // Would be cool if we can make these smooth
    void moveUp()
    {
        if(lane == 2)
        {
            lane = 1;
            y_pos = 54;
        }
        else if(lane == 3)
        {
            lane = 2;
            y_pos = 102;
        } 
    }
    void moveDown()
    {
        if(lane == 2)
        {
            lane = 3;
            y_pos = 150;
        }
        else if(lane == 1)
        {
            lane = 2;
            y_pos = 102;
        } 
    }
    // Redraw player each frame
    void draw() {
        // Make it smooth easiest way i could think of
        if (last_y_pos < y_pos){
            last_y_pos += 16; // If we change this make sure it's a factor of 48 otherwise it'l break (gap dist)
        }else if (last_y_pos > y_pos){
            last_y_pos -= 16;
        }
        frames[frameCount].Draw(x_pos, last_y_pos);
        frameCount++;
        if (frameCount >= 8){
            frameCount = 0; // reset frame count
        }
    }
    int getLane() {
        return lane;
    }
};

class Coin {
    private:
        int lane = 1; // Default lane
        int x_pos, y_pos;
        #define COIN_RADIUS 10
        int frameCount;
        FEHImage coin1;
        FEHImage coin2;
        FEHImage coin3;
        FEHImage coin4;
    public:
    // Constructor - lane 1 is top, 2 is center, 3 is bottom
    Coin(int laneInput){
        if (laneInput == 1){
            y_pos = SCREEN_HEIGHT / 4 + COIN_RADIUS;
            lane = 1;
        }
        else if (laneInput == 2){
            y_pos = SCREEN_HEIGHT / 2;
            lane = 2;
        } 
        else if (laneInput == 3){
            y_pos = 3 * SCREEN_HEIGHT / 4 - COIN_RADIUS;
            lane = 3;
        }
        x_pos = SCREEN_WIDTH + COIN_RADIUS*2; // Start at right of screen
        frameCount = 0;

        coin1.Open("coin1.png");
        coin2.Open("coin2.png");
        coin3.Open("coin3.png");
        coin4.Open("coin4.png");
    }
    void updatePosition(){
        // Move coin left across the screen
        x_pos -= PIXELS_PER_FRAME; // Move left
    }
    // Redraw coin each frame
    void draw(){
        if (frameCount >= 0 && frameCount <= 2){
            coin1.Draw(x_pos - COIN_RADIUS, y_pos - COIN_RADIUS);
        } else if (frameCount >= 3 && frameCount <= 5){
            coin2.Draw(x_pos - COIN_RADIUS, y_pos - COIN_RADIUS);
        } else if (frameCount >= 6 && frameCount <= 8){
            coin3.Draw(x_pos - COIN_RADIUS, y_pos - COIN_RADIUS);
        } else if (frameCount >= 9 && frameCount <= 11){
            coin4.Draw(x_pos - COIN_RADIUS, y_pos - COIN_RADIUS);
            frameCount = -1;
        }
        frameCount++;
    }
    int getXPos(){
        return x_pos;
    }
    int getLane(){
        return lane;
    }
};

class scrollImage
{
    private:
        boolean top;
        FEHImage image;
        int x_pos, y_pos;
    public:
    //Will have to create starting images with x as input
    scrollImage()
    {

    }
    scrollImage(boolean top, int x)
    {
        if(top)
        {
            image.Open("topSideWalk.png");
            x_pos = x;
            y_pos = 0;
        }
        else
        {
            image.Open("bottomSidewalk.png");
            x_pos = x;
            y_pos = 192;
        }
        image.Draw(x_pos, y_pos);
    }
    int getX()
    {
        return x_pos;
    }
    void updatePosition()
    {
        x_pos = x_pos - PIXELS_PER_FRAME;
        if(x_pos < -309)
        {
            x_pos = SCREEN_WIDTH;
        }
        
    }
    void draw()
    {
        image.Draw(x_pos, y_pos);
    }
};



class Car {
    private:
        int lane;
        int x_pos, y_pos;
        FEHImage carSprite;
    public:
        static const int CAR_WIDTH = 45;
    Car(int laneInput) {
        if (laneInput == 1){
            y_pos = 59;
            lane = 1;
        } else if (laneInput == 2){
            y_pos = 107;
            lane = 2;
        } else if (laneInput == 3){
            y_pos = 155;
            lane = 3;
        }
        x_pos = SCREEN_WIDTH;

        carSprite.Open("Car.png");
    }
    void updatePosition(){
        // Move car left across the screen
        x_pos -= PIXELS_PER_FRAME * DIFFICULTY;
    }
    // Redraw car each frame
    void draw(){
        carSprite.Draw(x_pos, y_pos);
    }
    int getXPos(){
        return x_pos;
    }
    int getLane(){
        return lane;
    }
};

class Bus {
    private:
        int lane;
        int x_pos, y_pos;
        FEHImage busSprite;
    public:
    Bus(int laneInput){
        if (laneInput == 1){
            y_pos = 54;
            lane = 1;
        } else if (laneInput == 2){
            y_pos = 102;
            lane = 2;
        } else if (laneInput == 3){
            y_pos = 150;
            lane = 3;
        }
        x_pos = SCREEN_WIDTH;

        busSprite.Open("Bus.png");
    }

    void updatePosition(){
        // Move bus left across the screen
        x_pos -= PIXELS_PER_FRAME * DIFFICULTY;
    }
    // Redraw bus each frame
    void draw(){
        busSprite.Draw(x_pos, y_pos);
    }
    int getXPos(){
        return x_pos;
    }
    int getLane(){
        return lane;
    }
};

int main()
{
    // Don't print warnings
    freopen("/dev/null", "w", stdout);

    drawMenu();
}

void drawMenu() {
    menuMusic.setVolume(0.5);
    menuMusic.play();
    gameMusic.pause();
    farMusic.pause();

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
                animateBetweenButton(x_pos, y_pos);
                drawPlay();
            }
            else if (y_pos > 100 && y_pos < 100 + boxHeight) {
                boxTouched = true;
                animateBetweenButton(x_pos, y_pos);
                drawInstructions();
            }
            else if (y_pos > 150 && y_pos < 150 + boxHeight) {
                animateBetweenButton(x_pos, y_pos);
                boxTouched = true;
                drawStatistics();
            } else if (y_pos > 200 && y_pos < 200 + boxHeight) {
                animateBetweenButton(x_pos, y_pos);
                boxTouched = true;
                drawCredits();
            }
        }
    }
}

void animateBetweenButton(float x, float y)
{
    
    float centerX = x;
    float centerY = y;
    int rad = 0;
    LCD.SetFontColor(WHITE);
    while (rad < SCREEN_WIDTH){
        LCD.FillCircle(centerX, centerY, rad);
        rad += 5; // arbitrary
        Sleep(1);
    }
    LCD.SetFontColor(BLACK);
}

void drawPlay()
{
    LCD.Clear();
    trackStats.resetStats();

    introScreen();
    LCD.SetFontColor(BLACK);
    int rad = 0;
    while (rad < SCREEN_WIDTH){
        LCD.FillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, rad);
        rad += 5; // arbitrary
        Sleep(1);
    }

    float x_pos, y_pos, x_dummy, y_dummy;
    bool exit = false;
    bool reset = true; // To reset game state on first frame after entering from menu
    int FRAME_RATE = 6; // Set frame redraw time to 10 ms
    int frameCount = 0; // Need to keep track of frame count to determine when to speed up
    PIXELS_PER_FRAME = 3;
    int* startTime = trackStats.getstartTime();
    *(startTime) = TimeNow();
    
    gameMusic.setVolume(0.5);
    menuMusic.pause();
    farMusic.pause();
    gameMusic.play();
    while(!exit)
    {
        
        // Run game frames until back button is pressed
        while (!LCD.Touch(&x_pos, &y_pos)){
            nextGameFrame(reset);
            Sleep(FRAME_RATE); // Frame rate - should be faster as time goes on eventually
            reset = false;
            frameCount++;
            if (frameCount % 500 == 0){ // Increase pixels per frame every so often to increase difficulty
                PIXELS_PER_FRAME++;
            }
        }
        while (LCD.Touch(&x_dummy, &y_dummy)){
            nextGameFrame(reset);
            Sleep(FRAME_RATE);
            reset = false;
            frameCount++;
            if (frameCount % 500 == 0){ // Increase pixels per frame every so often to increase difficulty
                PIXELS_PER_FRAME++;
            }
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

    FEHImage introI;
    introI.Open("introInstructionsUpdated.png");
    introI.Draw(0,0);
    bool end = false;

    int introTime = TimeNow();
    float dummyx = 0;
    float dummyy = 0;
    bool space = false;
    bool any = false;
    while(TimeNow() - introTime < 5 && !LCD.Touch(&dummyx, &dummyy) && (!space && !any))
    {
        if(Keyboard.isPressed(KEY_SPACE))
        {
            space = true;
        }
        else if(Keyboard.areAnyPressed())
        {
            any = true;
        }
    }
    any = false;

    while(!Keyboard.isPressed(KEY_SPACE) && !end && !space)
    {

        
        float dummyx = 0;
        float dummyy = 0;
        Sleep(300);
        FEHImage first;
        first.Open("Evil Money Guy Updated2.png");
        first.Draw(0,0);
        int start1 = TimeNow();
        while(TimeNow() - start1 < 5 && !space && !any && !LCD.Touch(&dummyx, &dummyy))
        {
            if(Keyboard.isPressed(KEY_SPACE))
            {
                space = true;
                break;
            }
            else if(Keyboard.areAnyPressed())
            {
                any = true;
            }
        }
        if(space)
        {
            break;
        }
        any = false;
    

        LCD.SetFontColor(BLACK);


        Sleep(300);
        FEHImage second;
        second.Open("Planning Guy Updated.png");
        second.Draw(0,0);
        int start2 = TimeNow();
        while(TimeNow() - start2 < 5 && !space && !any && !LCD.Touch(&dummyx, &dummyy))
        {
            if(Keyboard.isPressed(KEY_SPACE))
            {
                space = true;
                break;
            }
            else if(Keyboard.areAnyPressed())
            {
                any = true;
            }
        }
        if(space)
        {
            break;
        }
        any = false;

        Sleep(300);
        
        LCD.SetFontColor(BLACK);
        FEHImage third;
        third.Open("Heist Happening Updated 1.png");
        third.Draw(0,0);
        int start3 = TimeNow();
        while(TimeNow() - start3 < 5 && !space && !any && !LCD.Touch(&dummyx, &dummyy))
        {
            if(Keyboard.isPressed(KEY_SPACE))
            {
                space = true;
                break;
            }
            else if(Keyboard.areAnyPressed())
            {
                any = true;
            }
        }
        if(space)
        {
            break;
        }

        any = false;
        Sleep(300);
        FEHImage fourth;
        fourth.Open("Final Intro Screen Updated.png");
        fourth.Draw(0,0);
        int start4 = TimeNow();
        while(TimeNow() - start4 < 5 && !space && !any && !LCD.Touch(&dummyx, &dummyy))
        {
            if(Keyboard.isPressed(KEY_SPACE))
            {
                space = true;
                break;
            }
            else if(Keyboard.areAnyPressed())
            {
                any = true;
            }
        }
        if(space)
        {
            break;
        }
        end = true;
    }
    

}

void nextGameFrame(bool reset){
    // All objects should be declared static so their locations/states are maintained
    // Reminder, the object constructors take in lane number (1 = left, 2 = center, 3 = right)
    // Count frames for object generating timing
    static int frameCount = -1;
    frameCount++;

    static std::vector<Coin> coins;
    static std::vector<Car> cars;
    static std::vector<Bus> buses;

    static Player player(2);

    static scrollImage top1;
    static scrollImage top2;
    static scrollImage bottom1;
    static scrollImage bottom2;
    int temp = 25;
    int i = 0;
    int startTime = 0;
    int* start = trackStats.getstartTime();
    if (frameCount == 0){
        top1 = scrollImage(true, 0);
        top2 = scrollImage(true, SCREEN_WIDTH);
        bottom1 = scrollImage(false, 0);
        bottom2 = scrollImage(false, SCREEN_WIDTH);
        top1.draw();
        top2.draw();
        bottom1.draw();
        bottom2.draw();

       
    }   
    // Reset game when coming back from menu
    if (reset) {
        // Recreate all objects
        frameCount = 0;
        coins.clear();
        cars.clear();
        buses.clear();
        top1 = scrollImage(true, 0);
        top2 = scrollImage(true, SCREEN_WIDTH);
        bottom1 = scrollImage(false, 0);
        bottom2 = scrollImage(false, SCREEN_WIDTH);
    }

    // Easter egg music if you go far enough
    if (frameCount == 2095){
        farMusic.setVolume(0.5);
        farMusic.play();
    }

    // Handle random generation of obstacles/coins
    // Every time a row of objects moves a car's width, generate new row
    // I don't like C :( - why can't I use the dot operator with static variables?
    if ((frameCount % (int)(Car::CAR_WIDTH / (PIXELS_PER_FRAME*DIFFICULTY))) == 0) {
        generateNewRow(&coins, &cars, &buses);
    }

    // Clear screen to get ready for redraw
    LCD.Clear();


    // Update positions
    for (int i = 0; i < coins.size(); i++) {
        coins[i].updatePosition();
    }
    for (int i = 0; i < cars.size(); i++) {
        cars[i].updatePosition();
    }
    for (int i = 0; i < buses.size(); i++) {
        buses[i].updatePosition();
    }
   

    top1.updatePosition();
    top2.updatePosition();
    bottom1.updatePosition();
    bottom2.updatePosition();

    bottom1.draw();
    bottom2.draw();
    top1.draw();
    top2.draw();
    // Add to distance every frame
    trackStats.updateDistance();
    
    /* Move when arrow keys pressed, but can can't move two frames in a row to
    prevent player from basically teleporting*/
    static bool wasUpPressed = false;
    static bool wasDownPressed = false;
    
    bool isUpPressed = Keyboard.isPressed(KEY_LEFT) || Keyboard.isPressed(KEY_UP);
    bool isDownPressed = Keyboard.isPressed(KEY_RIGHT) || Keyboard.isPressed(KEY_DOWN);
    
    // Check is key was pressed last frame as well
    if (!wasUpPressed && isUpPressed) {
        player.moveUp();
    } else if (!wasDownPressed && isDownPressed) {
        player.moveDown();
    }
    
    // Update previous presses for next frame
    wasUpPressed = isUpPressed;
    wasDownPressed = isDownPressed;

    // Clean up objects that go off screen
    deleteOffScreenObjects(&coins, &cars, &buses);

    // Redraw lanes after clearing
    static LanesDrawer lanesDrawer = LanesDrawer();
    lanesDrawer.drawNextLaneFrame();

    // Redraw all objectss
    for (int i = 0; i < coins.size(); i++) {
        coins[i].draw();
    }
    for (int i = 0; i < cars.size(); i++) {
        cars[i].draw();
    }
    for (int i = 0; i < buses.size(); i++) {
        buses[i].draw();
    }
  
    
    // Redraw player
    player.draw();

    // Redraw back button
    LCD.DrawRectangle(5, 5, 20, 20);
    LCD.WriteAt("<", 5, 5);

    // Draw score
    trackStats.drawScore();

    // Check collisions
    const int CAR_FORGIVENESS = 6;
    const int BUS_FORGIVENESS = 12;
    for (int i = 0; i < coins.size(); i++){
        // Check if coin overlaps with player
        int coinLeft = coins[i].getXPos() - 10; // 10 is coin radius
        int coinRight = coinLeft + 10;
        int playerLeft = 5; // Player x position
        int playerRight = playerLeft + 35; // Add player width
    
        // Check X overlap and same lane as coin
        if (coinRight > playerLeft && coinLeft < playerRight && player.getLane() == coins[i].getLane()){
            collectCoin(&coins, i);
        }
    }

    for (int i = 0; i < cars.size(); i++){
        // Check if car overlaps with player
        int carLeft = cars[i].getXPos() + CAR_FORGIVENESS;
        int carRight = carLeft + Car::CAR_WIDTH - 2 * CAR_FORGIVENESS;
        int playerLeft = 5; // Player x position
        int playerRight = playerLeft + 35; // Add player width
    
        // Check X overlap and same lane as car
        if (carRight > playerLeft && carLeft < playerRight && player.getLane() == cars[i].getLane()){
            drawCollision(cars[i].getLane());
            endScreen();
        }
    }

    for (int i = 0; i < buses.size(); i++){
        // Check if bus overlaps with player
        int busLeft = buses[i].getXPos() + BUS_FORGIVENESS;
        int busRight = busLeft + 90 - 2 * BUS_FORGIVENESS; // 90 is Bus width
        int playerLeft = 5; // Player x position
        int playerRight = playerLeft + 35; // Add player width
    
        // Check X overlap and same lane
        if (busRight > playerLeft && busLeft < playerRight && player.getLane() == buses[i].getLane()){
            drawCollision(buses[i].getLane());
            endScreen();
        }
    }
    int* tempTime = trackStats.getTime();
    *(tempTime) = TimeNow() - *(trackStats.getstartTime());
}

// Handle collision animations
void drawCollision(int collisionLane){
    collision.play();

    int yPos = 51;
    if (collisionLane == 2){
        yPos = 99;
    } else if (collisionLane == 3){
        yPos = 147;
    }

    FEHImage frames[8];
    for (int i = 0; i < 8; i++){
        frames[i].Open(("crash" + std::to_string(i+1) + ".png").c_str()); //c_str converts to char array
        frames[i].Draw(5, yPos); // is 5 so centered on player
        Sleep(75);
    }
}


// Need to use threading because .play() isn't fully async
DWORD WINAPI playSoundThread(LPVOID soundptr) {
    FEHSound* sound = static_cast<FEHSound*>(soundptr);
    sound->play();
    return 0;
}

void collectCoin(std::vector<Coin> *coins, int coinID){
    (*coins).erase((*coins).begin() + coinID);
    
    // Create thread to play sound without blocking
    HANDLE hThread = CreateThread(NULL, 0, playSoundThread, (LPVOID)&coinCollect, 0, NULL);
    if (hThread != NULL) {
        CloseHandle(hThread); // Release handle, thread continues
    }
    
    trackStats.coinCollected();
}

void generateNewRow(std::vector<Coin> *coins, std::vector<Car> *cars, std::vector<Bus> *buses){
    // Track if we need to skip obstacle generation this row
    static int skipObstacleRows = 0;
    static int lastCoinLane = 2;
    // Track last bus lane for coin generation
    static int lastBusLane = 0;

    // Always generate a coin
    int coinLane = (Random.RandInt()) / 10923 + 1;
    while (coinLane == lastBusLane){
        coinLane = (Random.RandInt()) / 10923 + 1;
    }
    (*coins).push_back(Coin(coinLane));
    lastCoinLane = coinLane;

    // Check if obstacle generation is skipped
    if (skipObstacleRows > 0) {
        skipObstacleRows--;
        lastBusLane = 0; // Can reset this because coin can no longer spawn on top of bus
        return;
    }

    // Generate an obstacle in a different lane than the coin
    int obstacleLane = (Random.RandInt()) / 10923 + 1;
    while (obstacleLane == coinLane) {
        obstacleLane = (Random.RandInt()) / 10923 + 1;
    }

    int obstacleType = (Random.RandInt()) / 16383;
    if (obstacleType == 0) {
        (*cars).push_back(Car(obstacleLane));
        skipObstacleRows = 1; // Skip next row
    } else {
        (*buses).push_back(Bus(obstacleLane));
        skipObstacleRows = 2; // Bus is wider, skip 2 rows
        lastBusLane = obstacleLane;
    }
}

void deleteOffScreenObjects(std::vector<Coin> *coins, std::vector<Car> *cars, std::vector<Bus> *buses){
    
    for (int i = (*coins).size() - 1; i >= 0; i--) {
        if ((*coins)[i].getXPos() < -10){
            (*coins).erase((*coins).begin() + i);
        }
    }
    for (int i = (*cars).size() - 1; i >= 0; i--) {
        if ((*cars)[i].getXPos() < -45){
            (*cars).erase((*cars).begin() + i);
        }
    }
    for (int i = (*buses).size() - 1; i >= 0; i--) {
        if ((*buses)[i].getXPos() < -90){
            (*buses).erase((*buses).begin() + i);
        }
    }
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

    
    LCD.WriteAt("Best Run", 20, 50);
    LCD.SetFontScale(0.75);
    std::string coins = "Coins Collected: ";
    coins.append(std::to_string(*(trackStats.getMaxCoins())));
    LCD.WriteAt(coins, 20, 80);
    std::string distance = "Distance traveled: ";
    distance.append(std::to_string(*(trackStats.getMaxDistance())));
    LCD.WriteAt(distance, 20, 110);
    LCD.WriteAt("Score: " + std::to_string(*(trackStats.getMaxScore())), 20, 140);
    LCD.WriteAt("Time: " + std::to_string(*(trackStats.getMaxTime())) + "s", 20, 170);
    LCD.SetFontScale(1.0);


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
    FEHImage instructions;
    instructions.Open("fixed_instructions.png");
    instructions.Draw(0,0);

    

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
    
    
     

    FEHImage credits;
    credits.Open("creditScreen.png");
    credits.Draw(0,0);
    

    
      
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
    // use c++ string fxns b/c they're better
    std::string coins = "Coins Collected: ";
    coins.append(std::to_string(trackStats.getCoins()));
    LCD.WriteAt(coins, 20, 80);
    std::string distance = "Distance traveled: " + std::to_string(trackStats.getDistance())  + "m";
    LCD.WriteAt(distance, 20, 110);
    std::string time = "Time: " + std::to_string(*(trackStats.getTime())) + "s";
    LCD.WriteAt(time, 20, 140);
    LCD.WriteAt("Final score: " + std::to_string(trackStats.getScore()), 20, 170);
    
    updateMaxTime(*(trackStats.getTime()), trackStats.getMaxTime());
    updateMaxDistance(trackStats.getDistance(), trackStats.getMaxDistance());
    updateMaxScore(trackStats.getScore(), trackStats.getMaxScore());
    updateMaxCoins(trackStats.getCoins(), trackStats.getMaxCoins());

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
         if(x_pos > 5  && x_pos < 25  && y_pos > 5  && y_pos < 25){
            exit = true;
        }
    }
    LCD.Clear();
    drawMenu();
}