module("luci.controller.admin.loraserver", package.seeall)
local json = require "luci.json"
local util = require "luci.util"

function index()
	entry({"admin", "lorawan", "gwbridge"}, cbi("admin_lorawan/gwbridge"),_("LoRa Gateway MQTT Bridge"),20)
	
	entry({"admin", "loraserver"}, alias("admin","loraserver","status"), _("LoRa Network Server"), 50)
	entry({"admin", "loraserver", "status"}, template("admin_lorawan/loraserver_status"),_("Status"), 5)
	entry({"admin", "loraserver", "basic"}, cbi("admin_lorawan/basic"),_("General"), 10)
	entry({"admin", "loraserver", "gateway"}, cbi("admin_lorawan/gwbackend"),_("Gateway"), 15)
	entry({"admin", "loraserver", "gateway","edit"}, cbi("admin_lorawan/edit_gateway")).leaf = true
	entry({"admin", "loraserver", "app"}, cbi("admin_lorawan/app"),_("Application"), 20)
	entry({"admin", "loraserver", "app","edit"}, cbi("admin_lorawan/edit_app")).leaf = true
	entry({"admin", "loraserver", "app", "add_device"},call("add_device"))
	entry({"admin", "loraserver", "app", "edit_device"},cbi("admin_lorawan/edit_dev")).leaf = true
	entry({"admin", "loraserver", "integation"},cbi("admin_lorawan/integation"),_("Global Integration"),30)
	entry({"admin", "loraserver", "app", "batch_add_device"}, call("batch_add_device")).leaf = true
	entry({"admin", "loraserver", "app", "export"}, call("export_devices")).leaf = true
	entry({"admin", "loraserver", "app", "import"}, call("import_devices")).leaf = true
	entry({"admin", "loraserver", "app", "import_status"}, call("import_status")).leaf = true
	entry({"admin", "loraserver", "app", "device_info"}, call("device_info")).leaf = true
	entry({"admin", "loraserver", "app", "device_live_frame"}, call("device_live_frame")).leaf = true
	entry({"admin", "loraserver", "app", "batch_remove"}, call("batch_remove")).leaf = true
	entry({"admin", "loraserver", "app", "downlink"},call("downlink")).leaf = true
end

function downlink(app, dev)
	local util = require "luci.util"
	local json = require "luci.json"
	local confirmed = luci.http.formvalue("confirmed") == "1" and "true" or "false"
	local fport = luci.http.formvalue("fPort") 
	local payload = luci.http.formvalue("payload")
	local params = "{\\\"mote\\\":\\\"" .. dev .. "\\\",\\\"confirmed\\\":"..confirmed..",\\\"fPort\\\":" .. fport .. ",\\\"payload\\\":\\\"" .. payload .. "\\\"}"

	local res = luci.util.exec("srvctrl -p \"%s\" 2>&1" % { params })
	luci.http.write(res)
	return
end

function add_device()
	local uci = require "luci.model.uci".cursor()
	local http = require "luci.http"
	local dsp = require "luci.dispatcher"
	local eui = string.lower(http.formvalue("name"))
	local app = http.formvalue("app")
	local app_key
	
	if uci:get("lorasrv", eui ) then
		http.redirect(dsp.build_url("admin","loraserver","app", "edit",app ) .. "?err=dup")
		return
	end

	app_key = uci:get("lorasrv", app, "app_key")
	local section = uci:section("lorasrv","device", eui, { 
			mode = "otaa",
			Class = "A",
			app_key = app_key,
			disable_app_eui = "0"
		})
	if section then
		value = uci:get_list("lorasrv",app,"device")
		if type(value) == "table" then
			table.insert(value, eui)
		else
			value = { value, eui }
		end
		uci:set_list("lorasrv", app, "device", value)
	end

	uci:save("lorasrv")
	http.redirect(dsp.build_url("admin","loraserver","app", "edit_device",app, eui))
	return
end

function batch_add_device()
	local dsp = require "luci.dispatcher"
	local uci = require "luci.model.uci".cursor()
	local http = require "luci.http"
	local app = http.formvalue("app")
	local devs = http.formvalue("devs")
	local oui = http.formvalue("oui")
	local Class = http.formvalue("Class") or 'A'
	local json = require "luci.json"
	local app_dev_list = uci:get_list("lorasrv", app, "device")
	--local app_key = http.formvalue("app_key")
	app_key = uci:get("lorasrv", app, "app_key")
	if devs then
		local obj = json.decode(devs) 
		if obj ~= nil then
			local mode = obj['mode'];
			local t = obj['table'];
			if t ~= nil then
				if mode == 'otaa' then
					for i, v in pairs(t) do
						local dev_eui = v['dev_eui']
						if dev_eui ~= nil then
							table.insert(app_dev_list, dev_eui)
							uci:section("lorasrv", "device", dev_eui, { 
								name = "dev-" ..dev_eui,
								mode = "otaa",
								app_key = app_key,
								Class = Class,
								disable_app_eui = "0"
							})
						end
					end
				else
					local appskey = obj['appskey']
					local nwkskey = obj['nwkskey']
					for i,v in pairs(t) do
						local dev_eui = v['dev_eui']
						local dev_addr = v['dev_addr']
						if dev_eui ~= nil and dev_addr ~= nil then
							table.insert(app_dev_list, dev_eui)
							uci:section("lorasrv", "device", dev_eui, { 
								name = "dev-" ..dev_eui,
								mode = "abp",
								Class = Class,
								dev_addr = dev_addr,
								apps_key = appskey,
								nwks_key = nwkskey,
								disable_app_eui = "0"
							})
						end
					end
				end
				uci:set_list("lorasrv",app, "device", app_dev_list)
				uci:save("lorasrv")
				http.redirect(dsp.build_url("admin", "loraserver","app","edit",app))
			end
		end
	end
	luci.template.render("admin_lorawan/batch_add_device")
end

function export_devices(app)
	local http = require "luci.http"
	local uci = require "luci.model.uci".cursor()
	local devices = uci:get_list("lorasrv", app, "device")

	luci.http.header('Content-Disposition', 'attachment; filename="LoRaServer-Application-%s-devices.csv"' % { app })
	http.prepare_content("application/octet-stream")
	http.write('Device EUI,Device name,Activation mode,Application Key,Device Addr,Application Session Key,Network Session Key,Description\r\n')
	for i, d in pairs(devices) do
		local device = uci:get_all("lorasrv", d)
		http.write("%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n" % { device[".name"] or "", device["name"] or "", device["mode"] or "", device["app_key"] or "", device["dev_addr"] or "", device["apps_key"] or "", device["nwks_key"] or "", device["description"] or "", device["Class"] or "A"})
	end

	return
end

function import_devices( app )
	local dsp = require "luci.dispatcher"
	local uci = require "luci.model.uci".cursor()
	local fs = require "nixio.fs"
	local fp
	luci.http.setfilehandler(
		function(meta, chunk, eof)
			if not fp then
				fp = io.open("/tmp/.tmp_import_device_file", "w")
			end
			if chunk then
				fp:write(chunk)
			end
			if eof then
				fp:close()
			end

		end
	)

	local import_file = luci.http.formvalue("import_devices")

	if import_file and #import_file > 0 then
		
		luci.http.redirect(dsp.build_url("admin","loraserver","app","edit",app))
		local pid = nixio.fork()
		if pid > 0 then

		elseif pid == 0 then
			local function split(str, delimiter)
			    if str==nil or str=='' or delimiter==nil then
			        return nil
			    end
			 
			    local result = {}
			    for match in (str..delimiter):gmatch("(.-)"..delimiter) do
			        table.insert(result, match)
			    end
			    return result
			end
			-- patch stdin, out, err to /dev/null
			local null = nixio.open("/dev/console", "w+")
			if null then
				nixio.dup(null, nixio.stderr)
				nixio.dup(null, nixio.stdout)
				nixio.dup(null, nixio.stdin)
				if null:fileno() > 2 then
					null:close()
				end
			end
			local app_dev_list = uci:get_list("lorasrv", app, "device")
			local t
			local line, eui, name, mode, desc, appkey, appskey, nwkskey, addr
			local sfp = io.open("/tmp/.tmp_import_device_status", "w");
			local idx = 0
			for line in io.lines("/tmp/.tmp_import_device_file") do
				t = split(line, ',')
				if #t >= 8 then
					eui = t[1]:match("^([a-fA-F0-9]+)$")
					idx = idx + 1
					sfp:write("%d\r\n" % {idx})
					sfp:flush()

					if eui and not uci:get("lorasrv", eui) then
						table.insert(app_dev_list, eui)
						uci:section("lorasrv", "device", eui, {
							name = t[2],
							mode = t[3],
							app_key = t[4],
							dev_addr = t[5],
							apps_key = t[6],
							nwks_key = t[7],
							description = string.sub(t[8], 1, -2),
							Class = t[9] or "A"
							})
					
					end
				end
			end
			uci:set_list("lorasrv",app, "device", app_dev_list)
			uci:save("lorasrv")
			
			sfp:close()
			fs.remove("/tmp/.tmp_import_device_file")
			fs.remove("/tmp/.tmp_import_device_status")
		end
	end
end

function import_status()
	local fs = require "nixio.fs"
	local json = require "luci.json"
	luci.http.prepare_content("application/json");
	if fs.access("/tmp/.tmp_import_device_file") then
		local d = 0
		local t = 0
		local ts = luci.util.exec("wc -l /tmp/.tmp_import_device_file | awk '{printf $1}'")
		t = tonumber(ts)
		if( fs.access("/tmp/.tmp_import_device_status")) then	
			local ds = luci.util.exec("wc -l /tmp/.tmp_import_device_status | awk '{printf $1}'")
			d = tonumber(ds)
		end
		luci.http.write(json.encode({ total = t, done = d}))
		if( t == d ) then
			fs.remove("/tmp/.tmp_import_device_file")
			fs.remove("/tmp/.tmp_import_device_status")	
		end
	else
		luci.http.write(json.encode({ total = 0, done = 0}))
	end
	return

end

function device_info(dev)
    
    luci.http.prepare_content("application/json")
    luci.http.write(util.exec("srvctrl -d %s" % { dev }))
    return
end

function device_live_frame(dev)
	luci.http.prepare_content("application/json")
    luci.http.write(util.exec("srvctrl -l %s" % { dev }))
    return
end

function batch_remove(app)
	local dsp = require "luci.dispatcher"
	local eui = luci.http.formvalue("eui")
	local uci = require "luci.model.uci".cursor()
	local devs = uci:get_list("lorasrv",app, "device")
	local page = luci.http.formvalue("p")
	local limit = luci.http.formvalue("l")
	local find = luci.http.formvalue("find")
	if type(eui) == "table" then
		for idx, d in ipairs(eui) do
			for i=1,#devs,1 do
				if  devs[i] == d then
					table.remove(devs, i);
					break
				end
			end
			uci:delete("lorasrv",d);
		end
	else
		for i=1,#devs,1 do
			if  devs[i] == eui then
				table.remove(devs, i);
				break
			end
		end
		uci:delete("lorasrv",eui);
	end
	if #devs > 0 then
		uci:set_list("lorasrv", app, "device", devs)
	else
		uci:delete("lorasrv", app, "device")
	end
	uci:save("lorasrv")
	luci.http.redirect(dsp.build_url("admin","loraserver","app","edit",app) .. "?p=" .. page .. "&l=" .. limit .. "&find=" ..find)
	return
end