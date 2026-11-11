//
// Included Files
//
#include "driverlib.h"
#include "device.h"


//
// Defines
//
#define MSG_DATA_LENGTH    4
#define TX_MSG_OBJ_ID    1
#define RX_MSG_OBJ_ID    2


//
// Globals
//
uint16_t txMsgData[4];
uint16_t rxMsgData[4];


//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();


    //
    // Initialize GPIO and configure GPIO pins for CANTX/CANRX
    // on module A
    //
    Device_initGPIO();
    GPIO_setPinConfig(DEVICE_GPIO_CFG_CANRXA);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_CANTXA);


    //
    // Initialize the CAN controllers
    //
    CAN_initModule(CANA_BASE);


    //
    // Set up the CAN bus bit rate to 500kHz for each module
    // Refer to the Driver Library User Guide for information on how to set
    // tighter timing control. Additionally, consult the device data sheet
    // for more information about the CAN module clocking.
    //
    CAN_setBitRate(CANA_BASE, DEVICE_SYSCLK_FREQ, 500000, 20);


    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();


    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();


    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;


    //
    // Initialize the transmit message object used for sending CAN messages.
    // Message Object Parameters:
    //      CAN Module: A
    //      Message Object ID Number: 1
    //      Message Identifier: 0x00000055
    //      Message Frame: Extended
    //      Message Type: Transmit
    //      Message ID Mask: 0x0
    //      Message Object Flags: None
    //      Message Data Length: 4 Bytes
    //
    CAN_setupMessageObject(CANA_BASE, TX_MSG_OBJ_ID, 0x00000055,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS, MSG_DATA_LENGTH);


   //
   // Initialize the receive message object used for receiving CAN messages.
   // Message Object Parameters:
   //      CAN Module: B
   //      Message Object ID Number: 1
   //      Message Identifier: 0x00000056
   //      Message Frame: Extended
   //      Message Type: Receive
   //      Message ID Mask: 0x0
   //      Message Object Flags: Receive Interrupt
   //      Message Data Length: 4 Bytes
   //
    CAN_setupMessageObject(CANA_BASE, RX_MSG_OBJ_ID, 0x00000056,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_RX, 0,
                               CAN_MSG_OBJ_NO_FLAGS, MSG_DATA_LENGTH);






    //
    // Initialize the transmit message object data buffer to be sent
    //
    txMsgData[0] = 0x12;
    txMsgData[1] = 0x34;
    txMsgData[2] = 0x56;
    txMsgData[3] = 0x78;


    //
    // Start CAN module A and B operations
    //
    CAN_startModule(CANA_BASE);
//    CAN_startModule(CANB_BASE);


    //
    // Transmit messages from CAN-A to CAN-B
    //
    while(1)
    {
            CAN_sendMessage(CANA_BASE, TX_MSG_OBJ_ID, MSG_DATA_LENGTH,
                            txMsgData);


            CAN_readMessage(CANA_BASE, RX_MSG_OBJ_ID, rxMsgData);


        //
        // Delay 0.25 second before continuing
        //
        DEVICE_DELAY_US(250000);


        //
        // Increment the value in the transmitted message data.
        //
        txMsgData[0] += 0x01;
        txMsgData[1] += 0x01;
        txMsgData[2] += 0x01;
        txMsgData[3] += 0x01;
        
        //
        // Reset data if exceeds a byte
        //
        if(txMsgData[0] > 0xFF)
        {
            txMsgData[0] = 0;
        }
        if(txMsgData[1] > 0xFF)
        {
            txMsgData[1] = 0;
        }
        if(txMsgData[2] > 0xFF)
        {
            txMsgData[2] = 0;
        }
        if(txMsgData[3] > 0xFF)
        {
            txMsgData[3] = 0;
        }
    }


}