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

`timescale 1ns / 1ps

module tck7_sfp_ibert (
    output wire [7 : 0] TXN_O,
    output wire [7 : 0] TXP_O,
    input  wire [7 : 0] RXN_I,
    input  wire [7 : 0] RXP_I,
    input  wire [1 : 0] GTREFCLK0P_I,
    input  wire [1 : 0] GTREFCLK0N_I,
    input  wire [1 : 0] GTREFCLK1P_I,
    input  wire [1 : 0] GTREFCLK1N_I,
    input  wire SYSCLKP_I
);

//
// Ibert refclk internal signals
//
wire   [1:0] gtrefclk0_i;
wire   [1:0] gtrefclk1_i;
wire   [1:0] refclk0_i;
wire   [1:0] refclk1_i;
wire         sysclk_i;

//
// Refclk IBUFDS instantiations
//

IBUFDS_GTE2 u_buf_q2_clk0
  (
    .O            (refclk0_i[0]),
    .ODIV2        (),
    .CEB          (1'b0),
    .I            (GTREFCLK0P_I[0]),
    .IB           (GTREFCLK0N_I[0])
  );

IBUFDS_GTE2 u_buf_q2_clk1
  (
    .O            (refclk1_i[0]),
    .ODIV2        (),
    .CEB          (1'b0),
    .I            (GTREFCLK1P_I[0]),
    .IB           (GTREFCLK1N_I[0])
  );
IBUFDS_GTE2 u_buf_q3_clk0
  (
    .O            (refclk0_i[1]),
    .ODIV2        (),
    .CEB          (1'b0),
    .I            (GTREFCLK0P_I[1]),
    .IB           (GTREFCLK0N_I[1])
  );

IBUFDS_GTE2 u_buf_q3_clk1
  (
    .O            (refclk1_i[1]),
    .ODIV2        (),
    .CEB          (1'b0),
    .I            (GTREFCLK1P_I[1]),
    .IB           (GTREFCLK1N_I[1])
  );

//
// Refclk connection from each IBUFDS to respective quads depending on the source selected in gui
//
assign gtrefclk0_i[0] = refclk0_i[0];
assign gtrefclk1_i[0] = refclk1_i[0];
assign gtrefclk0_i[1] = refclk0_i[1];
assign gtrefclk1_i[1] = refclk1_i[1];



//
// Sysclock connection
//
assign sysclk_i = SYSCLKP_I;


//
// IBERT core instantiation
//
ibert_7series_gtx_0 u_ibert_core (
  .TXN_O(TXN_O),
  .TXP_O(TXP_O),
  .RXN_I(RXN_I),
  .RXP_I(RXP_I),
  .SYSCLK_I(sysclk_i),
  .GTREFCLK0_I(gtrefclk0_i),
  .GTREFCLK1_I(gtrefclk1_i)
);

endmodule
