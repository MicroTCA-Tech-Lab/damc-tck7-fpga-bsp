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

`timescale 1ns/1ps

module tck7_udp_beacon_tb;

//==============================================================================

localparam PKT_DELAY_LIM = 1250;

bit clk;
bit reset;

wire         gmii_rx_dv;
wire         gmii_rx_er;
wire   [7:0] gmii_rxd;
wire         gmii_tx_en;
wire         gmii_tx_er;
wire   [7:0] gmii_txd;

//==============================================================================
// 125 MHz clock
always #4 clk = !clk;

initial begin: proc_reset
    reset = 1;
    repeat (10) @(posedge clk);
    reset <= 0;
end

//==============================================================================
tck7_udp_beacon # (
    .PKT_DELAY_LIM(PKT_DELAY_LIM)
) DUT (
    .*
);

//==============================================================================

typedef byte packet_t[$];

// outputs from monitor for checking
time pkt_start [$];
packet_t packets[$];

logic gmii_tx_en_p;
bit in_packet = 0;
packet_t cur_pkt;

always @(posedge clk) begin: proc_monitor
    gmii_tx_en_p <= gmii_tx_en;
    if (!gmii_tx_en_p && gmii_tx_en) begin
        $display("%t [monitor] start of packet", $time);
        pkt_start.push_back($time);
        in_packet = 1;
    end else if (gmii_tx_en_p && !gmii_tx_en) begin
        $display("%t [monitor] end of packet", $time);
        in_packet = 0;
        packets.push_back(cur_pkt);
        cur_pkt.delete();
    end

    if (gmii_tx_en) begin
        cur_pkt.push_back(gmii_txd);
    end
end

//==============================================================================

const realtime EXPECT_PKT_PERIOD = realtime'(PKT_DELAY_LIM*1s)/125_000_000;

const realtime EXPECT_PKT_SIZE = 194;

//==============================================================================
initial begin: proc_main
    $display("%t Testbench starting", $time);

    #(50us);

    $display("%t Testbench finished, performing checks", $time);

    // check time diffs between packets
    for (int i = 1 ; i < pkt_start.size(); i++) begin
        automatic time diff = pkt_start[i] - pkt_start[i-1];
        assert (diff == EXPECT_PKT_PERIOD)
            $display("[check] OK: time diff between packets matches expected");
        else
            $display("[check] ERROR: time diff between packets (%t) does not match expected (%t)",
                diff, EXPECT_PKT_PERIOD);
    end

    // print packets
    $display("=======================================");
    $display("captured packets:");
    for (int i = 0; i < 10; i++) begin
        $write("pkt[%03d] = ", i);
        for (int j = 0; j < packets.size(); j++) begin
            $write("%02x,  ", packets[j][i]);
        end
        $write("\n");
    end

    $write("           ");
    for (int j = 0; j < packets.size(); j++) begin
        $write("...  ");
    end
    $write("\n");

    for (int i = packets[0].size()-10; i < packets[0].size(); i++) begin
        $write("pkt[%03d] = ", i);
        for (int j = 0; j < packets.size(); j++) begin
            $write("%02x,  ", packets[j][i]);
        end
        $write("\n");
    end
    $display("=======================================");

    // check captured packets length
    for (int i = 0; i < packets.size(); i++) begin
        assert (packets[i].size() == EXPECT_PKT_SIZE)
            $display("[check] OK: packet %2d has correct size", i);
        else
            $display("[check] ERROR: size of packet %2d (%3d bytes) does not match expected (%3d bytes)",
                i, packets[i].size(), EXPECT_PKT_SIZE);
    end

    $finish;
end

endmodule
