#include <WS2811Driver.h>

//Setup 60 GRB LED Strip on Launchpad pin 3
WS2811Driver ledStrip = WS2811Driver(60, 3, NEO_GRB);

//Timer variables
unsigned long currentMillis = 0;
//Will store last time strip was updated
long previousMillis = 0;
//Interval at which to update, essentially the FPS period in ms
long interval = 30;

//Variables related to frame generation
uint8_t i, j;
uint32_t animationCount1 = 0;

//Variables for reading the GamePad input
uint8_t currentRead = 0;
uint8_t previousRead = 0;

//Flags indicating current state
uint8_t frameReady = 1;
uint8_t displayGo = 0;

//Set all LEDs in strip to blanks
void clearStrip(void)
{
    for(i=0; i< ledStrip.numPixels(); i++)
        ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
}

//Used to generate rainbow color pattern
uint32_t Wheel(byte WheelPos) {
    if(WheelPos < 85) {
        return ledStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if(WheelPos < 170) {
        WheelPos -= 85; 
        return ledStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return ledStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

void setup(void)
{
    //So we don't interfer with the GamePad, we set all IO to be inputs with pulldowns
    for (i=0; i<=20; i++)
        pinMode(i, INPUT_PULLDOWN);

    //Ensure that the Strip buffer is blank
    clearStrip();

    //Set LED strip brightness to max
    ledStrip.setBrightness(255);
    
    //Enable Strip output
    ledStrip.begin();
}

void loop()
{
    //Store previous GamePad input
    previousRead = currentRead;

    //Read current GamePad input, and pack variable
    currentRead = digitalRead(15);
    currentRead += digitalRead(14) << 1;
    currentRead += digitalRead(13) << 2;
    currentRead += digitalRead(12) << 3;
    currentRead += digitalRead(2) << 4;
    
    //Check if input has changed
    if(currentRead != previousRead) {
        //If it has changed, blank strip for new animation
        clearStrip();
        //Reset any relevant frame generation variables
        animationCount1 = 0;
        //Set frame to display at next timer interval
        frameReady = 1;
    }

    //Check if we need to update the frame
    //Each of the cases in the switch statment are responsible for generating animations, and correlate to a parallel input from the GamePad
    //When a new frame is ready to display, the frameReady flag is set
    //After the frame is displayed by the timer, the frameReady flag is cleared so the next frame can be generated
    //This if checks if the previous frame has been disaplyed, and the buffer is ready for a new one
    if(!frameReady)
        switch (currentRead) {
        case 0:
            clearStrip();
            animationCount1 = 0;
            frameReady = 1;
            break;
        case 1:
            //Step through all pixels in the buffer based on the animationCount1 buffer, and set to Red one at a time
            ledStrip.setPixelColor(animationCount1, ledStrip.Color(255, 0, 0));
            //Increment animationCount1 to be used in the next frame generation sequence
            animationCount1++;
            //This animation ends at the end of the strip, so we reset at this point
            if(animationCount1 == ledStrip.numPixels() + 1) {
                animationCount1 = 0;
                clearStrip();
            }
            //Let the application know there is a frame ready to be displayed
            frameReady = 1;
            break;
        case 2:
            ledStrip.setPixelColor(animationCount1, ledStrip.Color(0, 255, 0));
            animationCount1++;
            if(animationCount1 == ledStrip.numPixels() + 1) {
                animationCount1 = 0;
                clearStrip();
            }
            frameReady = 1;
            break;
        case 3:
            //This animation runs a rainbow cycle across the whole strip
            for(i=0; i< ledStrip.numPixels(); i++)
                ledStrip.setPixelColor(i, Wheel(((i * 256 / ledStrip.numPixels()) + animationCount1) & 255));
            animationCount1++;
            if(animationCount1 == 256)
                animationCount1 = 0;
            frameReady = 1;
            break;
        //Additional frame sequences can be added here
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        //Attach a switch to Pin2 on the LP to enable an offline state, this is just a blue wipe animation
        case 16:
            ledStrip.setPixelColor(animationCount1, ledStrip.Color(0, 0, 255));
            animationCount1++;
            if(animationCount1 == ledStrip.numPixels() + 1) {
                animationCount1 = 0;
                clearStrip();
            }
            frameReady = 1;
            break;
        default:
            break;
        }

    //If there is a new frame in the buffer, and the timer has told us it's time to update the strip
    if(frameReady && displayGo) {
        //Display the latest frame
        ledStrip.show();
        //Reset frameReady flag so that the program will generate the next one
        frameReady = 0;
        //Reset the displayGo flag so that the timer can set it high and we don't prematurely update the strip
        displayGo = 0;
    }

    //Check if it's time to update the LED strip
    currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
        // save the last time the strip was updated
        previousMillis = currentMillis;
        //Let the program know it's time to update the strip
        displayGo = 1;
    }

}
