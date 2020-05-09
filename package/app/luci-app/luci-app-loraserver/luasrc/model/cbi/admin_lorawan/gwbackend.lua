local dsp = require "luci.dispatcher"
local datatype = require "luci.cbi.datatypes"
local fs = require "nixio.fs"
local devman = require "devman"


m = Map("lorasrv", translate("LoRa Network Server Gateway Overview"))

gateway=m:section(TypedSection,"gateway", translate("Gateway"))
gateway.addremove = true
gateway.dynamic = false
gateway.template = "cbi/tblsection"
gateway.sectionhead = "Gateway EUI"
gateway.extedit = dsp.build_url("admin", "loraserver", "gateway", "edit", "%s")

gateway:option(DummyValue, "name", translate("Name"))
gateway:option(DummyValue, "date", translate("Add time"))
gateway:option(DummyValue, "description", translate("Description"))
lastseen = gateway:option(DummyValue, "last_seen", translate("Last Seen"))

function lastseen.cfgvalue(self,section)
	local last_seen = 0
	local live_info = luci.util.exec("srvctrl -g %s" % { section })
    local live_data = luci.json.decode(live_info)
    local data = live_data and live_data["data"]
    if live_data then
    	last_seen = data.last_seen;
    end

    if last_seen == 0 then
		return translate("Never")
	end

	local diffs = os.time() - last_seen
	if diffs < 60 then
		sa = diffs .. " seconds ago"
	elseif diffs < 3600 then
		sa = math.modf(diffs/60) .. " minutes ago"
	elseif diffs < 3600*24 then
		sa = math.modf(diffs/3600) .. " hours ago"
	elseif diffs < 3600*24*7 then
		sa = math.modf(diffs/(3600*24)) .. " days ago"
	elseif diffs < 3600*24*30 then
		sa = math.modf(diffs/(3600*24*7)) .. " weeks ago"
	else
		sa = math.modf(diffs/(3600*24*30)) .. " months ago"
	end

    return sa;       
end

function gateway.create(self, section)
	if datatype.loraeui(section) == false then
		return false
	end
	m.uci:section("lorasrv", "gateway", section,{
		date = os.date()
		} )
	m.uci:save("lorasrv")
	luci.http.redirect(dsp.build_url("admin","loraserver","gateway","edit", section))
	return true
end

s = m:section(NamedSection, "gwbridge","gwbridge", translate("Gateway Backend Configuration"))
s:tab("general", translate("General Setup"))
s:tab("topic", translate("MQTT Topic"))


s:taboption("general",Value, "broker_ip", translate("MQTT Broker Address")).rmempty = false
s:taboption("general",Value, "broker_port", translate("MQTT Broker Port")).rmempty = false
MQTTVersion = s:taboption("general", ListValue, "mqtt_version", translate("MQTT Protocol Version"))
MQTTVersion:value("3.1", "V3.1")
MQTTVersion:value("3.1.1","V3.1.1")
MQTTVersion.default = "3.1"

s:taboption("topic",DummyValue, "rx_topic", translate("Uplink MQTT Topic"))
s:taboption("topic",DummyValue, "tx_topic", translate("Downlink MQTT Topic"))
s:taboption("topic",DummyValue, "ack_topic", translate("Downlink acknowledge MQTT Topic"))
s:taboption("topic",DummyValue, "stat_topic", translate("Gateway Statistic MQTT Topic"),translate("{{eui}} stands for LoRaGateway's EUI and must be included in the template"))

s:taboption("general", Value, "client_id", translate("Client ID"));
cs = s:taboption("general", Flag, "clean_session",translate("Clean Session"))
cs.default = "1";

rt = s:taboption("general", Flag, "retain", translate("Will Retain"))
rt.default = "0";

qos=s:taboption("general", ListValue, "qos", translate("Qos"));
qos:value("0", "0 - Almost Once")
qos:value("1", "1 - Atleast Once")
qos:value("2", "2 - Exactly Once")
qos.default = "1"

keepalive = s:taboption("general", Value, "keepalive", translate("keepalive"))
keepalive.default = '10'
keepalive.datatype = 'integer'

s:taboption("general",Flag, "auth_enable", translate("Enable User Authentication"))

usr=s:taboption("general",Value, "username", translate("Username"))
usr:depends("auth_enable", "1")
--usr.rmempty = false

pwd=s:taboption("general",Value, "password", translate("Password"))
pwd:depends("auth_enable","1")
--pwd.rmempty = false
pwd.password = true

tlsmode=s:taboption("general", ListValue, "tls_mode", translate("SSL/TLS Mode"));
tlsmode:value("none", "Disable")
tlsmode:value("ca_signed",translate("CA Signed server certificate"))
tlsmode:value("ca_cert", translate("Self-signed server certificate"))
tlsmode:value("self_signed", translate("Self-signed server & client certificate"))

tlsv=s:taboption("general",ListValue,"tls_version", translate("TLS Version"))
tlsv:value("tlsv1", translate("TLSv1"))
tlsv:value("tlsv1.1", translate("TLSv1.1"))
tlsv:value("tlsv1.2", translate("TLSv1.2"))
tlsv:depends("tls_mode", "ca_signed")
tlsv:depends("tls_mode", "ca_cert")
tlsv:depends("tls_mode", "self_signed")



ca_file = s:taboption("general",TextValue, "_ca_cert", translate("CA Certificate"))
ca_file.wrap    = "off"
ca_file.rows    = 6
ca_file:depends("tls_mode",'ca_cert')
ca_file:depends("tls_mode", "self_signed")

function ca_file.cfgvalue()
	return fs.readfile("/etc/lorasrv/gwca.crt") or ""
end

function ca_file.write(self, section, value)
	if value then
		fs.writefile("/etc/lorasrv/gwca.crt", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/lorasrv/gwca.crt")
	end
end

tls_file = s:taboption("general",TextValue, "_tls_cert", translate("Client Certificate"))
tls_file.wrap    = "off"
tls_file.rows    = 6

tls_file:depends("tls_mode", "self_signed")
function tls_file.cfgvalue()
	return fs.readfile("/etc/lorasrv/gwtls.crt") or ""
end

function tls_file.write(self, section, value)
	if value then
		fs.writefile("/etc/lorasrv/gwtls.crt", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/lorasrv/gwtls.crt")
	end
end
tls_key = s:taboption("general",TextValue, "_tls_key", translate("Client Key"))
tls_key.wrap    = "off"
tls_key.rows    = 6


tls_key:depends("tls_mode", "self_signed")
function tls_key.cfgvalue()
	return fs.readfile("/etc/lorasrv/gwtls.key") or ""
end


function tls_key.write(self, section, value)
	if value then
		fs.writefile("/etc/lorasrv/gwtls.key", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/lorasrv/gwtls.key")
	end
end
tls_key_pwd = s:taboption("general",Value, "tls_key_pwd", translate("Client Key Passphrase"))
tls_key_pwd:depends("tls_mode", "self_signed")
tls_key_pwd.password = true
function m.on_after_commit(map)
	local tls_key = map:formvalue("cbid.lorasrv.gwbridge._tls_key")
	local tls_cert = map:formvalue("cbid.lorasrv.gwbridge._tls_cert")
	local ca_cert = map:formvalue("cbid.lorasrv.gwbridge._ca_cert")
	if tls_key == "" then
		fs.writefile("/etc/lorasrv/gwtls.key", "")
	end
	if tls_cert == "" then
		fs.writefile("/etc/lorasrv/gwtls.crt", "")
	end

	if ca_cert == "" then
		fs.writefile("/etc/lorasrv/gwca.crt", "")
	end
end

return m