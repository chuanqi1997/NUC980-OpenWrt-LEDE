#!/usr/bin/lua

local out = io.stderr
local rs232 = require "luars232"
require("uci")

x = uci.cursor()

function Sleep(n)
   os.execute("sleep " .. n)
end




LORA = 0
local timeout = 1000 -- in milisecon

at_port = x:get("sub-ghz", "rfconfig", "AT_Serial")


print("AT_Serial:"..at_port )
--print("SUB-GHZ-AT-CMD-LUA-Version 0.1")

-- open port
local e, p = rs232.open(at_port)
--[[
if e ~= rs232.RS232_ERR_NOERROR then
	-- handle error
	out:write(string.format("can't open serial port '%s', error: '%s'\n",
			port_name, rs232.error_tostring(e)))
	return
end
--]]

--[[
-- set port settings
assert(p:set_baud_rate(rs232.RS232_BAUD_115200) == rs232.RS232_ERR_NOERROR)
assert(p:set_data_bits(rs232.RS232_DATA_8) == rs232.RS232_ERR_NOERROR)
assert(p:set_parity(rs232.RS232_PARITY_NONE) == rs232.RS232_ERR_NOERROR)
assert(p:set_stop_bits(rs232.RS232_STOP_1) == rs232.RS232_ERR_NOERROR)
--assert(p:set_flow_control(rs232.RS232_FLOW_OFF)  == rs232.RS232_ERR_NOERROR)
out:write(string.format("OK, port open with values '%s'\n", tostring(p)))
--]]



--OPEN THE RE DISPLAY
local err, len_written = p:write("+++", timeout)
assert(e == rs232.RS232_ERR_NOERROR)

-- read with timeout
local read_len = 1 -- read one byte
local timeout = 100 -- in miliseconds
local err, data_read, size = p:read(read_len, timeout)
assert(e == rs232.RS232_ERR_NOERROR)

if(data_read == "a")
	then
	print("at +++ read:"..data_read)
	p:write("a", timeout)
end

-- write with timeout 100 msec
local err, len_written = p:write("ATE0\r\n", timeout)
assert(e == rs232.RS232_ERR_NOERROR)

Sleep(1)

local err, len_written = p:write("AT\r\n", timeout)
assert(e == rs232.RS232_ERR_NOERROR)




local err, data_read, size = p:read(64, 3000)
print("err:"..err)
print("data_read:"..data_read)
print("size:"..size)


Sleep(1)

local err, len_written = p:write("AT+CSQ?\r\n", timeout)
assert(e == rs232.RS232_ERR_NOERROR)
local err, data_read, size = p:read(64, 3000)
print("AT+CSQ:",err,data_read,size)


modem = x:get("sub-ghz", "rfconfig", "modem")

Sleep(1)
cmd = "AT+MODEM="..modem.."\r\n"
err, len_written = p:write(cmd, timeout)
assert(e == rs232.RS232_ERR_NOERROR)

local err, data_read, size = p:read(64, 3000)
print("SEND:"..cmd)
print("AT+MODEM:",err,data_read,size)


if( at_modem == LORA )
then
	print("Modem is LORA" )
	sf = x:get("sub-ghz", "rfconfig", "sf")
	
	Sleep(1)
	cmd = "AT+SF="..sf.."\r\n"
	err, len_written = p:write(cmd, timeout)
	assert(e == rs232.RS232_ERR_NOERROR)
	local err, data_read, size = p:read(64, 3000)
	print("AT+SF:",err,data_read,size)
	print("SEND:"..cmd)


	BW = x:get("sub-ghz", "rfconfig", "bw")
	
	Sleep(1)
	cmd = "AT+BW="..bw.."\r\n"
	err, len_written = p:write(cmd, timeout)
	assert(e == rs232.RS232_ERR_NOERROR)
	local err, data_read, size = p:read(64, 3000)
	print("SEND:"..cmd)
	print("AT+BW:",err,data_read,size)
else
	print("Modem Not's LORA" )

	datarate = x:get("sub-ghz", "rfconfig", "datarate")
	
	Sleep(1)
	cmd = "AT+DATA_RATE="..datarate.."\r\n"
	err, len_written = p:write(cmd, timeout)
	assert(e == rs232.RS232_ERR_NOERROR)
	local err, data_read, size = p:read(64, 3000)
	print("SEND:"..cmd)
	print("AT+DATA_RATE:",err,data_read,size)
end

power = x:get("sub-ghz", "rfconfig", "power")

Sleep(1)
cmd = "AT+POWER="..power.."\r\n"
err, len_written = p:write(cmd, timeout)
assert(e == rs232.RS232_ERR_NOERROR)
local err, data_read, size = p:read(64, 3000)
print("SEND:"..cmd)
print("AT+POWER:",err,data_read,size)

sfre = x:get("sub-ghz", "rfconfig", "sfre")
Sleep(1)
cmd = "AT+TFREQ="..sfre.."\r\n"
err, len_written = p:write(cmd, timeout)
assert(e == rs232.RS232_ERR_NOERROR)
local err, data_read, size = p:read(64, 3000)
print("SEND:"..cmd)
print("AT+TFREQ:",err,data_read,size)

rfre = x:get("sub-ghz", "rfconfig", "rfre")
Sleep(1)
cmd = "AT+RFREQ="..rfre.."\r\n"
err, len_written = p:write(cmd, timeout)
assert(e == rs232.RS232_ERR_NOERROR)
local err, data_read, size = p:read(64, 3000)
print("SEND:"..cmd)
print("AT+RFREQ:",err,data_read,size)

channel = x:get("sub-ghz", "rfconfig", "def_channel")
Sleep(1)
cmd = "AT+CHANNEL="..channel.."\r\n"
err, len_written = p:write(cmd, timeout)
assert(e == rs232.RS232_ERR_NOERROR)
local err, data_read, size = p:read(64, 3000)
print("SEND:"..cmd)
print("AT+CHANNEL:",err,data_read,size)


Sleep(1)                                             
-- close
cmd = "AT+ENTM\r\n"
err, len_written = p:write(cmd, timeout)




assert(p:close() == rs232.RS232_ERR_NOERROR)












