local uci = require "luci.model.uci".cursor()

local dsp = require "luci.dispatcher"
local app=arg[1]

f = SimpleForm("batch", "Batch Add Device to Application " .. uci:get("lorasrv", app, "name"))
f.redirect = dsp.build_url("admin","loraserver","app","edit", app)
f.template = "cbi/batch_add_device"

start = f:field(Value, "start", translate("Start Device EUI"))
start.rmempty = false
step = f:field(Value, "step", translate("Step"))
step.rmempty = false
count = f:field(Value, "count", translate("Count"))
count.rmempty = false
app_key = f:field(Value, "app_key", translate("Application Key"))
app_key.rmempty = false

if f:submitstate() and start:formvalue(1) ~= "" and step:formvalue(1) ~= "" and count:formvalue(1) ~="" and app_key:formvalue(1) ~="" then
	local dev_table = uci:get_list("lorasrv",app, "device")
	local eui = tonumber(start:formvalue(1), 16)
	local s = tonumber(step:formvalue(1))
	local c = tonumber(count:formvalue(1))
	local key = app_key:formvalue(1)

	app_dlist = uci:get_list("lorasrv",app,"device")
	--if type(app_list) ~= "table" then
	--	app_dlist = { app_dlist }
	--end

	for i=eui,eui+c,s do
		local seui = tostring(i,16)
		local name="dev-" .. seui
		local value = {
					name = name,
					mode = "otaa",
					app_key = key
				}
		if uci:section("lorasrv", "device", seui, value) then
			table.insert(app_dlist, seui)
		end
	end

	uci:set_list("lorasrv", app, "device", app_dlist)
	uci:save("lorasrv")
	luci.http.redirect(dsp.build_url("admin","loraserver","app","edit", app))
end

return f