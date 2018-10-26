# This file is part of DAMC-TCK7 Board Support Package
#
# Copyright (c) 2018 Deutsches Elektronen-Synchrotron DESY
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

.main clear

if {[file exists work]} { vdel -lib work -all }

vlib work

vlog -sv ../hdl/tck7_udp_beacon.sv

vlog -sv ./tck7_udp_beacon_tb.sv


vsim work.tck7_udp_beacon_tb

add wave -divider "Clock and reset"
add wave \
    sim:/tck7_udp_beacon_tb/clk \
    sim:/tck7_udp_beacon_tb/reset

add wave -divider "GMII TX"
add wave -radix hex  \
    sim:/tck7_udp_beacon_tb/gmii_tx_en \
    sim:/tck7_udp_beacon_tb/gmii_tx_er \
    sim:/tck7_udp_beacon_tb/gmii_txd

add wave -divider "Internal - state"
add wave \
    sim:/tck7_udp_beacon_tb/DUT/pkt_start
add wave \
    sim:/tck7_udp_beacon_tb/DUT/state

run -all

wave zoom full
