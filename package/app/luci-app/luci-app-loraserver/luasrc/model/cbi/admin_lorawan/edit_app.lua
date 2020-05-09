local dsp = require "luci.dispatcher"
local devman = require "devman"
local lic = devman.sys.license()
local mote_max = lic and lic["mote"] or 128
local gw_max = lic and lic["gateway"] or 5
m = Map("lorasrv", translate("Application Edit - ") .. arg[1]  )

m.redirect = dsp.build_url("admin","loraserver","app")
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

s = m:section(NamedSection, arg[1], "application", "Application " .. m.uci:get("lorasrv",arg[1],"name"))

if m:get(arg[1], "eui") then
	s:tab("dev", translate("Devices"))
	devs = s:taboption("dev", DummyValue, "devices", translate("Devices"))
	devs.template = "cbi/lora-app-devices"
	devs.app_id = arg[1]
	devs.err = arg[2]
end

s:tab("conf", translate("Application Configuration"))
s:taboption("conf", Value, "name", translate("Name"))

eui = s:taboption("conf", Value, "eui", translate("Application EUI"))
eui.rmempty = false
eui.datatype="loraeui"
eui.template = "cbi/loraeui"
key = s:taboption("conf", Value, "app_key", translate("Application Key"))
key.rmempty = false
key.datatype="lorakey"
key.template = "cbi/lorakey"

s:taboption("conf", Flag, "auto_add", translate("Auto Add LoRa Device"), translate("If enabled, LoRa Device will be added automatically after Application EUI and Application Key pass verification."))
s:taboption("conf", Value, "description", translate("Description"))


s:tab("int", translate("Integrations"))

encode = s:taboption("int", ListValue, "data_encode", translate("Data Encode/Decode Type"))
encode:value("base64", "Base64")
encode:value("hexstr", "HEX String")
encode.default = "base64"

ReportRadioInfo = s:taboption("int", Flag, "ReportRadioInfo", translate("Report LoRa Radio Infomation"))
ReportRadioInfo.default = 1

http_enable = s:taboption("int", Flag, "http_enable", translate("Enable HTTP/HTTPS Integration"))
http_enable.default = 0

http_headers = s:taboption("int", DynamicList, "http_headers", translate("HTTP/HTTPS Headers"))
http_headers.template="cbi/app_httpheaders"


jsonurl = s:taboption("int", Value, "UplinkUrl", translate("Uplink data URL"))
jsonurl = s:taboption("int", Value, "JoinUrl", translate("Join notification URL"))
jsonurl = s:taboption("int", Value, "AckUrl", translate("Ack notification URL"))
jsonurl = s:taboption("int", Value, "StatusUrl", translate("Device-status notification URL"))

max_connect = s:taboption("int", Value, "http_max_connection", translate("Maximum number of concurrent connections"))
max_connect.datatype = "range(1, 256)"
max_connect.default = 16

max_queue = s:taboption("int", Value, "http_max_queue",translate("Maximum length of queue"))
max_queue.datatype = "range(1, 512)"
max_queue.default = 64


return m
