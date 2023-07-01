/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include "rpu-main.h"

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>
#include "xil_io.h"
#include "xil_cache.h"
#include "xil_exception.h"

#define SMMU_NSCR0 0xFD800400
#define SMMU_SCR0 0xFD800000
#define SMMU_SCR1 0xFD800004
#define SMMU_ISR0 0xFD5F0010
#define SMMU_SGFAR_low 0xFD800040
#define SMMU_SGFAR_high 0xFD800044
#define SMMU_SGFSYNR0 0xFD800050
#define SMMU_SGFSYNR1 0xFD800054
#define SMMU_NSGFAR_low 0xFD800440
#define SMMU_NSGFAR_high 0xFD800444

#define LPD_SLCR_SECURE 0xFF4B0020 // rpu trustzone settings
#define RPU0_CFG 0xFF9A0100

// coherent and non-coherent
#define COHERENT 1 // 1: coherent, 0: non-coherent

// not used
void readSMMUErr(){

	u16 regValShort = 0x0;
	u32 regVal = 0x0;

	// read SGFAR
	regValShort = Xil_In16(SMMU_SGFAR_low);
	xil_printf("The value of SMMU_SGFAR_low is: 0x%u\n\n\r", regValShort);

	regValShort = Xil_In16(SMMU_SGFAR_high);
	xil_printf("The value of SMMU_SGFAR_high is: 0x%u\n\n\r", regValShort);

	// read SGFSYNR0
	regValShort = Xil_In16(SMMU_SGFSYNR0);
	xil_printf("The value of SMMU_SGFSYNR0 is: 0x%u\n\n\r", regValShort);

	// read SGFSYNR1
	regValShort = Xil_In16(SMMU_SGFSYNR1);
	xil_printf("The value of SMMU_SGFSYNR1 is: 0x%u\n\n\r", regValShort);

	// read SMMU_NSGFAR_low
	regValShort = Xil_In16(SMMU_NSGFAR_low);
	xil_printf("The value of SMMU_NSGFAR_low is: 0x%u\n\n\r", regValShort);

	// read SMMU_NSGFAR_low
	regVal = Xil_In32(SMMU_NSGFAR_high);
	xil_printf("The value of SMMU_NSGFAR_high is: 0x%lX\n\n\r", regVal);
}

int main()
{
	init_platform();
	u32 regVal = 0x0;

	xil_printf(" RPU bare-metal application \n\n\r");

	if (COHERENT == 1) {
		xil_printf("COHERENT\n\n\r");
		Xil_Out32(RPU0_CFG, 0x00000002U);
		regVal = Xil_In32(RPU0_CFG);
		xil_printf("The value of RPU0_CFG is: 0x%"PRIX32"\n\n\r", regVal);
	}
	else if (COHERENT == 0){
		xil_printf("NON-COHERENT\n\n\r");
		Xil_Out32(RPU0_CFG, 0x00000000U);
	}

	// SECURE TRANSACTION TO SMMU config, this will set the SMMU to block any transaction
	Xil_Out32(SMMU_SCR0, 0x00000402); // this is enough to generate interrupt GFIE active and USFCGFG active

	// NOTE: in case of coherency the program blocks at this point

	// write to DDR Memory
	xil_printf("Writing...\n\n\r");
	Xil_Out32(RPU_DDR_LOW_S_BASE, 0xFFFFFFFF);

	// read to DDR Memory
	regVal = Xil_In32(RPU_DDR_LOW_S_BASE);
	xil_printf("The value of RPU_DDR_LOW_S_BASE is NOW 0x%"PRIX32"\n\n\r", regVal);

	cleanup_platform();
    return 0;
}
