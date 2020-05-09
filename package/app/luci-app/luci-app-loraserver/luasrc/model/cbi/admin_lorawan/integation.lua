local fs = require "nixio.fs"
local devman = require "devman"
local lic = devman.sys.license()
local mote_max = lic and lic["mote"] or 128
local gw_max = lic and lic["gateway"] or 5

m = Map("lorasrv", translate("Application Server Integration"))
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
s = m:section(NamedSection, "lorasrv", "lorasrv", translate("Application Server Integration") )

s:tab("general", translate("General Setup"))
s:tab("topic", translate("MQTT Topic template Setup"), "All topics support one or more of the four variable fields {{application_ID}} {{application_name}} {{application_EUI}} {{device_EUI}}. The variable field must be in a separate domain (the content between the two '/'s is a separate domain). All topics must contain the {{device_EUI}} variable field.")

join=s:taboption("topic", Value, "topic_join", translate("Join Topic"), translate("Event published when a device joins the network."))
join.default="application/{{application_ID}}/device/{{device_EUI}}/join"

up=s:taboption("topic", Value, "topic_uplink", translate("Uplink Topic"), translate("Contains the data and meta-data for an uplink application payload."))
up.default="application/{{application_ID}}/device/{{device_EUI}}/rx"

dw=s:taboption("topic", Value, "topic_downlink", translate("Downlink Topic"), translate("Scheduling downlink data by application server"))
dw.default="application/{{application_ID}}/device/{{device_EUI}}/tx"

ack=s:taboption("topic", Value, "topic_ack", translate("Ack Topic"), translate("Event published on downlink frame acknowledgements."))
ack.default="application/{{application_ID}}/device/{{device_EUI}}/ack"

st=s:taboption("topic", Value, "topic_status", translate("Status Topic"),translate("Event for battery and margin status received from devices."))
st.default="application/{{application_ID}}/device/{{device_EUI}}/status"


s:taboption("general", Value, "broker_ip", translate("MQTT Broker Address"))
s:taboption("general", Value, "broker_port", translate("MQTT Broker Port"))

MQTTVersion = s:taboption("general", ListValue, "mqtt_version", translate("MQTT Protocol Version"))
MQTTVersion:value("3.1", "V3.1")
MQTTVersion:value("3.1.1","V3.1.1")
MQTTVersion.default = "3.1"

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

s:taboption("general", Flag, "auth_enable", translate("Enable User Authentication"))
usr=s:taboption("general",Value, "username", translate("Username"))
usr:depends("auth_enable", "1")

pwd=s:taboption("general",Value, "password", translate("Password"))
pwd:depends("auth_enable","1")
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

ca_file:depends("tls_mode", "ca_cert")
ca_file:depends("tls_mode", "self_signed")
function ca_file.cfgvalue()
	return fs.readfile("/etc/lorasrv/ca.crt") or ""
end

function ca_file.write(self, section, value)
	if value then
		fs.writefile("/etc/lorasrv/ca.crt", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/lorasrv/ca.crt")
	end
end

tls_file = s:taboption("general",TextValue, "_tls_cert", translate("Client Certificate"))
tls_file.wrap    = "off"
tls_file.rows    = 6
tls_file:depends("tls_mode", "self_signed")
function tls_file.cfgvalue()
	return fs.readfile("/etc/lorasrv/tls.crt") or ""
end

function tls_file.write(self, section, value)
	if value then
		fs.writefile("/etc/lorasrv/tls.crt", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/lorasrv/tls.crt")
	end
end
tls_key = s:taboption("general",TextValue, "_tls_key", translate("Client Key"))
tls_key.wrap    = "off"
tls_key.rows    = 6
tls_key:depends("tls_mode", "self_signed")
function tls_key.cfgvalue()
	return fs.readfile("/etc/lorasrv/tls.key") or ""
end


function tls_key.write(self, section, value)
	if value then
		fs.writefile("/etc/lorasrv/tls.key", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/lorasrv/tls.key")
	end
end

tls_key_pwd = s:taboption("general",Value, "tls_key_pwd", translate("Client Key Passphrase"))
tls_key_pwd:depends("tls_mode", "self_signed")
tls_key_pwd.password = true
function m.on_after_commit(map)
	local tls_key = map:formvalue("cbid.lorasrv.lorasrv._tls_key")
	local tls_cert = map:formvalue("cbid.lorasrv.lorasrv._tls_cert")
	local ca_cert = map:formvalue("cbid.lorasrv.lorasrv._ca_cert")
	if tls_key == "" then
		fs.writefile("/etc/lorasrv/tls.key", "")
	end
	if tls_cert == "" then
		fs.writefile("/etc/lorasrv/tls.crt", "")
	end

	if ca_cert == "" then
		fs.writefile("/etc/lorasrv/ca.crt", "")
	end
end
return m
