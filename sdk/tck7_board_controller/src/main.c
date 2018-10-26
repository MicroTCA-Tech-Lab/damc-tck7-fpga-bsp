/*
 * This file is part of DAMC-TCK7 Board Support Package
 *
 * Copyright (c) 2018 Deutsches Elektronen-Synchrotron DESY
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <string.h>
#include <unistd.h>
#include <xil_printf.h>
#include <xparameters.h>
#include <xstatus.h>
#include <xuartlite.h>

#include "platform.h"
#include "user_cmd.h"
#include "iic.h"
#include "si5338.h"
#include "version.h"

int main()
{
	XUartLite uart;
	int rc;
	unsigned char rx_buf[16] = {0};

	init_platform();

	xil_printf("### DAMC-TCK7 Board Controller (MicroBlaze) ###\r\n");
	xil_printf("### version: " VERSION_STR
			                   "                            ###\r\n");

	rc = XUartLite_Initialize(&uart, XPAR_MB_AXI_UARTLITE_0_DEVICE_ID);
	if (rc != XST_SUCCESS) {
		xil_printf("XUartLite_Initialize() error, stopping...\r\n");
		while(1);
	}

	rc = iic_init();
	if (rc != XST_SUCCESS) {
		xil_printf("iic_init() error, stopping...\r\n");
		while(1);
	}

	xil_printf("# Si5338 initialization\r\n");
	uint8_t si_addrs[] = {SI5338_ADDR_0, SI5338_ADDR_1};

	for (int i = 0; i < sizeof(si_addrs)/sizeof(*si_addrs); i++) {
		uint8_t si_addr = si_addrs[i];
		si5338_reg_dump(si_addr);
		si5338_conf(si_addr);
		si5338_soft_reset(si_addr, 1);
	}
	usleep(100000);
	for (int i = 0; i < sizeof(si_addrs)/sizeof(*si_addrs); i++) {
		uint8_t si_addr = si_addrs[i];
		si5338_soft_reset(si_addr, 0);
		si5338_reg_dump(si_addr);
	}


	xil_printf("Initialization done, entering while(1) loop...\r\n");

	while(1) {
		rc = XUartLite_Recv(&uart, rx_buf, sizeof(rx_buf));
		if (rc > 0) {
			xil_printf("%s", rx_buf);
			user_cmd_append((char*)rx_buf, rc);
			memset(rx_buf, 0, sizeof(rx_buf));
		}
		usleep(1000);
	}

	cleanup_platform();
	return 0;
}
