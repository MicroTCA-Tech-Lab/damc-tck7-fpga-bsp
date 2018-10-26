# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "ENABLE" -parent ${Page_0}
  set PKT_DELAY_LIM [ipgui::add_param $IPINST -name "PKT_DELAY_LIM" -parent ${Page_0}]
  set_property tooltip {in clock cycles} ${PKT_DELAY_LIM}
  #Adding Group
  set Config [ipgui::add_group $IPINST -name "Config" -parent ${Page_0}]
  ipgui::add_param $IPINST -name "MAC_ADDR" -parent ${Config}
  ipgui::add_param $IPINST -name "IP_ADDR" -parent ${Config}
  ipgui::add_param $IPINST -name "UDP_PORT" -parent ${Config}

  #Adding Group
  set Suggestion [ipgui::add_group $IPINST -name "Suggestion" -parent ${Page_0}]
  ipgui::add_static_text $IPINST -name "Netcat" -parent ${Suggestion} -text {Use tcpdump to listen to the transmitted packets
<html><pre>
sudo tcpdump -n udp dst port 12345 -v -X
</pre></html>}

  #Adding Group
  set UDP/IPv4_Engine [ipgui::add_group $IPINST -name "UDP/IPv4 Engine" -parent ${Page_0}]
  ipgui::add_static_text $IPINST -name "FullUDPcore" -parent ${UDP/IPv4_Engine} -text {<html>This is a very simple core, capable of periodically transmitting
just a single pre-recorded packet. Here it used to demonstrate
Gigabit Ethernet over MicroTCA backplane.

If you would need a complete UDP/IPv4 high-performance engine,
please visit <a href=https://techlab.desy.de>MicroTCA Technology Lab (https://techlab.desy.de)</a> website.</html>}



}

proc update_PARAM_VALUE.ENABLE { PARAM_VALUE.ENABLE } {
	# Procedure called to update ENABLE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ENABLE { PARAM_VALUE.ENABLE } {
	# Procedure called to validate ENABLE
	return true
}

proc update_PARAM_VALUE.IP_ADDR { PARAM_VALUE.IP_ADDR } {
	# Procedure called to update IP_ADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.IP_ADDR { PARAM_VALUE.IP_ADDR } {
	# Procedure called to validate IP_ADDR
	return true
}

proc update_PARAM_VALUE.MAC_ADDR { PARAM_VALUE.MAC_ADDR } {
	# Procedure called to update MAC_ADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.MAC_ADDR { PARAM_VALUE.MAC_ADDR } {
	# Procedure called to validate MAC_ADDR
	return true
}

proc update_PARAM_VALUE.PKT_DELAY_LIM { PARAM_VALUE.PKT_DELAY_LIM } {
	# Procedure called to update PKT_DELAY_LIM when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PKT_DELAY_LIM { PARAM_VALUE.PKT_DELAY_LIM } {
	# Procedure called to validate PKT_DELAY_LIM
	return true
}

proc update_PARAM_VALUE.UDP_PORT { PARAM_VALUE.UDP_PORT } {
	# Procedure called to update UDP_PORT when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.UDP_PORT { PARAM_VALUE.UDP_PORT } {
	# Procedure called to validate UDP_PORT
	return true
}


proc update_MODELPARAM_VALUE.ENABLE { MODELPARAM_VALUE.ENABLE PARAM_VALUE.ENABLE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ENABLE}] ${MODELPARAM_VALUE.ENABLE}
}

proc update_MODELPARAM_VALUE.PKT_DELAY_LIM { MODELPARAM_VALUE.PKT_DELAY_LIM PARAM_VALUE.PKT_DELAY_LIM } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PKT_DELAY_LIM}] ${MODELPARAM_VALUE.PKT_DELAY_LIM}
}

