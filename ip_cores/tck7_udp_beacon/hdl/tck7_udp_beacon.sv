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


module tck7_udp_beacon #(
    parameter ENABLE = 1,
    parameter integer PKT_DELAY_LIM = 125_000_000
) (
    input        clk,
    input        reset,

    input        gmii_rx_dv,
    input        gmii_rx_er,
    input  [7:0] gmii_rxd,
    output       gmii_tx_en,
    output       gmii_tx_er,
    output [7:0] gmii_txd
);

//==============================================================================

logic [31:0] pkt_delay;
wire       pkt_start;

assign pkt_start = (pkt_delay == PKT_DELAY_LIM-1) && ENABLE;

always_ff @(posedge clk) begin: proc_pkt_delay
    if (reset) begin
        pkt_delay <= 0;
    end else begin
        if (pkt_start)  pkt_delay <= 0;
        else            pkt_delay <= pkt_delay + 1;
    end
end

//==============================================================================

logic [7:0] pkt [0:193] = {
    8'h55, 8'h55, 8'h55, 8'h55, 8'h55, 8'h55, 8'h55, 8'hd5, 8'hff, 8'hff, 8'hff,
    8'hff, 8'hff, 8'hff, 8'h00, 8'h22, 8'h33, 8'h44, 8'h12, 8'h34, 8'h08, 8'h00,
    8'h45, 8'h00, 8'h00, 8'ha8, 8'h89, 8'h73, 8'h40, 8'h00, 8'h40, 8'h11, 8'hee,
    8'h3f, 8'hc0, 8'ha8, 8'h01, 8'hea, 8'hff, 8'hff, 8'hff, 8'hff, 8'hbe, 8'h9e,
    8'h30, 8'h39, 8'h00, 8'h94, 8'h02, 8'h0c, 8'h4d, 8'h6f, 8'h69, 8'h6e, 8'h21,
    8'h20, 8'h48, 8'h65, 8'h6c, 8'h6c, 8'h6f, 8'h20, 8'h66, 8'h72, 8'h6f, 8'h6d,
    8'h20, 8'h44, 8'h41, 8'h4d, 8'h43, 8'h2d, 8'h54, 8'h43, 8'h4b, 8'h37, 8'h2c,
    8'h20, 8'h74, 8'h68, 8'h69, 8'h73, 8'h20, 8'h69, 8'h73, 8'h20, 8'h55, 8'h44,
    8'h50, 8'h20, 8'h62, 8'h72, 8'h6f, 8'h61, 8'h64, 8'h63, 8'h61, 8'h73, 8'h74,
    8'h20, 8'h62, 8'h65, 8'h61, 8'h63, 8'h6f, 8'h6e, 8'h20, 8'h72, 8'h75, 8'h6e,
    8'h6e, 8'h69, 8'h6e, 8'h67, 8'h20, 8'h69, 8'h6e, 8'h20, 8'h46, 8'h50, 8'h47,
    8'h41, 8'h20, 8'h61, 8'h6e, 8'h64, 8'h20, 8'h74, 8'h72, 8'h61, 8'h6e, 8'h73,
    8'h6d, 8'h69, 8'h74, 8'h74, 8'h69, 8'h6e, 8'h67, 8'h20, 8'h55, 8'h44, 8'h50,
    8'h20, 8'h70, 8'h61, 8'h63, 8'h6b, 8'h61, 8'h67, 8'h65, 8'h73, 8'h20, 8'h6f,
    8'h76, 8'h65, 8'h72, 8'h20, 8'h47, 8'h69, 8'h67, 8'h61, 8'h62, 8'h69, 8'h74,
    8'h20, 8'h45, 8'h74, 8'h68, 8'h65, 8'h72, 8'h65, 8'h6e, 8'h65, 8'h74, 8'h20,
    8'h6f, 8'h66, 8'h20, 8'h41, 8'h4d, 8'h43, 8'h20, 8'h70, 8'h6f, 8'h72, 8'h74,
    8'h20, 8'h30, 8'h0a, 8'h0a, 8'ha6, 8'h93, 8'hc8
};

// state transitions
enum int unsigned { S_IDLE, S_TX } state;

wire pkt_done;

always_ff @(posedge clk) begin: proc_state
    if (reset) begin
        state <= S_IDLE;
    end else begin
        case (state)
            S_IDLE: if (pkt_start) state <= S_TX;
            S_TX  : if (pkt_done)  state <= S_IDLE;
        endcase
    end
end

// word selector (in packet)
logic [$clog2($size(pkt)+1)-1:0] word_sel;
assign pkt_done = word_sel == $size(pkt)-1;

always_ff @(posedge clk) begin: proc_word_sel
    if (reset) begin
        word_sel <= 'd0;
    end else begin
        if (state == S_TX) word_sel <= word_sel + 'd1;
        else               word_sel <= 'd0;
    end
end

// output
logic       gmii_tx_en_reg;
logic       gmii_tx_er_reg;
logic [7:0] gmii_txd_reg;

assign gmii_tx_en = gmii_tx_en_reg;
assign gmii_tx_er = gmii_tx_er_reg;
assign gmii_txd = gmii_txd_reg;

always_ff @(posedge clk) begin: proc_fsm_output
    case (state)
        S_IDLE: begin
            gmii_tx_en_reg <= 1'b0;
            gmii_tx_er_reg <= 1'b0;
            gmii_txd_reg   <= 8'h00;
        end
        S_TX  : begin
            gmii_tx_en_reg <= 1'b1;
            gmii_tx_er_reg <= 1'b0;
            gmii_txd_reg   <= pkt[word_sel];
        end
    endcase
end

endmodule
