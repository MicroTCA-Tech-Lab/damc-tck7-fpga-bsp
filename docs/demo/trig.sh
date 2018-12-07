#! /bin/bash

# script to configure and trigger AXI Traffic generator

AXI_TG_BASE=0x10000

AXI_TG_ADDR_ST_CTRL=0x30
AXI_TG_ADDR_ST_CONF=0x34
AXI_TG_ADDR_TR_LEN=0x38

# set delay between packets
reg_rw /dev/xdma/card0/user $(($AXI_TG_BASE + $AXI_TG_ADDR_ST_CONF)) w 0x10000

# set packet len, number of packets
reg_rw /dev/xdma/card0/user $(($AXI_TG_BASE + $AXI_TG_ADDR_TR_LEN)) w 0x107ff

# trigger the traffic generation
reg_rw /dev/xdma/card0/user $(($AXI_TG_BASE + $AXI_TG_ADDR_ST_CTRL)) w 1

