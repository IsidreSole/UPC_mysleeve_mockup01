/*
 * Inventek Systems, LLC (c) Copyright 2016
 * All Rights Reserved.
 *
 * To use you will need the following software:
 * 	Hercules Setup Utility: http://www.hw-group.com/products/hercules/index_en.html
 *
 * 	It will be used as a TCP Client when the application is in Server mode (default)
 * 	and a TCP Server when the application is in Client mode.
 *
 *  Application, Server Mode:
 *   Application:
 *   1. Set the AP_SSID and AP_PASSWORD to your Access Point
 *   2. Build and Run
 *
 *   Hercules:
 *   1. Select the TCP Client tab
 *   2. Set Client to the IP address
 *   3. Set Port to 8002
 *   4. Connect PC to same network as Application is connecting to (See Network Settings below)
 *   5. Wait for "Server Ready"
 *   6. Click "Connect" button
 *   7. Use a "Send" box to send a message to the Application. When the message is received
 *     it will be echoed back to the terminal
 *
 *  Application, Client Mode:
 *   Hercules:
 *   1. Select the TCP Server tab
 *   2. Set Port to 8002
 *   3. In "Server settings" check the "Server echo
 *   3. Click "Listen" button
 *
 *   Application:
 *   1. In Application, locate "Client Settings"
 *   2. Un-comment Client define
 *   3. Set IPADDR to the IP address of the PC connect to the same network in the "Network Settings"
 *   4. Optional, change the "Test Message". Note the message length + 7 can't be larger than the
 *      MAX_MSG_SIZE define below.
 *   5. Build and Run
 *   6. The received message (echoed) from server will be outputed on the debug terminal.
 *
 *-------------------------------------------------------------------------------
 * LICENSE:
 *  THE IWIN PROGRAM IS FREE SOFTWARE FOR INVENTEK CUSTOMERS ONLY.
 *  INVENTEK SYSTEMS RETAINS ALL OWNERSHIP AND INTELLECTUAL PROPERTY RIGHTS
 *  IN THE IWIN CODE ACCOMPANYING THIS MESSAGE AND IN ALL DERIVATIVES HERETO.
 *  USER MAY USE THE IWIN CODE, AND ANY DERIVATIVES CREATED BY ANY PERSON OR
 *  ENTITY BY OR ON USER?S BEHALF, EXCLUSIVELY WITH INVENTEK'S PROPRIETARY
 *  PRODUCTS.
 *
 *  USER AGREES THAT USER IS SOLELY RESPONSIBLE FOR TESTING THE CODE AND
 *  DETERMINING IT?S SUITABILITY.  INVENTEK HAS NO OBLIGATION TO MODIFY, TEST,
 *  CERTIFY, OR SUPPORT THE CODE.
 *
 *  USER ACCEPTANCE AND/OR USE OF THE IWIN CODE CONSTITUTES AGREEMENT TO THE
 *  TERMS AND CONDITIONS OF THIS NOTICE. IWIN CAN BE REDISTRIBUTED AND/OR
 *  MODIFIED BY INVENTEK CUSTOMERS ONLY AND UNDER THE TERMS OF THE GNU GENERAL
 *  PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION, EITHER VERSION
 *  3 OF THE LICENSE, OR (AT USER?S OPTION) ANY LATER VERSION.
 *
 *  THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT
 *  ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS
 *  FOR A PARTICULAR PURPOSE.  REFERENCE THE GNU GENERAL PUBLIC LICENSE FOR MORE
 *  DETAILS.
 *
 *  CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY INVENTEK "AS IS".  NO WARRANTIES,
 *  WHETHER EXPRESSED, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO,
 *  IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE APPLY TO THIS CODE, IT?S INTERACTION WITH INVENTEK 'S
 *  PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 *  USER ACKNOWLEDGES AND AGREES THAT, IN NO EVENT, SHALL INVENTEK BE LIABLE,
 *  WHETHER IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF
 *  STATUTORY DUTY), STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE,
 *  FOR ANY INDIRECT, SPECIAL, PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL
 *  LOSS, DAMAGE, FOR COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE CODE,
 *  HOWSOEVER CAUSED, EVEN IF INVENTEK HAS BEEN ADVISED OF THE POSSIBILITY OR THE
 *  DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW, INVENTEK 'S
 *  TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS CODE, SHALL NOT
 *  EXCEED THE PRICE PAID DIRECTLY TO INVENTEK SPECIFICALLY TO HAVE THIS CODE
 *  DEVELOPED.
 *
 *  USER SHOULD HAVE RECEIVED A COPY OF THE GNU GENERAL PUBLIC LICENSE ALONG
 *  WITH THIS PROGRAM. IF NOT, SEE <HTTP://WWW.GNU.ORG/LICENSES/>.
 *-------------------------------------------------------------------------------
 */
#ifndef ESWIFI_APP_H_
#define ESWIFI_APP_H_
/*
 * Needed System includes
 */
#include <stdio.h>
#include <string.h>

/*
 * eS-WiFi defines for control and message responses
 */
#define ESWIFI_TRUE 			0x01			// used for checking a TRUE relationship
#define ESWIFI_FALSE 			!ESWIFI_TRUE	// used for checking a FALSE relationship
#define ESWIFI_SUCCESS	 		0x00			// used to indicate a matched relationship
#define ESWIFI_FAIL				-1				// flag for Failure
#define ESWIFI_TIMEOUT			-2				// flag for Timeout

/*
 * eS-WiFi defines for printing debug messages on the Nuleco's main UART
 */
#define ESWIFI_DEBUG_ECHO	  	ESWIFI_FALSE
#define ESWIFI_DEBUG			ESWIFI_TRUE

/*
* Network Settings for AP Access
*
* set ESWIFI_AP_SSID to your AP SSID
* set ESWIFI_AP_PASSWORD to your AP Password
*/
#define ESWIFI_AP_SSID			"MIWIFI_2G_Qbcj"		//Access Point Name
#define ESWIFI_AP_PASSWORD		"t4EWENYJ"		//Access Point Password
//#define ESWIFI_AP_SSID			"XQ5H3"		//Access Point Name
//#define ESWIFI_AP_PASSWORD		"ThisIsMyHomeNetwork1"		//Access Point Password

/*
 * Set the Maximum number of characters in an outgoing message (must include command)
 */
#define ESWIFI_MAX_MSG_SIZE	(128)

/*
* Network Settings for TCP Communications
*
* uncomment #define CLIENT below to enable Client Mode
* set ESWIFI_IPADDR to your Remote Server IP Address
* set ESWIFI_PORT to your Remote Server TCP Port
*/
#define CLIENT											//Un-comment for Client mode
#define ESWIFI_IPADDR			"192.168.1.14"			//Remote IP Address
#define ESWIFI_PORT				"8002"					//Remote Port

/*
 * Set up UART buffers used to communication with the eS-WiFi
 */
#define ESWIFI_UART_RX_BUFFSIZE 256						// set the number of UART buffer bytes
struct esWifiCircularBuffer
{
	uint8_t esWifiRxBuffer[ESWIFI_UART_RX_BUFFSIZE];	// buffer for UART es-WiFi communication
	uint16_t tail; 										// store data at tail
	uint16_t head; 										// read data from head
};
struct esWifiCircularBuffer esWiFiRxData;				// init UART buffers

/*
 * eS-WiFi RX buffer is used for parsing eS-WiFi AT Command responses
 */
struct WIFI_RX_PACKET{
	uint16_t length;									// number of data items in buffer
	uint8_t buffer[ESWIFI_UART_RX_BUFFSIZE];			// set up RX buffer
} WIFI_RX_PACKET;
struct WIFI_RX_PACKET ESWIFI_RX;								// init ESWIFI_RX Parse buffer

/*
 * Set Application Buffers
 */
#define ESWIFI_APP_BUF_SIZE	(512)						// maximum number of byte for buffer
struct APP_RXTX_BUF{
	uint8_t rxData[ESWIFI_APP_BUF_SIZE];				// setup RX Data buffer
	uint8_t payload[ESWIFI_APP_BUF_SIZE];				// setup Payload buffer
	uint8_t txData[ESWIFI_APP_BUF_SIZE+10];				// setup TX Data buffer plus command termination characters
};
struct APP_RXTX_BUF eswifi_app_buf;							// init eS-WiFi application buffer

/*
 * Sent AT Command or message pointed to by data to eS-WiFi Module
 */
void esWifiTx(int8_t *data);

/*
 * Purge the RX buffers associated with the eS-WiFi Module
 */
void esWifiFlushUart(void);

/*
 * Purge eS-WiFi AT Command Parse buffer
 */
void esWifiFlushBuff(void);

/*
 * Read a byte from the eS-WiFi's UART RX buffer with timeout
 */
int8_t esWifiRx(uint8_t *rxBuff, uint16_t timeOut);

/*
 * Parse the eS-WiFi UART RX buffer for message pointed to by message
 */
int8_t esWifiParse(int8_t* message);

/*
 * use the main Nucleo UART for debug print messages
 */
void dbgTx(int8_t *data);

#endif /* ESWIFI_APP_H_ */
