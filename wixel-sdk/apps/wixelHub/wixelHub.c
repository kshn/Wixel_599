/* example_usb_com app:

This example app shows how to process and respond to commands received from
a USB virtual COM port.  The command protocol is documented below.


== Serial Command Protocol ==

Command Name: Toggle Yellow LED
Protocol:
  Computer sends 0x81 OR the ASCII encoding of character 'y' (0x79).

Command Name: Get X
Protocol:
  Computer sends 0x82
  Wixel sends back a 2-byte response that contains the current value X as a
  2-byte little-endian integer.

Command Name: Set X
Protocol:
  Computer sends 0x83, low7Bit, upper7Bits

Command Name: Clear Error
Protocol:
  Computer sends 0x84.

Command Name: Get Time
Protocol:
  Computer sends the ASCII encoding of the character 't' (0x74).
  Wixel sends back an ASCII string that includes the number of
  milliseconds that the Wixel has been running for.
*/

#include <wixel.h>
#include <usb.h>
#include <usb_com.h>
#include <usb_hid_constants.h>
#include <radio_queue.h>
#include <stdio.h>

#define COMMAND_TOGGLE_YELLOW_LED   0x81
#define COMMAND_CLEAR_ERROR         0x84

/* VARIABLES ******************************************************************/

/** True if the yellow LED should currently be on. */
BIT yellowLedOn = 0;

/** True if a serial protocol error occurred recently. */
BIT serialProtocolError = 0;

/** The binary command byte received from the computer. */
uint8 commandByte;

/** The binary data bytes received from the computer. */
uint8 dataBytes[32];

/** The number of data bytes we are waiting for to complete the current command.
 * If this is zero, it means we are not currently in the middle of processing
 * a binary command. */
uint8 dataBytesLeft = 0;

/** The number of data bytes received so far for the current binary command.
 * If dataBytesLeft==0, this is undefined. */
uint8 dataBytesReceived;

/** A temporary buffer used for composing responses to the computer before
 * they are sent.  Must be bigger than the longest possible response to any
 * command.
 */
uint8 XDATA response[32];

/** The sequence counter that tracks what the last packet ID was. */
uint16 seqCount = 0;
uint8 cSendingOn = 0;
uint8 cSendingCount = 0;

/* FUNCTIONS ******************************************************************/

void updateLeds()
{
    usbShowStatusWithGreenLed();
    LED_YELLOW(yellowLedOn);
    LED_RED(serialProtocolError);
}

void executeCommand()
{
    switch(commandByte)
    {
    case COMMAND_TOGGLE_YELLOW_LED:
        yellowLedOn ^= 1;
        break;

    case COMMAND_CLEAR_ERROR:
        serialProtocolError = 0;
        break;
    }
}


int sendPacket()
{
	uint8 XDATA * txBuf;
	uint32 time;
	//uint8 responseLength;
	if(txBuf = radioQueueTxCurrentPacket())
	{
		txBuf[0] = 6;						// PACKET LENGTH IN BYTES!!!
		// Sequence Number
		txBuf[1] = seqCount & 0xff;			// Right Side
		txBuf[2] = (seqCount >> 8);			// Left Side
		// Time Stamp
		time = getMs();
		txBuf[3] = (uint8)(time >> 24) & 0xff;
		txBuf[4] = (uint8)(time >> 16) & 0xff;
		txBuf[5] = (uint8)(time >> 8) & 0xff;
		txBuf[6] = (uint8)time & 0xff;
		radioQueueTxSendPacket();
		
		//responseLength = sprintf(response, "Packet #%d Queued\r\n",seqCount);
		//usbComTxSend(response, responseLength);
		
		seqCount++;
		return 1;
	}
	else 
	{
		//uint8 responseLength;
		//responseLength = sprintf(response, "Queueing Failed\r\n");
		//usbComTxSend(response, responseLength);
		return 0;
	}
}


/** Processes a new byte received from the USB virtual COM port.
 * 
 * NOTE: This function is complicated because it handles receiving multi-byte commands.
 * If all of your commands are just 1 byte, then you should remove executeCommand and
 * replace processByte with something much simpler like this:
 *     switch(byteReceived)
 *     {
 *         case COMMAND_A: actionA(); break;
 *         case COMMAND_B: actionB(); break;
 *         case COMMAND_C: actionC(); break;
 *         default: serialProtocolError = 1; break;
 *     }
 * */
void processByte(uint8 byteReceived)
{
	int i;
    if (byteReceived & 0x80)
    {
        // We received a command byte.
        
        if (dataBytesLeft > 0)
        {
            serialProtocolError = 1;
        }
        
        commandByte = byteReceived;
        dataBytesReceived = 0;
        dataBytesLeft = 0;
        
        // Look at the command byte to figure out if it is valid, and
        // determine how many data bytes to expect.
        switch(commandByte)
        {
            case COMMAND_TOGGLE_YELLOW_LED:
            case COMMAND_CLEAR_ERROR:
                dataBytesLeft = 0;
                break;

            default:
                // Received an invalid command byte.
                serialProtocolError = 1;
                return;
        }
    
        if (dataBytesLeft==0)
        {
            // We have received a single-byte command.
            executeCommand();
        }
    }
    else if (dataBytesLeft > 0)
    {
        // We received a data byte for a binary command.

        dataBytes[dataBytesReceived] = byteReceived;
        dataBytesLeft--;
        dataBytesReceived++;
        
        if (dataBytesLeft==0)
        {
            // We have received the last byte of a multi-byte command.
            executeCommand();
        }
    }
    else
    {
        // We received a byte that is less than 128 and it is not part of
        // a binary command.  Maybe it is an ASCII command

        uint8 responseLength;
        uint32 time;
        switch(byteReceived)
        {
        case 't':
            time = getMs();
            // SDCC's default sprintf doesn't seem to support 32-bit ints, so we will
            // split getMs into two parts and print it in hex.
            responseLength = sprintf(response, "time=0x%04x%04x\r\n", (uint16)(time >> 16), (uint16)time);
            usbComTxSend(response, responseLength);
            break;

        case 'y':
            yellowLedOn ^= 1;
            break;
			
		case 's':
			if(param_radio_channel == 128)
			{
				seqCount = 0;
			}
			else 
			{
				seqCount = 1000;
			}
			for(i=0; i < 1000; i++) 
			{
				if(sendPacket() == 0) i--;
			}
			responseLength = sprintf(response, "Packets Sent\r\n");
            usbComTxSend(response, responseLength);
			break;
		case 'd':
			
			seqCount = 0;
			radioQueueChangeChannel(228);
			for(i=0; i < 1000; i++) 
			{
				if(sendPacket() == 0) i--;
			}
			
			seqCount = 0;
			radioQueueChangeChannel(128);
			for(i=0; i < 1000; i++) 
			{
				if(sendPacket() == 0) i--;
			}
			
			responseLength = sprintf(response, "Packets Sent on Both Channels\r\n");
            usbComTxSend(response, responseLength);
			break;
		case 'c':
			cSendingOn = 1;
			break;
		case 'o':
			cSendingOn = 0;
			break;
        }
    }
}

/** Checks for new bytes available on the USB virtual COM port
 * and processes all that are available. */
void processBytesFromUsb()
{
    uint8 bytesLeft = usbComRxAvailable();
    while(bytesLeft && usbComTxAvailable() >= sizeof(response))
    {
        processByte(usbComRxReceiveByte());
        bytesLeft--;
    }
}

void main()
{
	int i;
    systemInit();
    usbInit();
	radioQueueInit();

    while(1)
    {
        boardService();
        updateLeds();
        usbComService();
        processBytesFromUsb();
		if(cSendingOn == 1) {
			cSendingCount ++;
			if(param_radio_channel == 128)
			{
				seqCount = 0;
			}
			else 
			{
				seqCount = 1000;
			}
			for(i=0; i < 1000; i++) 
			{
				if(sendPacket() == 0) i--;
			}
			if(cSendingCount > 10)
			{
				cSendingCount = 0;
				cSendingOn = 0;
			}
		}
    }
}
