
/**
	@file bc660k_quectel.cpp
	@author Renan Tesch

	MIT License

	Copyright (c) Copyright 2023 Renan Tesch
	GitHub https://github.com/TeschRenan

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABbc660k_quectelILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#include "bc660k_quectel.h"

bc660k_quectel::bc660k_quectel(){

}

const char* bc660k_quectel::TAG = "[BC660K QUECTEL]";

/**
 * @brief Remove the \r\n.
**/
void bc660k_quectel::strip_cr_lf_tail(char *str, uint32_t len)
{
    if (str[len - 2] == '\r') {
        str[len - 2] = '\0';
    } else if (str[len - 1] == '\r') {
        str[len - 1] = '\0';
    }
}

/**
 * @brief Obtain the current ICCID.
 * @param  [iccid]: Returned in paramter the current ICCID.
**/
void bc660k_quectel::getICCID(char* iccid){

	char temp[256] = {0};

	modemLib.write("AT+QCCID\r\n");
	
	if(modemLib.getResponse(temp,"+QCCID:",sizeof(temp),10000) != 0)	{

		ESP_LOGW(TAG, "AT+QCCID RESPONSE ---> %s",temp);
		
	}

	if ((temp != NULL) && (temp[0] != '\0')) {

		sscanf(temp, "%*s%s%*s", iccid);

		int len = snprintf(iccid, 21, "%s", iccid);
        if (len > 2) {
            /* Strip "\r\n" */
            strip_cr_lf_tail(iccid, len);

        }
	
	}

}

/**
 * @brief Set as connect automatic our manual for operator.
 * @param  [mode]: 0 = automatic and 1 for Manual operator selection.
 * @param  [format]: 	0 Long format alphanumeric <oper>
						1 Short format alphanumeric <oper>
						2 Numeric <oper>. Only valid when <mode>=1 or <mode>=4 
						default = 2
* @param  [oper]: MCC for operator, default "72403"					
 * @return [0]: Fail to send command.
 * @return [1]: Success to send command.
**/
uint8_t bc660k_quectel::setOperator(int mode, int format = 2, char* oper = "72403"){

	uint8_t returnCode = 0;

	char temp[256] = {0};

	if(mode == 1){

		sprintf(temp,"%s%d,%d,\"%s\"%s","AT+COPS=",mode,format,oper,"\r\n");

	}
	else{

		sprintf(temp,"%s%s","AT+COPS=0","\r\n");
	}	

	modemLib.flush();

	//Configure Parameters of MCC

	if(modemLib.atCmdWaitResponse(temp,"OK",NULL,-1, 5000, NULL, 0 ) != 0){

		ESP_LOGW(TAG, "Set Operator Ok");

		returnCode = 1;

	}
	else{

		ESP_LOGW(TAG, "Set Operator Fail");

		returnCode = 0;
	}

	return returnCode;

}

/**
 * @brief Get the current modem Date Time and Send to ESP RTC.
 * @param [utc] use to define current utc, example Brazil UTC = -3
 * @return [0]: Fail to send command.
 * @return [1]: Success to send command.
**/
uint8_t bc660k_quectel::getCurrentDateTime(char* utc){

	uint32_t year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

	int8_t returnCode = 0;

	char temp[256] = {0};

	modemLib.write("AT+CCLK?\r\n");

	if(modemLib.getResponse(temp,"+CCLK",sizeof(temp),10000) != 0)	{

		ESP_LOGW(TAG, "AT+CCLK? RESPONSE ---> %s",temp);

		sscanf(temp, "%*s%d/%d/%d,%d:%d:%d%*s", &year, &month, &day , &hour , &minute , &second);

		ESP_LOGW(TAG,"%d/%d/%d,%d:%d:%d", year, month, day , hour , minute , second);
		
	}

    if(year != 0 ){

        struct tm tm;
        struct timeval stv;
        time_t date;

		
		tm.tm_year = year + 100;

		tm.tm_mon = (month - 1);

		tm.tm_mday = day;

		tm.tm_hour = hour + atoi(utc);

		tm.tm_min = minute;

		tm.tm_sec = second;

		date = mktime(&tm);

		stv.tv_sec = date;
		stv.tv_usec = 0;
		settimeofday(&stv, NULL);

		returnCode = 1;

    }
	else{

		returnCode = 0;
	}

	return returnCode;
}

/**
 * @brief Obtain the current operator MCC.
 * @param  [mccc]: Returned in paramter the current MCC.
**/
uint8_t bc660k_quectel::getOperator(char* mcc){

	char temp[256] = {0};

	modemLib.write("AT+COPS?\r\n");

	if(modemLib.getResponse(temp,"+COPS:",sizeof(temp),10000) != 0)	{

		ESP_LOGW(TAG, "AT+COPS? RESPONSE ---> %s",temp);
		
		if ((temp != NULL) && (temp[0] != '\0')) {

		mcc = strstr(temp, ",\"");
		
		if (mcc) {
			mcc += 2;
			char *s = strchr(mcc, '\"');
			if (s) *s = 0;
			return 1;
			
		}
		}
		else{
	
			ESP_LOGW(TAG, "Operator Name Empty");
		
		}

	}

	return 0;

}

/**
 * @brief Obtain the current RSSI.
 * @return Is returned the current RSSI.
**/
int8_t bc660k_quectel::getRSSI(){

	int RSSI = 0;

	char temp[256] = {0};

	modemLib.write("AT+CSQ\r\n");
	
	if(modemLib.getResponse(temp,"+CSQ:",sizeof(temp),10000) != 0)	{

		ESP_LOGW(TAG, "AT+CSQ RESPONSE ---> %s",temp);
		
	}
	
	if ((temp != NULL) && (temp[0] != '\0')) {

		sscanf(temp, "%*s%d,%*s", &RSSI);

		for(int i = 0; i < RSSI_MAPPED_POINTS; i++){

			if(RSSI == mapped_RSSI[i]){

				RSSI = value_x_RSSI[i];

				ESP_LOGW(TAG, "RSSI %i", RSSI);
				
				break;
			}

		}
	
	}

	return RSSI;

}

/**
 * @brief Set APN to connect in the network.
 * @param  [PDPType]: Set normaly "IP"  Internet Protocol (IETF STD 5)
	"IPV6" Internet Protocol version 6 (IETF RFC 2460)
	"IPV4V6" Dual IP stack (see 3GPP TS 24.301)
	"Non-IP" Transfer of Non-IP data to external packet network (see 3GPP TS 24.301)in Returned in paramter the current MCC.
 * @param  [name]: APN Name
 * @param  [name]: APN User
 * @param  [name]: APN Pass
 * @return [0]: Fail to send command.
 * @return [1]: Success to send command.
**/
uint8_t bc660k_quectel::setAPN(const char* PDPType,const char* name,const char* user,const char* pass){

	uint8_t returnCode = 0;

	char temp[256] = {0};

	if(strlen(user) != 0 && strlen(pass) != 0){

		sprintf(temp,"%s\"%s\",\"%s\",\"%s\",\"%s\"%s","AT+QCGDEFCONT=",PDPType,name,user,pass,"\r\n");

	}
	else{

		sprintf(temp,"%s\"%s\",\"%s\"%s","AT+QCGDEFCONT=",PDPType,name,"\r\n");

	}

	modemLib.flush();

	//Configure Parameters of APN

	if(modemLib.atCmdWaitResponse(temp,"OK",NULL,-1, 5000, NULL, 0 ) != 0){

		ESP_LOGW(TAG, "Set APN Ok");

		returnCode = 1;

	}
	else{

		ESP_LOGW(TAG, "Set APN Fail");

		returnCode = 0;
	}

	return returnCode;

}

/**
 * @brief Set band to modem running.
 * @param [band] use to define band to attach 
 * [band] = 0 all bands
 * [band] = 1 band 3 and 28
 * @return [0]: Fail to send command.
 * @return [1]: Success to send command.
**/
uint8_t bc660k_quectel::setBAND(int band){

	uint8_t returnCode = 0;

	char temp[256] = {0};

	modemLib.flush();

	if(band == 0){

		sprintf(temp,"%s","AT+QBAND=0\r\n");

	}
	if(band == 1){

		sprintf(temp,"%s","AT+QBAND=2,3,28\r\n");

	}

	if(modemLib.atCmdWaitResponse(temp,"OK",NULL,-1, 5000, NULL, 0 ) != 0){


		ESP_LOGW(TAG, "Set BAND Ok");
	

		returnCode = 1;

	}
	else{


		ESP_LOGW(TAG, "Set BAND Fail");
	

		returnCode = 0;
	}

	return returnCode;

}

/**
 * @brief Enable register
 * @return [0]: Fail to send command.
 * @return [1]: Success to send command.
**/
uint8_t bc660k_quectel::enableNetworkRegister(){

	int8_t returnCode = 0;

	modemLib.flush();

	if(modemLib.atCmdWaitResponse("AT+CEREG=1\r\n","OK",NULL,-1, 5000, NULL, 0 ) != 0){


		ESP_LOGW(TAG, "Enable Network Register Ok");
	

		returnCode = 1;

	}
	else{


		ESP_LOGW(TAG, "Enable Network Register Fail");
	

		returnCode = 0;
	}

	return returnCode;
}

/**
 * @brief Enable connection
 * @return [0]: Fail to send command.
 * @return [1]: Success to send command.
**/
int8_t bc660k_quectel::enableConnection(){

	int8_t returnCode = 0;

	modemLib.flush();

	if(modemLib.atCmdWaitResponse("AT+CSCON=1\r\n","OK",NULL,-1, 5000, NULL, 0 ) != 0){


		ESP_LOGW(TAG, "Enable Connection Ok");
	

		returnCode = 1;

	}
	else{


		ESP_LOGW(TAG, "Enable Connection Fail");
	

		returnCode = 0;
	}

	return returnCode;
}
