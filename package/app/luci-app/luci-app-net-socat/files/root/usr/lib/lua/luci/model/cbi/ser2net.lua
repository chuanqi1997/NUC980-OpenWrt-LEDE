--[[
LuCI - Lua Configuration Interface
Copyright 2010 Jo-Philipp Wich <xm@subsignal.org>
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
--]]


require("luci.sys")


-- /etc/config/ser2net
m = Map("ser2net", translate("Serial<->Networking"), translate("Serial data and network data exchanged."))

s = m:section(TypedSection, "ser2net", "")

--Don't Allow the user to create and remove the interfaces
s.addremove = false
s.anonymous = true

s:tab("basic", translate("Basic Configuration"))

enabled = s:taboption("basic", Flag, "enabled", translate("Enable"))
enabled.default = 0

--enabled = s:taboption("basic", ListValue, "enabled", translate("Enable"))                   
--enabled:value("0", "OFF")
--enabled:value("1", "ON")                                                                           
--enabled.default = "0"  




log_enabled = s:taboption("basic", Flag, "socat_log_enable", translate("Log Enable"))
log_enabled.default = 0

--the thrid par is the config par
DeviceId = s:taboption("basic", Value, "DeviceId", translate("Device ID"))
DeviceId.rmempty = false
DeviceId.default = "12345678"
--DeviceId:depends("enabled", "1")


--interval = s:taboption("basic", Value, "interval", translate("Heartbeat Interval (s)"))
--interval.rmempty = false
--interval.default = "60"
--interval:depends("enabled", "1")

--heartbeat = s:taboption("basic", Value, "heartbeat", translate("Heartbeat Content"))
--heartbeat.rmempty = false
--heartbeat.default = "FE"
--heartbeat:depends("enabled", "1")


--MaxDataLen = s:taboption("basic", Value, "MaxDataLen", translate("MaxDataLen"))
--MaxDataLen.rmempty = false
--MaxDataLen.default = "1000"
--MaxDataLen:depends("enabled", "1")


DataWaitTime = s:taboption("basic", Value, "MaxWaitTime", translate("Data Max Wait Time(S)"))
DataWaitTime.rmempty = false
DataWaitTime.default = "30"
--DataWaitTime:depends("enabled", "1")


--ReDialTime = s:taboption("basic", Value, "ReDialTime", translate("ReDialTime"))
--ReDialTime.rmempty = false
--ReDialTime.default = "15"
--ReDialTime:depends("enabled", "1")



s:tab("serial", translate("Serial Configuration"))

baudrate = s:taboption("serial", ListValue, "baudrate", translate("Baudrate (bps)"))
baudrate:value("300", "300")
baudrate:value("1200", "1200")
baudrate:value("2400", "2400")
baudrate:value("4800", "4800")
baudrate:value("9600", "9600")
baudrate:value("19200", "19200")
baudrate:value("38400", "38400")
baudrate:value("57600", "57600")
baudrate:value("115200", "115200")
baudrate.default = "9600"

databit = s:taboption("serial", ListValue, "databit", translate("Databit"))
databit:value("7DATABITS", "7")
databit:value("8DATABITS", "8")
databit.default = "8DATABITS"

parity_check = s:taboption("serial", ListValue, "parity_check", translate("Odd-Even Check"))
parity_check:value("NONE", translate("None Check"))
parity_check:value("ODD", translate("Odd Check"))
parity_check:value("EVEN", translate("Even Check"))
parity_check.default = "NONE"

stopbit = s:taboption("serial", ListValue, "stopbit", translate("Stopbit"))
stopbit:value("1STOPBIT", "1")
stopbit:value("2STOPBITS  ", "2")
stopbit.default = "1STOPBIT"

s:tab("network", translate("Networking Configuration"))

o = s:taboption("network", ListValue, "srvmode", translate("Net Mode"))
o:value("0", "TCP Client")
--o:value("1", "Tcp")
--o:value("2", "Udp")
--o:value("3", "Tcp&Udp")


serip = s:taboption("network", Value, "server_addr", translate("Server Address")) 
--serip:depends("srvmode", "0")
--serip.datatype="port"


o = s:taboption("network", Value, "server_port", translate("Server Port"))
--o:depends("srvmode", "0")
--o:depends("srvmode", "2")
--o:depends("srvmode", "3")
o.datatype="port"


protocol = s:taboption("network", ListValue, "protocol", translate("Net Protocol"))


--protocol:value("udp", "Udp")
protocol:value("tcp", "Tcp")

protocol:depends("srvmode", "0")

--ip1 = s:taboption("network", Value, "ip1", translate("Data Center 1"))
--ip1:depends("srvmode", "0")


--ip1.datatype="ip4addr"

BasePort1 = s:taboption("network", Value, "local_port", translate("Local Used Port"))
BasePort1:depends("srvmode", "0")
BasePort1.datatype="port"




reconnect = s:taboption("network", Value, "ReConnectTime", translate("Re Connect Time(S)"))   
reconnect:depends("srvmode", "0")                                                           
reconnect.datatype="port" 
reconnect.default="30"

--[[

protocol2 = s:taboption("network", ListValue, "protocol2", translate("Protocol 2"))
protocol2:value("udp", "Udp")
protocol2:value("tcp", "Tcp")
protocol2:depends("srvmode", "0")
ip2 = s:taboption("network", Value, "ip2", translate("Data Center 2"))
ip2:depends("srvmode", "0")

--ip2.datatype="ip4addr"

BasePort2 = s:taboption("network", Value, "BasePort2", translate("Data Center 2 Port"))
BasePort2:depends("srvmode", "0")
BasePort2.datatype="port"

protocol3 = s:taboption("network", ListValue, "protocol3", translate("Protocol 3"))
protocol3:value("udp", "Udp")
protocol3:value("tcp", "Tcp")
protocol3:depends("srvmode", "0")
ip3 = s:taboption("network", Value, "ip3", translate("Data Center 3"))
ip3:depends("srvmode", "0")

--ip3.datatype="ip4addr"

BasePort3 = s:taboption("network", Value, "BasePort3", translate("Data Center 3 Port"))
BasePort3:depends("srvmode", "0")
BasePort3.datatype="port"

protocol4 = s:taboption("network", ListValue, "protocol4", translate("Protocol 4"))
protocol4:value("udp", "Udp")
protocol4:value("tcp", "Tcp")
protocol4:depends("srvmode", "0")
ip4 = s:taboption("network", Value, "ip4", translate("Data Center 4"))
ip4:depends("srvmode", "0")

--ip4.datatype="ip4addr"

BasePort4 = s:taboption("network", Value, "BasePort4", translate("Data Center 4 Port"))
BasePort4:depends("srvmode", "0")
BasePort4.datatype="port"

protocol5 = s:taboption("network", ListValue, "protocol5", translate("Protocol 5"))
protocol5:value("udp", "Udp")
protocol5:value("tcp", "Tcp")
protocol5:depends("srvmode", "0")
ip5 = s:taboption("network", Value, "ip5", translate("Data Center 5"))
ip5:depends("srvmode", "0")

--ip5.datatype="ip4addr"

BasePort5 = s:taboption("network", Value, "BasePort5", translate("Data Center 5 Port"))
BasePort5:depends("srvmode", "0")
BasePort5.datatype="port"

--]]

local apply = luci.http.formvalue("cbi.apply")
if apply then
	io.popen("/etc/init.d/ser2net_client restart")
	io.popen("/etc/init.d/socat restart")
end
return m
