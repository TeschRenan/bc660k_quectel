/**
	@file bc660k_quectel.h
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
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


#ifndef BC660K_QUECTEL_H
#define BC660K_QUECTEL_H

#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "modem.h"
#include <time.h> 
#include <sys/time.h>


#define RSSI_MAPPED_POINTS 32

using namespace std;

		
class bc660k_quectel {
    
    private:

	int8_t mapped_RSSI[RSSI_MAPPED_POINTS] = { 0,
												1,
												2,
												3,
												4,
												5,
												6,
												7,
												8,
												9,
												10,
												11,
												12,
												13,
												14,
												15,
												16,
												17,
												18,
												19,
												20,
												21,
												22,
												23,
												24,
												25,
												26,
												27,
												28, 
												29,
												30,
												31};	
								
	int32_t value_x_RSSI[RSSI_MAPPED_POINTS] = { 
											-113,
											-111,
											-109, 
											-107,	
											-105,	
											-103,	
											-101,	
											-99,
											-97,
											-95,
											-93,
											-91,
											-89,
											-87,
											-85,
											-83,
											-81,
											-79,
											-77,
											-75,
											-73,
											-71,
											-69,
											-67,
											-65,
											-63,
											-61,
											-59,
											-57,
											-55,
											-53,
											-51};

	modem modemLib;

    static const char* TAG;
    static int32_t msgID;

	void strip_cr_lf_tail(char *str, uint32_t len);

    public:
   
        bc660k_quectel();
        void    getICCID(char* iccid);
        uint8_t setOperator(int mode, int format = 2, const char* oper = " ");
        uint8_t getCurrentDateTime(char* utc);
        uint8_t getOperator(char* mcc);
		int8_t  getRSSI();
		uint8_t setAPN(const char* PDPType,const char* name,const char* user,const char* pass);  
        uint8_t setBAND(int band);
        uint8_t enableNetworkRegister();
        int8_t  enableConnection();
};

#endif
