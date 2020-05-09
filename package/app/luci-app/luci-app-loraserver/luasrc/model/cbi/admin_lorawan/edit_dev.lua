local util = require "luci.util"
local dsp = require "luci.dispatcher"

local devman = require "devman"
local lic = devman.sys.license()
local mote_max = lic and lic["mote"] or 128
local gw_max = lic and lic["gateway"] or 5

m = Map("lorasrv", translate("Application - Edit Device ") .. arg[2])
m.redirect = dsp.build_url("admin","loraserver","app","edit", arg[1])  
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
s = m:section(NamedSection, arg[2], "device",  translate("Device") .. "-" ..arg[2])

if m:formvalue("cbid.lorasrv." .. arg[2] .. "._clear_fcnt") then
	luci.sys.exec("srvctrl -c %s" % { arg[2]})
end

local live_info = util.exec("srvctrl -d %s" % { arg[2]})
local live_obj = luci.json.decode(live_info)
local live_data = live_obj and live_obj["data"]
local live_stat = live_data and live_data["stat"]

if m:get(arg[2], "name") then
	s:tab("ov", translate("Overview"))
end

s:tab("conf", translate("Configuration"))
s:tab("act", translate("Activation"))
s:tab("dl",translate("Downlink"))
s:tab("live", translate("Live Device Data"))

if m:get(arg[2], "name") then
	overview = s:taboption("ov",DummyValue, translate("Overview"));
	overview.template = "cbi/lora-device-statistic"
	--overview.stat = live_stat
	overview.data = live_data
end


dlink=s:taboption("dl", DummyValue, translate("Confirmed"))
dlink.template = "cbi/lora-device-dlink"
dlink.app_id = arg[1]
dlink.dev_id = arg[2]

Name = s:taboption("conf",Value, "name", translate("Device name"))
Name.rmempty = false

Class = s:taboption("conf", ListValue, "Class", translate("Class"))
Class:value("A", "A")
Class:value("C", "C")

mode = s:taboption("conf", ListValue,"mode", translate("Join mode"))

mode:value("otaa", translate("OTAA"))
mode:value("abp", "ABP")

use_def_appkey = s:taboption("conf", Flag, "use_spec_app_key", translate("Use specific application key"))
use_def_appkey.default = "0"
use_def_appkey:depends("mode", "otaa")

appkey=s:taboption("conf", Value, "app_key", translate("Application Key"))
appkey:depends("use_spec_app_key", "1")
appkey.template = "cbi/lorakey"

addr = s:taboption("conf", Value, "dev_addr", translate("Device Address"))
addr:depends("mode", "abp")

appskey=s:taboption("conf", Value, "apps_key", translate("Application Session Key"))
appskey:depends("mode","abp")
appskey.template = "cbi/lorakey"

mwkskey=s:taboption("conf", Value, "nwks_key", translate("Network Session Key"))
mwkskey:depends("mode","abp")
mwkskey.template = "cbi/lorakey"

fcntwidth = s:taboption("conf", ListValue, "fcntwidth", translate("Frame-counter Width"))
fcntwidth:value("16", "16 bit")
fcntwidth:value("32","32 bit")
fcntwidth.default = "32"

local valf = s:taboption("conf", Flag, "valid_fcnt", translate("Eanble frame-counter Validation"))
valf.default = "1"

--vappeui = s:taboption("conf", Flag, "disable_app_eui",translate("Disable Application EUI Validation"))
--vappeui.default = "1";
lptp = s:taboption("conf", Flag, "lptp", translate("Enable LPTP"))
lptp.default = "0"

s:taboption("conf", Value, "description", translate("Description"))
live = s:taboption("live", DummyValue, "devices", translate("Devices"))
live.template = "cbi/lora-device-live"
live.app_id = arg[1]
live.dev_id = arg[2]

if live_data then
	dev = s:taboption("act", DummyValue, "live_addr",translate("Device address"))
	dev.value = live_data["dev_addr"]

	appsk = s:taboption("act", DummyValue, "live_appskey",translate("Application session key"))
	appsk.value = live_data["apps_key"]

	appsk = s:taboption("act", DummyValue, "live_nwkskey",translate("Network session key"))
	appsk.value = live_data["nwks_key"]

	appsk = s:taboption("act", DummyValue, "fcnt_up",translate("Uplink frame-counter"))
	appsk.value = live_data["fcnt_up"]

	appsk = s:taboption("act", DummyValue, "fcnt_down",translate("Downlink frame-counter"))
	appsk.value = live_data["fcnt_down"]

	btn = s:taboption("act", Button, "_clear_fcnt", translate("Clear frame-counter"))
	btn.value="Clear"
end


return m
