/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : main.c
 *    Description : Main module
 *
 *    History :
 *    1. Date        : August 5, 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for the IAR LPC2478-SK board.
 *  It implements USB CDC (Communication Device Class) device and install
 * it like a Virtual COM port. The UART0 is used for physical implementation
 * of the RS232 port.
 *
 * Jumpers:
 *  EXT/JLINK  - depending of power source
 *  ISP_E      - unfilled
 *  RST_E      - unfilled
 *  BDS_E      - unfilled
 *  C/SC       - SC
 *
 * Note:
 *  After power-up the controller gets it's clock from internal RC oscillator that
 * is unstable and may fail with J-Link auto detect, therefore adaptive clocking
 * should always be used. The adaptive clock can be select from menu:
 *  Project->Options..., section Debugger->J-Link/J-Trace  JTAG Speed - Adaptive.
 *
 * The LCD shares pins with Trace port. If ETM is enabled the LCD will not work.
 *
 *    $Revision: 28 $
 **************************************************************************/
#include "includes.h"

#define NONPROT 0xFFFFFFFF
#define CRP1  	0x12345678
#define CRP2  	0x87654321
/*If CRP3 is selected, no future factory testing can be performed on the device*/
#define CRP3  	0x43218765

#ifndef SDRAM_DEBUG
#pragma segment=".crp"
#pragma location=".crp"
__root const unsigned crp = NONPROT;
#endif

#define LCD_VRAM_BASE_ADDR ((Int32U)&SDRAM_BASE_ADDR)

extern Int32U SDRAM_BASE_ADDR;
extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;
////////////////////uIP ////////////////////

 float pow1 = 35;
 float volt1 = 130;
 float curr1 = 15;

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

// CALCULATED COEFFICIENTS FOR MEASUREMENT CONVERSIONS
#define VRMS_COEFF 2292.58464996;
#define IRMS_COEFF 154.2380;//159.2173;
#define PRMS_COEFF 241210 //  14782//160.38492//241210;
#define PRRMS_COEFF 241210 //  14782//160.38492//241210;
//TIMER RELATED CONSTANTS
#define DISCOVERY_TIME 80
#define SETTLE_TIME    20

//////////////////////// GLCD ANG GUI RELATED VALUES
#define ButtonXleft  25
#define ButtonXright 62
#define ButtonYup    214
#define ButtonYdown  239

#define Button2Xleft  240
#define Button2Xright 276
#define Button2Yup    214
#define Button2Ydown  239

#define STATUS_BARX 0
#define STATUS_BARXx 319   
#define STATUS_BARY 200
#define STATUS_BARYy 210

#define WhiteBackground 0xffffffff
#define GreyBackground  0x00ffffff
#define BlackFont       0x000000

//
#define DEFFAULT_MINIMUM_CHANGE 1
#define ALLOWED_ERROR_PERCENT 5.5
//DEVICE SIGNATURE ARRAY SETTINGS
#define DEVICE_LIST_SIZE 6
#define DEVICE_PARAMETER_LIST_SIZE 4


#define VOLTAGE 0 
#define IRMS_INDEX 1



float VRMS, tempsf, IRMS, PRMS, PRRMS;
Int32U V_VAL, I_VAL, P_VAL, PR_VAL;
float VRMS_IDLE, IRMS_IDLE, PRMS_IDLE, PRRMS_IDLE;
float IRMS_MIN, IRMS_MAX, IRMS_AVG;
float PRMS_MIN, PRMS_MAX, PRMS_AVG;
float PRRMS_MIN, PRRMS_MAX, PRRMS_AVG;
float minimum_change;
float dif1, dif2;
float TESTBUFF[512];
int testbuffc = 0;
int devices_registered = 0;
enum DEV_STATE CurrentState;
int registered_devices = 3;
int connected_devices = 0;
int number_of_measurements_made = 0;
float DeviceSignatures[DEVICE_LIST_SIZE][DEVICE_PARAMETER_LIST_SIZE];
unsigned long timerval;


////
#define CURSOR_H_SIZE 32
#define CURSOR_V_SIZE 32
Int32U cursor_x = (C_GLCD_H_SIZE - CURSOR_H_SIZE) / 2, cursor_y = (C_GLCD_V_SIZE - CURSOR_V_SIZE) / 2;
ToushRes_t XY_Touch;
Boolean Touch = FALSE;
bool hasItIncremented = false;
////

/* Device states */
enum DEV_STATE {
    state_idle,
    state_discover,
    state_learn,

};

/*************************************************************************
 * Function Name: calculate_error_percent_of_floats
 * Parameters: float x, float y
 *
 * Return: float
 *
 * Description: INPUTS 2 FLOATS, RETURNS % DIFFERNECE BETWEEN THEM
 *
 *************************************************************************/
float calculate_error_percent_of_floats(float x, float y) {
    float c;
    c = (float) (y * 100) / x;
    if (((float) 100 - c) < 0) {
        return (float) c - 100;
    } else {
        return (float) 100 - c;
    }
}


/*************************************************************************
 * Function Name: setButtons
 * Parameters: bool yes
 *
 * Return: none
 *
 * Description: Based on input values either prints the button text or removes it
 *************************************************************************/
void setButtons(bool yes){
  if(yes==true){
    ////button yes
    GLCD_SetFont(&Terminal_18_24_12, BlackFont, WhiteBackground-10);
    GLCD_SetWindow(ButtonXleft, ButtonYup, ButtonXright, ButtonYdown);
    GLCD_TextSetPos(0, 0);
    GLCD_print("YES");
    //button NO          
    GLCD_SetFont(&Terminal_18_24_12, BlackFont, WhiteBackground-10);
    GLCD_SetWindow(Button2Xleft, Button2Yup, Button2Xright, Button2Ydown);
    GLCD_TextSetPos(0, 0);
    GLCD_print("NO");
                }
    else if (yes==false){
    
    ////button yes
    GLCD_SetFont(&Terminal_18_24_12, BlackFont, WhiteBackground);
    GLCD_SetWindow(ButtonXleft, ButtonYup, ButtonXright, ButtonYdown);
    GLCD_TextSetPos(0, 0);
    GLCD_print("   ");
    //button NO          
    GLCD_SetFont(&Terminal_18_24_12, BlackFont, WhiteBackground);
    GLCD_SetWindow(Button2Xleft, Button2Yup, Button2Xright, Button2Ydown);
    GLCD_TextSetPos(0, 0);
    GLCD_print("   ");
  }
}

float totest;
float totestAVG;
bool deviceFound = false;

/*************************************************************************
 * Function Name: checkTouch
 * Parameters: none
 *
 * Return: int
 *
 * Description: based on X Y positions of registerd touch, return an int
 * representing the predefined area touched. This allows to handle multiple 
 * buttons and controls. return 0 by deffault if no predefined screen areas touched.
 *************************************************************************/
int checkTouch() {
    if (TouchGet(&XY_Touch)) {
        cursor_x = XY_Touch.X;
        cursor_y = XY_Touch.Y;
        // GLCD_Move_Cursor(cursor_x, cursor_y);
        if (FALSE == Touch) {
            Touch = TRUE;
            // To detect if touch event happened inside Window
            if (cursor_x >= ButtonXleft && cursor_x <= ButtonXright && cursor_y >= ButtonYup && cursor_y <= ButtonYdown) {
                USB_H_LINK_LED_FCLR = USB_H_LINK_LED_MASK; // HERE do needed stuff
                return 1;
            }
            // To detect if touch event happened inside Window
            if (cursor_x >= Button2Xleft && cursor_x <= Button2Xright && cursor_y >= Button2Yup && cursor_y <= Button2Ydown) {
                USB_H_LINK_LED_FCLR = USB_H_LINK_LED_MASK;
                return 2;
            }
            if (cursor_x >= 280 && cursor_x <= 319 && cursor_y >= 5 && cursor_y <= 15) {
             USB_H_LINK_LED_FCLR = USB_H_LINK_LED_MASK;
                //280, 5, 319, 15
                  for (int i = 0; i <= DEVICE_LIST_SIZE; i++) {
                  DeviceSignatures[i][0] = 0;
                  DeviceSignatures[i][1] = 0;
                  DeviceSignatures[i][2] = 0;
                  DeviceSignatures[i][3] = 0;
                  registered_devices=0;
                  timer_counter=0;
                  
                        GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                        GLCD_SetWindow(STATUS_BARX, STATUS_BARY, STATUS_BARXx, STATUS_BARYy);
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("\f                                                 ");
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("ALL DEVICES SIGNATURE DATA DELETED");
                  }
                
                return 100;
            }
        }
    } else if (Touch) {
        USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
        Touch = FALSE;
    }

    return 0;
}

/*************************************************************************
 * Function Name: syncMinChange
 * Parameters: none
 *
 * Return: none
 *
 * Description:  
 * //outdated, use POWER NOw.....0.006 is default current change to triger event....
 * NEW min change value could be found from fluctations of connected devices. 
 * for instance take MIN  and MAX VALUES find their difference=> new THRESHOLD
 * IF NOW DEVICES CONNECTED , SET THRESHOLD TO DEFFAULT_MINIMUM_CHANGE value (0.006 at the moment)
 *
 *************************************************************************/
void syncMinChange() {
    if (connected_devices == 0) {
        minimum_change = DEFFAULT_MINIMUM_CHANGE;
    } else { // needs improvement // maybe scroll thru all , find max, and make from it
        for (int i = 0; i <= registered_devices - 1; i++) {
            if (DeviceSignatures[i][3] == 1) {
                minimum_change = (float) DeviceSignatures[i][0]*0.05;
            }
        }
    }
}

/*************************************************************************
 * Function Name: doStateMachineStuff
 * Parameters: none
 *
 * Return: none
 *
 * Description:  State machine. Called From main() while loop after succesfully receiving and parsing a packet of data.
 * At the moment uses 2 states.
 * STATE 1 is state_idle: Serves to register IDLE STATE VARIABLES and NOTICE CHANGE if new variables differ from "IDLE" ones.
 * IF SUFFICIENT PREDIFENED change detected, than change state to state_discover.
 * Before transition to it- "Register" idle variables, reset timer , number of measurements made etc... 
 * STATE 2 is state_discover: At this state the behavior of newly connected device will be monitored for PREDEFINED time. 
 * Based on observation, compare results to registered devices signatures, if match found - INFORM THE USER, if not found- PROMT user to add new device
 * STATE 3 maybe to see which was disconnected.
 *************************************************************************/
void doStateMachineStuff() {
    switch (CurrentState) {
        case state_idle:
            // FIND DIFFERENCE BETWEEN NEWLY RECEIVED AND PREVIOUS VARIABLEs.
            dif1 = (float) PRMS_IDLE - PRMS;
            dif2 = (float) PRMS - PRMS_IDLE;
            // IF DIFFERENCE IS SUFFICIENT
            if (dif1 >= minimum_change || dif2 >= minimum_change) {
                if (IRMS_IDLE < IRMS) {
                    hasItIncremented = true;
                } else {
                    hasItIncremented = false;
                }
                //DETECTION AND IDENTIFICATION OF NEW DEVICE PREPARATION
                // Display notice
                GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                GLCD_SetWindow(STATUS_BARX, STATUS_BARY, STATUS_BARXx, STATUS_BARYy);
                GLCD_TextSetPos(0, 0);
                GLCD_print("\f                                                 ");
                GLCD_TextSetPos(0, 0);
                GLCD_print("\f Change detected:%3.3f  ||%3.3f Start state_discover  \r\n", dif1, dif2) ;
                CurrentState = state_discover;

                //Should we start a timer ? lets try
                timer_counter = 0;
                number_of_measurements_made = 0;
                //for now ///
                IRMS_MIN = IRMS;
                PRMS_MIN = PRMS;
                //// set var to false by deffault
                deviceFound = false;
            } else {
                // IF EVENT NOT CAPTURED, CONTINUE REGISTERING IDLE VALUES
                VRMS_IDLE = VRMS;
                IRMS_IDLE = IRMS;
                PRMS_IDLE = PRMS;
            }
            break;

        case state_discover:
            /// Wait for some time elapse based on timer. In order to get stabilized meter values.
            if ((timer_counter < DISCOVERY_TIME) &&(timer_counter >= SETTLE_TIME)) { // IRMS_MIN,IRMS_MAX,IRMS_AVG;
                if (PRMS_MIN >= PRMS) {
                    PRMS_MIN = PRMS;
                }
                if (PRMS_MAX <= PRMS) {
                    PRMS_MAX = PRMS;
                }
                TESTBUFF[testbuffc++] = PRMS;
                PRMS_AVG = PRMS_AVG + PRMS;
                number_of_measurements_made++;
                break;
            } else if ((timer_counter >= DISCOVERY_TIME)&&PRMS_IDLE<PRMS) {
                totest = fabs(PRMS_IDLE - (PRMS_AVG / number_of_measurements_made));
                totestAVG = (PRMS_AVG / number_of_measurements_made);
                //now the problem is to distinguish IRMS of just connected device; 
                // Easiest way is to substract from total IRMS the IRMS of KNOWN DEVICE
                // So check which devices are connected and substract their IRMS from total 

                for (int i = 0; i <= DEVICE_LIST_SIZE; i++) {
                    if (DeviceSignatures[i][3] == 1) {
                        totestAVG = (PRMS_AVG / number_of_measurements_made);
                        totestAVG = (float) totestAVG - DeviceSignatures[i][0];
                        
                        PRMS_AVG = PRMS_AVG - (DeviceSignatures[i][0] * number_of_measurements_made);
                        PRMS_MIN = PRMS_MIN - DeviceSignatures[i][1];
                        PRMS_MAX = PRMS_MAX - DeviceSignatures[i][2];

                    }
                }
                  

                //CHECK IF MIN RECEIVED VALUE IS HIGER OR EQUAL TO LOWEST REGISTERED in the registered device list 
                //AND if MAX VALUE RECEIVED IS LESS OR EQUAL TO HIGHEST REGISTERED.
                // IF YES deviceFound=true; connected_devices++; CurrentState= state_idle
                // think of adding average IRMS with 5-10 % error ? 
                for (int i = 0; i <= registered_devices - 1; i++) {
                    if (//(PRMS_MAX<=DeviceSignatures[i][2])  //maybe one more condition ? //(PRMS_MIN>=DeviceSignatures[i][1]) removed for now, always changing blet
                            (calculate_error_percent_of_floats((totestAVG-connected_devices), DeviceSignatures[i][0]) <= (ALLOWED_ERROR_PERCENT+connected_devices))
                            ) {
                        //////////////////////test area , just print some shit if true   
                        DeviceSignatures[i][3] = 1;
                        GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                        GLCD_SetWindow(STATUS_BARX, STATUS_BARY, STATUS_BARXx, STATUS_BARYy);
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("\f                                                 ");
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("DEVICE NUMBER %i connected ", i + 1);

                        deviceFound = true;
                        connected_devices++;
                        CurrentState = state_idle;
                        PRMS_AVG = 0;
                        // trying to sync 
                        VRMS_IDLE = VRMS;
                        IRMS_IDLE = IRMS;
                        PRMS_IDLE = PRMS;
                        syncMinChange();
                    }
                }
                // IF NO DEVICE WAS FOUND IN THE LIST, PROMT USER TO TO SAVE IT SIGNATURE  OR NOT
                if (deviceFound == false) {

                    GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                    GLCD_SetWindow(STATUS_BARX, STATUS_BARY, STATUS_BARXx, STATUS_BARYy);
                    GLCD_TextSetPos(0, 0);
                    GLCD_print("\f                                                 ");
                    GLCD_TextSetPos(0, 0);
                    GLCD_print("            DEVICE NOT FOUND ! SAVE IT ? ");
                    //Draw buttons now 
                    setButtons(true);
                    /// example on stopping other actions , until USER input is handeled
                    int action = 0;
                    while (action == 0) {
                        action = checkTouch();
                    }
                    // If user pressed yes
                    if (action == 1) {
                      
                        DeviceSignatures[registered_devices][0] = totestAVG;// PRMS_AVG / number_of_measurements_made;
                        DeviceSignatures[registered_devices][1] = PRMS_MIN;
                        DeviceSignatures[registered_devices][2] = PRMS_MAX;
                        DeviceSignatures[registered_devices][3] = 1;
                        registered_devices++;
                        connected_devices++;
                        GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                        GLCD_SetWindow(STATUS_BARX, STATUS_BARY, STATUS_BARXx, STATUS_BARYy);
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("\f                                                 ");
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("DEVICE NUMBER %i added ", registered_devices);
                        CurrentState = state_idle;
                    } else if (action == 2) {
                        CurrentState = state_idle;
                    }
                    //undraw buttons
                     setButtons(false);
                }
                break;
            } else if ((timer_counter >= DISCOVERY_TIME)&&PRMS_IDLE>PRMS){
              
              for (int i = 0; i <= registered_devices - 1; i++) {
                    if ((calculate_error_percent_of_floats(PRMS_IDLE-PRMS, DeviceSignatures[i][0]) <= ALLOWED_ERROR_PERCENT)) {
                        //////////////////////test area , just print some shit if true   
                        DeviceSignatures[i][3] = 0;
                        GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                        GLCD_SetWindow(STATUS_BARX, STATUS_BARY, STATUS_BARXx, STATUS_BARYy);
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("\f                                                 ");
                        GLCD_TextSetPos(0, 0);
                        GLCD_print("DEVICE NUMBER %i Disconnected ", i + 1);

                        deviceFound = false;
                        connected_devices--;
                        CurrentState = state_idle;
                        PRMS_AVG = 0;
                        // trying to sync 
                        //VRMS_IDLE = VRMS;
                       // IRMS_IDLE = IRMS;
                        PRMS_IDLE = PRMS;
                        syncMinChange();
                    }
                }
              break;
            }           
            break;
        case state_learn:
            // NOT NEEDED/IMPLEMENTED YET       
            break;
        default:
            break;
    }

}

/*************************************************************************
 * Function Name: parse_SmartBox_Frame
 * Parameters: none
 *
 * Return: none
 *
 * Description: Extracts the values from the UART_RX_BUFFER buffer.
 * Performs necessary calculations and updates the global variables.
 *************************************************************************/
void parse_SmartBox_Frame() {
    ///extract voltage////////////////////
    V_VAL = (UART_RX_BUFFER[1] << 24) | (UART_RX_BUFFER[2] << 16) | (UART_RX_BUFFER[3] << 8) | UART_RX_BUFFER[4];
    //not cool but for now to get from 0.1 to 1 we will devide by 100000000
    VRMS = (float) V_VAL / 100000000;
    VRMS = (float) VRMS* VRMS_COEFF;
    //////////////////////////////IRMS PART   
    I_VAL = (UART_RX_BUFFER[5] << 24) | (UART_RX_BUFFER[6] << 16) | (UART_RX_BUFFER[7] << 8) | UART_RX_BUFFER[8]; // strtoul(val_buf,NULL,16) ;
    //not cool but for now to get from 0.1 to 1 we will devide by 100000000
    IRMS = (float) I_VAL / 100000000;
    IRMS = (float) IRMS* IRMS_COEFF;
    if (IRMS < 0.007) {
        IRMS = 0;
    }
    //////////////////////////////REAL/ACTIVE POWER PART
    P_VAL = (UART_RX_BUFFER[9] << 24) | (UART_RX_BUFFER[10] << 16) | (UART_RX_BUFFER[11] << 8) | UART_RX_BUFFER[12]; // strtoul(val_buf,NULL,16) ;
    PRMS = (float) P_VAL / 100000000;
    PRMS = (float) PRMS * (float) PRMS_COEFF;
    PRMS = (float) PRMS / 2;
    PRMS = (float) PRMS - 0.71;
    tempsf = (float) IRMS*VRMS;
    //////////////////////////////REACTIVE POWER PART
    PR_VAL = (UART_RX_BUFFER[13] << 24) | (UART_RX_BUFFER[14] << 16) | (UART_RX_BUFFER[15] << 8) | UART_RX_BUFFER[16];
    PRRMS = (float) PR_VAL / 644380;
}

/*************************************************************************
 * Function Name: print_device_consumptions
 * Parameters: none
 *
 * Return: none
 *
 * Description: Function to Draw the power consumption of currently connected devices.
 * Look through device signature array to see which devices are connected ,
 * based on total power consumption and device signatures estimate aprox. consumption of each connected device.
 *************************************************************************/
void print_device_consumptions() {
  
 for (int i = 0; i <= DEVICE_LIST_SIZE; i++) {
                            GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                            GLCD_SetWindow(0, 91, 319, 185);
                            GLCD_TextSetPos(0, i);                        
                    if (DeviceSignatures[i][3] == 1) {     
                          // For now just print Real and Signature data
                        GLCD_print("Device:%lu I:%3.3fA P:%3.3fW Q:%3.3fVAR\r\n",i+1 ,IRMS, DeviceSignatures[i][0] , PRRMS);                        
                    }else{GLCD_print("Device:%lu Disconnected \r\n", i+1);} 
                }
                            GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
                            GLCD_SetWindow(0, 91, 300, 185);
                            GLCD_TextSetPos(0, 7);   
                            GLCD_print("Device signatures stored:%i \r\n",registered_devices);
}


/*************************************************************************
 * Function Name: update_GLCD_Measurements
 * Parameters: none
 * Return: none
 * Description: Updates Main Measurements Values On The GLCD Screen
 *************************************************************************/
void update_GLCD_Measurements(){
            GLCD_SetFont(&Terminal_18_24_12, BlackFont, WhiteBackground);
            GLCD_SetWindow(0, 40, 310, 88);
            GLCD_TextSetPos(0, 0);
            GLCD_print("   V:%3.3fV I:%3.3fA \r\n",VRMS,IRMS);
            GLCD_TextSetPos(0, 1);
            GLCD_print("   P:%3.3fW  Q:%3.3fVAR \r\n",PRMS,PRRMS);
}

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/

int main(void) {
///uIP
unsigned int i;
uip_ipaddr_t ipaddr;
struct timer periodic_timer, arp_timer;
///
    GLCD_Ctrl(FALSE);
    // Init GPIO
    GpioInit();
#ifndef SDRAM_DEBUG
    // MAM init
    MAMCR_bit.MODECTRL = 0;
    MAMTIM_bit.CYCLES = 3; // FCLK > 40 MHz
    MAMCR_bit.MODECTRL = 2; // MAM functions fully enabled
    // Init clock
    InitClock();
    // SDRAM Init
    SDRAM_Init();
#endif // SDRAM_DEBUG
    // Init VIC
    VIC_Init();
    // GLCD init
    GLCD_Init(IarLogoPic.pPicStream, NULL);
    // Disable Hardware cursor
    GLCD_Cursor_Dis(0);
    // Init UART 0
    UartInit(UART_0, 4, NORM);
    // set current state to idle.
    CurrentState = state_idle;

    // Init touch screen
    TouchScrInit();

    // Touched indication LED
    USB_H_LINK_LED_SEL = 0; // GPIO
    USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
    USB_H_LINK_LED_FDIR |= USB_H_LINK_LED_MASK;
    __enable_interrupt();

    // Enable GLCD 320x240
    GLCD_Ctrl(TRUE);

    GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
    GLCD_SetWindow(70, 5, 245, 15);
    GLCD_TextSetPos(0, 0);
    GLCD_print("\fIAR Systems SmartBox UI V 0.2");
  
    GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
    GLCD_SetWindow(STATUS_BARX, STATUS_BARY, STATUS_BARXx, STATUS_BARYy);
    GLCD_TextSetPos(0, 0);
    GLCD_print("\f                                                 ");
    GLCD_TextSetPos(0, 0);
    GLCD_print("\fSystem started! IDLE MODE");
     
    UartLineCoding_t UartLineCoding;
    // CDC_LineCoding = UsbCdcGetLineCodingSettings();
    // Update the baud rate
    UartLineCoding.dwDTERate = 115200;
    // Update the stop bits number
    UartLineCoding.bStopBitsFormat = UART_ONE_STOP_BIT;
    // Update the parity type
    UartLineCoding.bParityType = UART_NO_PARITY;
    // Update the word width
    UartLineCoding.bDataBits = UART_WORD_WIDTH_8;
    // Set UART line coding
    UartSetLineCoding(UART_0, UartLineCoding);
    // String termination of receive buffer
    UART_RX_BUFFER[UART0_RX_SIZE - 1] = '\0';

    //TIMER 3 INIT
    init_timer(TIME_INTERVAL);

    //////////////test setup /// predifined variables for solder iron and fan in mode 3 and white lamp
    DeviceSignatures[0][0] = 32.6;
    DeviceSignatures[0][1] = 21;
    DeviceSignatures[0][2] = 39;

    DeviceSignatures[1][0] = 25.2;
    DeviceSignatures[1][1] = 23;
    DeviceSignatures[1][2] = 26;

    DeviceSignatures[2][0] = 14.4;
    DeviceSignatures[2][1] = 11;
    DeviceSignatures[2][2] = 16.4;

    // setting minimum current difference value, which surves to triger event if device connected disconnnected
    // could be changed later based on which devices are connected.
    minimum_change = DEFFAULT_MINIMUM_CHANGE;

    totest = calculate_error_percent_of_floats(32.7, 32);
    ////
     // Sys timer init 1/100 sec tick
  clock_init(2);

  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);

    ////
    while(!tapdev_init());


  // uIP web server
  // Initialize the uIP TCP/IP stack.
  uip_init();

  uip_ipaddr(ipaddr, 192,168,0,100);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 192,168,0,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);

  // Initialize the HTTP server.
  httpd_init();
    
    
    /////
    
    
    while (1) {
      uip_len = tapdev_read(uip_buf);
    if(uip_len > 0)
    {
      if(BUF->type == htons(UIP_ETHTYPE_IP))
      {
	      uip_arp_ipin();
	      uip_input();
	      /* If the above function invocation resulted in data that
	         should be sent out on the network, the global variable
	         uip_len is set to a value > 0. */
	      if(uip_len > 0)
        {
	        uip_arp_out();
	        tapdev_send(uip_buf,uip_len);
	      }
      }
      else if(BUF->type == htons(UIP_ETHTYPE_ARP))
      {
        uip_arp_arpin();
	      /* If the above function invocation resulted in data that
	         should be sent out on the network, the global variable
	         uip_len is set to a value > 0. */
	      if(uip_len > 0)
        {
	        tapdev_send(uip_buf,uip_len);
	      }
      }
    }
    else if(timer_expired(&periodic_timer))
    {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++)
      {
      	uip_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0)
        {
          uip_arp_out();
          tapdev_send(uip_buf,uip_len);
        }
      }
#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
        uip_udp_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0) {
          uip_arp_out();
          tapdev_send();
        }
      }
#endif /* UIP_UDP */
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer))
      {
        timer_reset(&arp_timer);
        uip_arp_timer();
      }
    }
      
      
      
      
      
      
      
      
      
      
      
        // If data received from UART REPRESENTS [0x01][0x02][...Number of measurements * 4 bytes....][0x04][0x05]
        // Then Parse the the received frame and update display
        if (FRAME_READY) {
            FRAME_READY = false;
            parse_SmartBox_Frame();
            //////////update display
            update_GLCD_Measurements();
             pow1 = PRMS;
             volt1 = VRMS;
             curr1 =IRMS;
            //
            doStateMachineStuff();           
            //
            print_device_consumptions();
        }
        //for now here, to test and debug. Might not be needed in main loop in final design
        checkTouch();
        // Also here temporary to see how timer works.
        if (CurrentState != state_learn) {
            GLCD_SetFont(&Terminal_9_12_6, BlackFont, WhiteBackground);
            GLCD_SetWindow(280, 5, 319, 15);
            GLCD_TextSetPos(0, 0);
            GLCD_print("t:%lu \r\n", timer_counter);
        }

    }
}
