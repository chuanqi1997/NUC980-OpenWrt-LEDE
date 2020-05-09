local fs = require "nixio.fs"

m = Map("loragwbr", translate("LoRa Gateway MQTT Bridge"))

s = m:section(NamedSection,"gwbridge","gwbridge", translate("LoRa Gateway MQTT Bridge Configuration"))
s:tab("general", translate("General Setup"))
s:tab("topic", translate("MQTT Topic Template Setup"))

en = s:taboption("general", Flag, "enable", translate("Enable"))
en.default = "1"

server = s:taboption("general", ListValue, "proto", translate("LoRa Network Server Type"))
server:value("build-in", "Build-in LoRa Network Server")
server:value("json-v2", "loraserver 2.x")
server:value("protobuf", "loraserver 3.x (PROTOBUF)")
server:value("json-v3", "loraserver 3.x (JSON-V3)")

server.default = "build-in"
server.template = "cbi/gwbrsrv"
s:taboption("general",Value, "broker_ip", translate("MQTT Broker Address"))
s:taboption("general",Value, "broker_port", translate("MQTT Broker Port"))


s:taboption("topic",Value, "rx_topic", translate("Uplink MQTT Topic"))
s:taboption("topic",Value, "tx_topic", translate("Downlink MQTT Topic"))
s:taboption("topic",Value, "ack_topic", translate("Downlink acknowledge MQTT Topic"))
s:taboption("topic",Value, "stat_topic", translate("Gateway Statistic MQTT Topic"),translate("{{eui}} stands for LoRaGateway's EUI and must be included in the template"))

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

s:taboption("general",Flag, "auth_enable", translate("Enable User Authentication"))
usr=s:taboption("general",Value, "username", translate("Username"))
pwd=s:taboption("general",Value, "password", translate("Password"))
usr:depends("auth_enable", "1")
pwd:depends("auth_enable", "1")
pwd.password = true

tlsmode=s:taboption("general", ListValue, "tls_mode", translate("SSL/TLS Mode"));
tlsmode:value("none", "Disable")
tlsmode:value("ca_signed","CA signed server certificate")
tlsmode:value("ca_cert", "Self-signed server certificate")
tlsmode:value("self_signed", "Self-signed server & client certificate")


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
	return fs.readfile("/etc/gwbridge/ca.crt") or ""
end

function ca_file.write(self, section, value)
	if value then
		fs.writefile("/etc/gwbridge/ca.crt", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/gwbridge/ca.crt")
	end
end

tls_file = s:taboption("general",TextValue, "_tls_cert", translate("Client Certificate"))
tls_file.wrap    = "off"
tls_file.rows    = 6
tls_file:depends("tls_mode", "self_signed")
function tls_file.cfgvalue()
	return fs.readfile("/etc/gwbridge/tls.crt") or ""
end

function tls_file.write(self, section, value)
	if value then
		fs.writefile("/etc/gwbridge/tls.crt", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/gwbridge/tls.crt")
	end
end
tls_key = s:taboption("general",TextValue, "_tls_key", translate("Client Key"))
tls_key.wrap    = "off"
tls_key.rows    = 6
tls_key:depends("tls_mode", "self_signed")
function tls_key.cfgvalue()
	return fs.readfile("/etc/gwbridge/tls.key") or ""
end


function tls_key.write(self, section, value)
	if value then
		fs.writefile("/etc/gwbridge/tls.key", value:gsub("\r\n", "\n"))
	else
		fs.remove("/etc/gwbridge/tls.key")
	end
end

tls_key_pwd = s:taboption("general",Value, "tls_key_pwd", translate("Client Key Passphrase"))
tls_key_pwd:depends("tls_mode", "self_signed")
tls_key_pwd.password = true

function m.on_after_commit(map)
	local tls_key = map:formvalue("cbid.loragwbr.gwbridge._tls_key")
	local tls_cert = map:formvalue("cbid.loragwbr.gwbridge._tls_cert")
	local ca_cert = map:formvalue("cbid.loragwbr.gwbridge._ca_cert")
	if tls_key == "" then
		fs.writefile("/etc/gwbridge/tls.key", "")
	end
	if tls_cert == "" then
		fs.writefile("/etc/gwbridge/tls.crt", "")
	end

	if ca_cert == "" then
		fs.writefile("/etc/gwbridge/ca.crt", "")
	end
end

return m