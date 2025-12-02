#include "FEHLCD.h"

int main()
{
    LCD.WriteLine("Hello World!");
    while (1) {
        LCD.Update();
        // Never end
    }
    return 0;
}