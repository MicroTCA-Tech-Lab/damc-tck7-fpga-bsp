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
#include <xil_printf.h>

#include "user_cmd.h"
#include "si5338.h"
#include "version.h"

static char recv_buf[128];
static int recv_pos = 0;

#define CMD_TERM "\r\n"

// prototype for command
typedef void (*cmd_func_t)(const char *rx_buf);

// list entry
typedef struct {
	const char *cmd;
	cmd_func_t func;
	const char *desc;
} cmd_entry_t;

static void ver_func(const char *rx_buf);
static void help_func(const char *rx_buf);
static void si_func(const char *rx_buf);

// command list
cmd_entry_t cmds[] = {
	{"version", ver_func, "Prints version"},
	{"help",    help_func, "Prints list of all commands"},
	{"si",      si_func, "Print SiLabs oscillator registers"},
};

static void ver_func(const char *rx_buf) {
	xil_printf("version: " VERSION_STR "\r\n");
}

static void help_func(const char *rx_buf) {
	xil_printf("available commands:\r\n");
	for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
		xil_printf("  %s: %s\r\n", cmds[i].cmd, cmds[i].desc);
	}
}

static void si_func(const char *rx_buf) {
	si5338_reg_dump(SI5338_ADDR_0);
	si5338_reg_dump(SI5338_ADDR_1);
}

void user_cmd_append(char *rx_buf, int len) {
	if (len+recv_pos > sizeof(recv_buf)) {
		// we would overflow the buffer, clear the buffer and return error msg
		recv_pos = 0;
		xil_printf("unknown command (command too long)\r\n");
	} else {
		memcpy(&recv_buf[recv_pos], rx_buf, len);
		recv_pos += len;

		if (strstr(recv_buf, CMD_TERM) != 0) {
			// traverse all commands, search for match
			for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
				if (strstr(recv_buf, cmds[i].cmd) == recv_buf) {
					cmds[i].func(rx_buf);
					break;
				}
			}

			// clear buf
			recv_pos = 0;
			memset(recv_buf, 0, sizeof(recv_buf));
		}
	}
}
