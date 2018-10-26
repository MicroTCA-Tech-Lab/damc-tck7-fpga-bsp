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


set_property PACKAGE_PIN V21 [get_ports clk_50]
set_property IOSTANDARD LVCMOS33 [get_ports clk_50]

set_property PACKAGE_PIN H21 [get_ports fpga_reset_n]
set_property IOSTANDARD LVCMOS15 [get_ports fpga_reset_n]

set_property PACKAGE_PIN F5 [get_ports {pcie_clk_clk_n[0]}]
set_property PACKAGE_PIN F6 [get_ports {pcie_clk_clk_p[0]}]

set_property LOC GTXE2_CHANNEL_X0Y20 [get_cells {system_i/xdma_0/inst/system_xdma_0_0_pcie2_to_pcie3_wrapper_i/pcie2_ip_i/U0/inst/gt_top_i/pipe_wrapper_i/pipe_lane[0].gt_wrapper_i/gtx_channel.gtxe2_channel_i}]
set_property PACKAGE_PIN G3 [get_ports {pcie_7x_mgt_rtl_0_rxn[0]}]
set_property PACKAGE_PIN G4 [get_ports {pcie_7x_mgt_rtl_0_rxp[0]}]
set_property PACKAGE_PIN D1 [get_ports {pcie_7x_mgt_rtl_0_txn[0]}]
set_property PACKAGE_PIN D2 [get_ports {pcie_7x_mgt_rtl_0_txp[0]}]
set_property LOC GTXE2_CHANNEL_X0Y21 [get_cells {system_i/xdma_0/inst/system_xdma_0_0_pcie2_to_pcie3_wrapper_i/pcie2_ip_i/U0/inst/gt_top_i/pipe_wrapper_i/pipe_lane[1].gt_wrapper_i/gtx_channel.gtxe2_channel_i}]
set_property PACKAGE_PIN E3 [get_ports {pcie_7x_mgt_rtl_0_rxn[1]}]
set_property PACKAGE_PIN E4 [get_ports {pcie_7x_mgt_rtl_0_rxp[1]}]
set_property PACKAGE_PIN B1 [get_ports {pcie_7x_mgt_rtl_0_txn[1]}]
set_property PACKAGE_PIN B2 [get_ports {pcie_7x_mgt_rtl_0_txp[1]}]
set_property LOC GTXE2_CHANNEL_X0Y23 [get_cells {system_i/xdma_0/inst/system_xdma_0_0_pcie2_to_pcie3_wrapper_i/pcie2_ip_i/U0/inst/gt_top_i/pipe_wrapper_i/pipe_lane[3].gt_wrapper_i/gtx_channel.gtxe2_channel_i}]
set_property PACKAGE_PIN C3 [get_ports {pcie_7x_mgt_rtl_0_rxn[3]}]
set_property PACKAGE_PIN C4 [get_ports {pcie_7x_mgt_rtl_0_rxp[3]}]
set_property PACKAGE_PIN B5 [get_ports {pcie_7x_mgt_rtl_0_txn[3]}]
set_property PACKAGE_PIN B6 [get_ports {pcie_7x_mgt_rtl_0_txp[3]}]
set_property LOC GTXE2_CHANNEL_X0Y22 [get_cells {system_i/xdma_0/inst/system_xdma_0_0_pcie2_to_pcie3_wrapper_i/pcie2_ip_i/U0/inst/gt_top_i/pipe_wrapper_i/pipe_lane[2].gt_wrapper_i/gtx_channel.gtxe2_channel_i}]
set_property PACKAGE_PIN D5 [get_ports {pcie_7x_mgt_rtl_0_rxn[2]}]
set_property PACKAGE_PIN D6 [get_ports {pcie_7x_mgt_rtl_0_rxp[2]}]
set_property PACKAGE_PIN A3 [get_ports {pcie_7x_mgt_rtl_0_txn[2]}]
set_property PACKAGE_PIN A4 [get_ports {pcie_7x_mgt_rtl_0_txp[2]}]

set_property PACKAGE_PIN AE23 [get_ports IIC_0_sda_io]
set_property PACKAGE_PIN AD23 [get_ports IIC_0_scl_io]
set_property IOSTANDARD LVCMOS33 [get_ports IIC_0_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports IIC_0_scl_io]

set_property PACKAGE_PIN AB20 [get_ports UART_0_rxd]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_rxd]
set_property PACKAGE_PIN AC20 [get_ports UART_0_txd]
set_property IOSTANDARD LVCMOS33 [get_ports UART_0_txd]

# port 0
set_property PACKAGE_PIN AC8 [get_ports gigeth_gtrefclk_clk_p]
set_property PACKAGE_PIN AH9 [get_ports amc_port0_rxn]
set_property PACKAGE_PIN AH10 [get_ports amc_port0_rxp]
set_property PACKAGE_PIN AJ11 [get_ports amc_port0_txn]
set_property PACKAGE_PIN AJ12 [get_ports amc_port0_txp]


