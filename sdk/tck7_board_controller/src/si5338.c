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


/**
 * Si5338 management
 *
 * all public functions should accept I2C address as argument; there are more
 * Si5338 on TCK7
 */

#include <xil_types.h>
#include <xil_printf.h>
#include <xstatus.h>

#include "iic.h"

#define code

//#include "si5338/register_map_312_5M.h"
//#include "si5338/register_map_156_25M.h"
#include "si5338/register_map_156_25M_and_125M_on_clk0.h"

#undef code

// keep this updated according to the above include
#define SI5338_FREQ_STR "156.25 MHz on clk1, clk2 and 125 MHz on clk0"


#define DEBUG 0

#define debug_print(...) \
            do { if (DEBUG) xil_printf("[si5338] " __VA_ARGS__); } while (0)

/** only set register address - used for reading from that register */
static int si5338_write_reg_addr(uint8_t reg_addr) {
	u8 wr_buf[8];
	wr_buf[0] = reg_addr;

	return iic_write(wr_buf, 1);
}

/** write to Si5338 register */
static int si5338_write_reg(uint8_t reg_addr, uint8_t data) {
	u8 wr_buf[8];

	debug_print("si5338_write_reg(reg_addr=%d, data=0x%x)\r\n", reg_addr, data);

	wr_buf[0] = reg_addr;
	wr_buf[1] = data;

	return iic_write(wr_buf, 2);
}

/** read from Si5338 register */
static int si5338_read_reg(u8 reg_addr, u8 *data){
	int rc;

	debug_print("si5338_read_reg(reg_addr=%d)\r\n", reg_addr);

	rc = si5338_write_reg_addr(reg_addr);
	if (rc != XST_SUCCESS) {
		return XST_FAILURE;
	}

	rc = iic_read_data(data);
	if (rc != XST_SUCCESS) {
		return XST_FAILURE;
	}

	debug_print("  success, data=0x%x\r\n", *data);
	return XST_SUCCESS;
}

/** based on
 * Si5338 REFERENCE MANUAL: CONFIGURING THE Si5338 WITHOUT CLOCKBUILDER DESKTOP
 * 10.3. Register Write-Allowed Mask
 */
int si5338_conf(u8 iic_addr){
	int rc;
	u8 read_buffer;

	iic_set_slave_addr(iic_addr);
	debug_print("si5338_conf(iic_addr=0x%x)\r\n", iic_addr);
	xil_printf("[si5338] conf osc at 0x%02x with out freq: %s\r\n", iic_addr, SI5338_FREQ_STR);

	for (int i = 0; i < sizeof(Reg_Store)/sizeof(Reg_Store[0]); i++){
		uint8_t addr = Reg_Store[i].Reg_Addr;
		uint8_t val  = Reg_Store[i].Reg_Val;
		uint8_t mask = Reg_Store[i].Reg_Mask;

		if (mask == 0) {
			continue;
		}

		if (mask == 0xFF) {
			debug_print("si5338_conf: direct write: addr = %02x, val = %02x\r\n", addr, val);
			rc = si5338_write_reg(addr, val);
			if (rc != XST_SUCCESS) {
				xil_printf("[si5338] si5338_conf: si5338_write_reg() failed\r\n");
				return rc;
			}
		} else {
			debug_print("si5338_conf: read write modify: "
					"addr = %02x, val = %02x, mask = %02x\r\n", addr, val, mask);
			rc = si5338_read_reg(addr, &read_buffer);
			if (rc != XST_SUCCESS) {
				xil_printf("[si5338] si5338_conf: si5338_read_reg() failed\r\n");
				return rc;
			}

			u8 clear_curr_val = read_buffer & (~mask);
			u8 clear_new_val =  val & mask;
			u8 combined = clear_curr_val | clear_new_val;

			rc = si5338_write_reg(addr, combined);
			if (rc != XST_SUCCESS) {
				xil_printf("[si5338] si5338_conf: si5338_write_reg() failed\r\n");
				return rc;
			}
		}
	}
	return XST_SUCCESS;
}

int si5338_soft_reset(u8 iic_addr, int reset){
	int rc;

	debug_print("si5338_soft_reset(iic_addr=0x%x, reset=%d)\r\n", iic_addr, reset);

	iic_set_slave_addr(iic_addr);
	rc = si5338_write_reg(246, reset ? 0x2 : 0);
	if (rc != XST_SUCCESS) {
		xil_printf("[si5338] si5338_soft_reset: si5338_write_reg() failed\r\n");
		return rc;
	}

	return XST_SUCCESS;
}

int si5338_reg_dump(u8 iic_addr) {
	int rc;
	u8 read_buffer;

	iic_set_slave_addr(iic_addr);
	xil_printf("Si5338 at addr: 0x%x\r\n", iic_addr);
	xil_printf("  out freq: %s\r\n", SI5338_FREQ_STR);

	u8 addrs[] = {0, 2, 3, 4, 5, 27, 31, 218};
	const char* reg_names[] = {"Rev ID", "id 1", "id 2", "id 3", "id 4", "i2c addr", "clk divs", "PLL status"};

	for (int i = 0; i < sizeof(addrs)/sizeof(addrs[0]); i++){
		u8 addr = addrs[i];
		rc = si5338_read_reg(addr, &read_buffer);
		if (rc != XST_SUCCESS) {
			xil_printf("[si5338] si5338_reg_dump: si5338_read_reg() failed\r\n");
			return rc;
		}
		xil_printf("  at addr %02x = %02x (reg name: %s)\r\n", addr, read_buffer, reg_names[i]);
	}

	return XST_SUCCESS;
}
