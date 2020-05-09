local dsp = require "luci.dispatcher"
local devman = require "devman"
local lic = devman.sys.license()
local mote_max = lic and lic["mote"] or 128
local gw_max = lic and lic["gateway"] or 5

m = Map("lorasrv", translate("LoRa Network Server Application Overview"))
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
app = m:section(TypedSection, "app", translate("Applications"))
app.addremove = true
app.dynamic = false
app.template = "cbi/tblsection"
app.sectionhead = "ID"
app.extedit = dsp.build_url("admin", "loraserver", "app", "edit", "%s")
app.placeholder = translate("Please input application name")
name = app:option(DummyValue,"name", translate("Name"))

devs = app:option(DummyValue,"device", translate("Devices"))
function devs.cfgvalue(self, section)
	return #m.uci:get_list("lorasrv", section, "device")
end

data = app:option(DummyValue, "date", translate("Creation Date"))
description = app:option(DummyValue, "description", translate("Description"))
function app.create(self, section)
	m.proceed = true
	local last_id = m:get("lorasrv", "app") or "0"
	local app_id = tonumber(last_id) + 1
	m.uci:section("lorasrv","app",app_id,{ 
			name = section,
			date = os.date()
		})
	m:set("lorasrv","app", app_id)
	m.uci:save("lorasrv")
	luci.http.redirect(dsp.build_url("admin","loraserver","app","edit","" .. app_id))
	return true
end

function app.remove(self, section)
	local devs = m.uci:get_list("lorasrv", section, "device")
	for i, v in pairs(devs) do
		m:del(v)
	end
	m:del(section)
end



return m