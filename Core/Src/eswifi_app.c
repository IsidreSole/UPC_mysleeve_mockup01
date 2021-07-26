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
/*
 * Needed System Includes
 */
#include "stm32f4xx_hal.h"
#include "eswifi_app.h"
#include <stdio.h>
#include <string.h>

/*
 * Flag used to signal that an AT Command has been detected in the UART RX from the eS-WiFi
 */
uint8_t esWifiRxFlag = 0;
//extern struct esWifiCircularBuffer esWiFiRxData;

/*
 * external protocol types for using the Nucleo's UARTs
 */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/*
 * print a debug message pointed to by data to the Nucleo's main UART
 */
void dbgTx(int8_t * data)
{
	while (*data)
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)data++, 1, 0xFFFF);
	}
}

/*
 * Sent AT Command or message pointed to by data to eS-WiFi Module
 */
void esWifiTx(int8_t * data)
{
	static HAL_StatusTypeDef ret = HAL_OK;

		ret = HAL_UART_Transmit(&huart1, (uint8_t *) data , 1, 0xFFFF);

}

/*
 * Purge the RX buffers associated with the eS-WiFi Module
 */
void esWifiFlushUart(void)
{
	esWiFiRxData.tail = 0;
	esWiFiRxData.head = 0;
}

/*
 * Purge eS-WiFi AT Command Parse buffer
 */
void esWifiFlushBuff(void)
{
	memset(ESWIFI_RX.buffer,'\0',ESWIFI_RX.length);
	ESWIFI_RX.length = 0;
}

/*
 * Read a byte from the eS-WiFi's UART RX buffer with timeout
 */
int8_t esWifiRx(uint8_t *rxBuff, uint16_t timeOut)
{
	static uint8_t termCharDetect = 0;
	static HAL_StatusTypeDef ret = HAL_OK;
	char aux[32];

	/* read until time out */
	do
	{

		ret = HAL_UART_Receive_IT(&huart1,aux, 1);   //  Aqui tindras que posar el buffer i una longitud de 1 caracter.

		if (ret == HAL_OK){
			esWiFiRxData.esWifiRxBuffer[esWiFiRxData.tail++] = aux;

		}
		/* check for serial data available */
		if(esWiFiRxData.head != esWiFiRxData.tail)
		{
			/* serial data available, so return data to user */
			*rxBuff = esWiFiRxData.esWifiRxBuffer[esWiFiRxData.head++];

			/* check for ring buffer wrap */
			if (esWiFiRxData.head >= ESWIFI_UART_RX_BUFFSIZE)
			{
				/* ring buffer wrap, so reset head pointer to start of buffer */
				esWiFiRxData.head = 0;
			}

			ESWIFI_RX.buffer[ESWIFI_RX.length++] = *rxBuff;

			if (*rxBuff == '>')
			{
				if (termCharDetect == 0)
				{
					termCharDetect = 1;
				}
			}

			if (*rxBuff == ' ')
			{
				if (termCharDetect == 1)
				{
					termCharDetect = 2;
				}
				else
				{
					termCharDetect = 0;
				}
			}
			if (termCharDetect == 2)
			{
				{
					esWifiRxFlag = ESWIFI_TRUE;
					termCharDetect = 0;
					return SUCCESS;
				}
			}

			return SUCCESS;
		}
	}while(timeOut--);

	return ESWIFI_FAIL;
}

/*
 * Parse the eS-WiFi UART RX buffer for message pointed to by message
 */
int8_t esWifiParse(int8_t *message)
{
	uint8_t rxBuffDummy;
	uint16_t size;

	memset(eswifi_app_buf.rxData,'\0', ESWIFI_APP_BUF_SIZE);
	ret = HAL_UART_Receive_IT(&huart1,aux, 1);   //  Aqui tindras que posar el buffer i una longitud de 1 caracter.
	while(!esWifiRxFlag)
        HAL_Delay(10);

	if (esWifiRxFlag)
	{
		size = ESWIFI_RX.length;
		memcpy(eswifi_app_buf.rxData, ESWIFI_RX.buffer, size);
		esWifiRxFlag = ESWIFI_FALSE;

		/* ASCII Data */
		if(strstr((char*)eswifi_app_buf.rxData,(char *)message) != NULL)
		{
			esWifiFlushBuff();
			return ESWIFI_SUCCESS;
		}
		else
		{
			esWifiFlushBuff();
			return ESWIFI_FAIL;
		}
	}

	// Clean up after processing message
	// Message error -- it can be removed
	esWifiFlushBuff();
	return ESWIFI_TIMEOUT;
}

/************************ (C) COPYRIGHT Inventek Systems *****END OF FILE****/
