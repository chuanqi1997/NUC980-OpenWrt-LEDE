local devman = require "devman"
local lic = devman.sys.license()
local mote_max = lic and lic["mote"] or 128
local gw_max = lic and lic["gateway"] or 5
m = Map("lorasrv", translate("LoRa Network Server General Configuration"))

local lora_device_nb = 0;
local lora_gateway_nb = 0;
m.uci:foreach("lorasrv", "app", 
	function(section)
		if section.device then
			lora_device_nb = lora_device_nb + #section.device
		end
	end
)

if lora_device_nb > mote_max then
m.warning = {
	title = "License Warning",
	text = "The number of LoRa Devices has reached the limit of the free Licenses."
}
end

s = m:section(NamedSection, "lorasrv", "lorasrv", translate("General Configuration"))

enable = s:option(Flag, "enable", translate("Enable"))

freq = s:option(ListValue, "band", translate("Region"))
local band=devman.sys.band()
if string.find(band, "923") or string.find(band, "915") or string.find(band, "920") or string.find(band, "902") or string.find(band, "928") then 
freq:value("US915", "US902-928")
freq:value("AS923", "AS923")
freq:value("AU915", "AU915")
freq:value("KR920", "KR920")
end

if string.find(band, "868") or string.find(band, "865") or string.find(band, "867") or string.find(band, "870") or string.find(band, "863") then 
freq:value("EU868", "EU863-870")
freq:value("IN865", "IN865-867")
freq:value("RU864", "RU864-870")
end

if string.find(band, "470") or string.find(band, "510") then 
freq:value("CN470", "CN470-510")
end

if string.find(band, "433") then 
freq:value("EU433", "EU433")
end
--channel = s:option(Value, "channel", translate("Enable Channel"))

adr = s:option(Flag, "adr_enable", translate("Enable ADR"))

dr_min = s:option(ListValue, "dr_min", translate("Minimum allowed data-rate"), translate("Minimum allowed data rate. Used for ADR."))
dr_min.template = "cbi/adr_min"
dr_min:value("0","DR_0")
dr_min:value("1","DR_1")
dr_min:value("2","DR_2")
dr_min:value("3","DR_3")
dr_min:value("4","DR_4")
dr_min:value("5","DR_5")
dr_min:value("6","DR_6")
dr_min:value("7","DR_7")
dr_min:value("8","DR_8")
dr_min:value("9","DR_9")
dr_min:value("10","DR_10")
dr_min:value("11","DR_11")
dr_min:value("12","DR_12")
dr_min:value("13","DR_13")
dr_min:value("14","DR_14")
dr_min:value("15","DR_15")
dr_max = s:option(ListValue, "dr_max", translate("Maximum allowed data-rate"), translate("Maximum allowed data rate. Used for ADR."))
dr_max.template = "cbi/adr_max"
dr_max:value("0","DR_0")
dr_max:value("1","DR_1")
dr_max:value("2","DR_2")
dr_max:value("3","DR_3")
dr_max:value("4","DR_4")
dr_max:value("5","DR_5")
dr_max:value("6","DR_6")
dr_max:value("7","DR_7")
dr_max:value("8","DR_8")
dr_max:value("9","DR_9")
dr_max:value("10","DR_10")
dr_max:value("11","DR_11")
dr_max:value("12","DR_12")
dr_max:value("13","DR_13")
dr_max:value("14","DR_14")
dr_max:value("15","DR_15")

adr_margin_db = s:option(Value, "adr_margin_db", translate("ADR Margin (dB)"), translate("A higher number means that the network-server will keep more margin, resulting in a lower data-rate but decreasing the chance that the device gets disconnected because it is unable to reach one of the surrounded gateways."))
adr_margin_db.default = 10
adr_margin_db.datatype = "range(-5, 20)"

nid = s:option(Value, "network_id", translate("Network ID"))
nid.default = "1"

RECEIVE_DELAY1 = s:option(Value, "RECEIVE_DELAY1", translate("Rx 1 Delay (sec)"))
RECEIVE_DELAY1.default = 1
RECEIVE_DELAY1.datatype = "range(1, 15)"

Rx1DrOffSet = s:option(ListValue, "Rx1DrOffset", translate("Rx 1 DataRate Offset"))
Rx1DrOffSet:value(0, "0")
Rx1DrOffSet:value(1, "1")
Rx1DrOffSet:value(2, "2")
Rx1DrOffSet:value(3, "3")
Rx1DrOffSet:value(4, "4")
Rx1DrOffSet:value(5, "5")
Rx1DrOffSet:value(6, "6")
Rx1DrOffSet:value(7, "7")
Rx1DrOffSet.template = "cbi/Rx1DrOffset"
Rx1DrOffSet.default = 0

Rx2Frequency = s:option(Value, "Rx2Frequency", translate("Rx 2 Frequency (Hz)"))
Rx2Frequency.default = 0;
Rx2Frequency.datatype = 'integer'
Rx2Frequency.template = 'cbi/Rx2Frequency'

Rx2Datarate = s:option(ListValue, "Rx2Datarate", translate("Rx 2 Datarate"))
Rx2Datarate.template = 'cbi/Rx2DR'
Rx2Datarate:value(-1, "Regional Default")
Rx2Datarate.default = -1;
Rx2Datarate:value("0","DR_0")
Rx2Datarate:value("1","DR_1")
Rx2Datarate:value("2","DR_2")
Rx2Datarate:value("3","DR_3")
Rx2Datarate:value("4","DR_4")
Rx2Datarate:value("5","DR_5")
Rx2Datarate:value("6","DR_6")
Rx2Datarate:value("7","DR_7")
Rx2Datarate:value("8","DR_8")
Rx2Datarate:value("9","DR_9")
Rx2Datarate:value("10","DR_10")
Rx2Datarate:value("11","DR_11")
Rx2Datarate:value("12","DR_12")
Rx2Datarate:value("13","DR_13")
Rx2Datarate:value("14","DR_14")
Rx2Datarate:value("15","DR_15")
dl_pwr = s:option(ListValue, "dl_pwr", translate("Downlink Tx Power (dBm)"))
dl_pwr.default = 20
dl_pwr:value(-6)
dl_pwr:value(-3)
dl_pwr:value(0)
dl_pwr:value(3)
dl_pwr:value(6)
dl_pwr:value(10)
dl_pwr:value(11)
dl_pwr:value(12)
dl_pwr:value(13)
dl_pwr:value(14)
dl_pwr:value(16)
dl_pwr:value(20)
dl_pwr:value(23)
dl_pwr:value(25)
dl_pwr:value(26)
dl_pwr:value(27)

UpDwellTime = s:option(ListValue, "UpDwellTime", translate("Uplink Dwell Time Limit"))
UpDwellTime:value(0, "0 - No Limit")
UpDwellTime:value(1, "1 - 400ms")
UpDwellTime:depends("band", "AS923")
UpDwellTime:depends("band", "AU915")
UpDwellTime.default = 1

DownDwellTime = s:option(ListValue, "DownDwellTime", translate("Downlink Dwell Time Limit"))
DownDwellTime:value(0, "0 - No Limit")
DownDwellTime:value(1, "1 - 400ms")
DownDwellTime:depends("band", "AS923")
DownDwellTime.default = 1

status=s:option(Value, "status_interval", translate("Device-status request interval (sec)"), translate("Interval to initiate an End-Device status request (seconds). Set to 0 to disable."))
status.default = 0

loglvl = s:option(ListValue, "loglvl", translate("Log Level"))
loglvl:value(0, "ERROR")
loglvl:value(1, "INFO")
loglvl:value(2, "DEBUG")
loglvl:value(3, "TRACE")
loglvl.default = 1

stat_intv=s:option(ListValue, "stat_interval", translate("Statistic Period"))
stat_intv:value(60, translate("1 Minute"))
stat_intv:value(600, translate("10 Minutes"))
stat_intv:value(3600, translate("1 Hour"))
stat_intv:value(86400, translate("1 Day"))
stat_intv.default = 600
return m