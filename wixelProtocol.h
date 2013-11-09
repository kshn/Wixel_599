/*This is a header file outlining the visible functions of the the file for implementing the unreliable broadcast + reliable unicast mechanism. This is to be implemented between a pair of Wixels at the "Hub" and multiple Wixels at the "Nodes". One of the Hub Wixels is for transmitting data/commands, and the other is meant for receiving the responses and interrupts. The implementation details of this will be fleshed out in subsequent updates. */


/** Dependencies **************************************************************/
#include <wixel.h>
#include <board1.h>
#include <usb.h>
#include <usb_com.h>
#include <radio_com.h>
#include <radio_link.h>
#include <uart1.h>

// Insert other relevant library names here

//*****************************************************************************


// Functions ******************************************************************
//The function declarations below are meant for the various operations at the hub. For the nodes, we will have a subset of these functions.


void addNode(uint8 nodeID); 
//This function should be called whenever we wish to add a node to the network. The Wixel at this node should be preconfigured with a unique nodeID, which will be used to address this particular node. All packet transactions with this node should contain this nodeID at the header. This function also allocates a certain time slot for the return packets from this node to the hub.

void removeNode(uint8 nodeID);
//This function should be called when we wish to disconnect a node from the network. The node we wish to disconnect has a unique nodeID which must be specified when calling this function. Calling this function also frees up the time slot allocated to the said node. 

void bcastData(int command);
//This function is used to add "command" to the unreliable queue. This "command" is received and decoded by all the nodes. This method of communication is unreliable.

void ucastData(uint8 nodeID, int command);
//This function should be called to send commands to a particular node, specified by the nodeID, reliably. The command is added to the reliable queue, with an assigned priority 0, i.e., this type of data is given a lower priority than the other type of data with reliable communication, the INTERRUPT type.


//The following functions are helper functions, meant to assist with the implementation. ***************************************
void syncPacket();
//This function takes no arguments, and should be called periodically to transmit a synchronization packet. The return time slots are to be defined with respect to this synchronization packet.

int decodeRcvdPkt();
//This function will be called to decode the received packets at the receiving Wixel and perform relevant operations. The received packets may be ACK/NAK, INT, or other data/commands. This function will have to be called for every received packet. 

int createPkt(uint8 nodeID, int data);
//This creates the packet for transmission by adding the header/trailing bits specifying the destination, and the packet numbers for this particular message.

void addToQueue(int pkt, uint8 type);
//This is meant to add the packets (pkt) to the transmit queue. The argument 'type' is used to denote if the packet is a broadcast packet (lowest priority), unicast packet, or an interrupt packet, determining the priority of that packet. 


// ************************************ Program flow ***************************

This is the expected sequence of operations the user needs to do to carry out the desired communications. 
1) Add the n nodes of the network by calling the addNode() function, with the unique IDs. This also allots a time slot for the return channel to each of the nodes, relative to the sync packet.   
2) To broadcast a message, call the bcastData() function, passing the message as an argument.
3) To reliably send a message to node A, pass the message and the node ID to the ucastData() function. 

Behind the scenes

addNode():
    When this function is called, the node ID is added to an array at the hub which maintains a list of all the nodes currently connected to the system. This also send a packet to the concerned node giving it timing information for the return channel. 
    The timing information here includes the following information:
        start time
        length
        # repeat
        beginning/end/random
    NOTE TO THE PROF: SHOULD THE SYNC PULSE ALWAYS BE SENT AT A CONSTANT RATE, OR DO WE CHANGE THE RATE OF THE SYNC PULSE WITH THE NUMBER OF NODES ATTACHED? IF WE'RE CHANGING THE RATE OF THE SYNC PULSE, THEN THIS INFORMATION SHOULD BE BROADCAST TO ALL THE NODES IN THE SYSTEM EACH TIME THE addNode() FUNCTION IS CALLED.

removeNode():
    When this function is called, the node corresponding to this ID is removed from the array at the hub, thereby disconnecting this node from the network. This should also send the updated Sync pulse rate to all the nodes.

bcastData():
    This function in turn calls the createPkt() function, which adds the header, and addToQueue() function, which puts this packet in the priority queue with the lowest priority. When this packet is transmitted, all the nodes decode this packet and do the required operations.
    

