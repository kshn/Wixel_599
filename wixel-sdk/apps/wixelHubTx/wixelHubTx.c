/** send sync pulse app:

This application tests the random number generator.

It makes the yellow LED blink randomly.

You can also connect to it using the virtual COM port and send commands to it.
Each command is a single character:
'r' : Generate and random number and print it.
's' : Seed RNG from serial number (discards previous state of RNG).
'a' : Seed RNG from ADC (discards previous state of RNG).
'0' : Seed RNG with 0x0000.
'1' : Seed RNG with 0xFFFF.
'8' : Seed RNG with 0x8003.
'Y' : Start blinking the yellow LED randomly.  (Every other command disables
      the yellow LED's blinking so it doesn't interfere with the sequence of
      numbers.)
 */

#include <cc2511_map.h>
#include <wixel.h>
#include <usb.h>
#include <usb_com.h>
#include <stdio.h>
#include <random.h>

uint32 nextToggle = 0;

BIT blinkYellow = 1;

void updateLeds()
{
    usbShowStatusWithGreenLed();

    // NOTE: The code below is bad because it is reading two bytes of timeMs,
    // and the interrupt that updates timeMs could fire between those two reads.
    if (blinkYellow)
    {
        uint32 time = getMs();
        if (time >= nextToggle)
        {
            LED_YELLOW_TOGGLE();
            LED_RED_TOGGLE();
            LED_GREEN_TOGGLE();
            nextToggle = time + randomNumber();
        }
    }
    else
    {
        LED_YELLOW(1);
    }
}


void main()
{
    systemInit();

    usbInit();
    randomSeedFromSerialNumber();
    while(1)
    {
        boardService();
        updateLeds();
        usbComService();
        //receiveCommands();
    }
}
