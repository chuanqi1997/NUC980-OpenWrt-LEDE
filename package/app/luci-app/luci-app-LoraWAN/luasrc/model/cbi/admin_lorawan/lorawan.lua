local lora_n = luci.sys.exec("devman -d lora")
local devman = require "devman"
local dsp = require "luci.dispatcher"
local has_loraserver = nixio.fs.access("/etc/config/loragwbr")
m = Map("lora_pkt_fwd", translate("LoRa Packet Forwarder"), translate("LoraWAN Packet Forwarder Configuration"))
m:chain("lorasrv")
m:chain("loragwbr")

if devman.sys.basemodel() == "RAK7249" then
	if not m.uci:get("lora_pkt_fwd", "sx1301_1")  then 
		m.action = {
			name = translate("Add LoRa Concentrator"),
			url = dsp.build_url("admin/lorawan/packet_forwarder/add_sx1301/sx1301_1")
		}
	else
		m.action = {
			name = translate("Remove LoRa Concentrator 1"),
			url = dsp.build_url("admin/lorawan/packet_forwarder/rm_sx1301/sx1301_1")
		}

	end
end
s = m:section(NamedSection, "gateway_conf", "gateway_conf", translate("Gateway Configuration"))

s:tab("general", translate("General Setup"))
if devman.sys.basemodel() ~= "RAK7258" then
	s:tab("beacon", translate("Beacon Setup"))
end

s:tab("filter", translate("Packet Filter"))
s:tab("gps", translate("GPS Information"))

white_enable = s:taboption("filter", Flag, "white_enable", translate("Enable White List Mode"))

ouis_list = s:taboption("filter", DynamicList, "white_ouis", translate("OUI List"))
netid_list = s:taboption("filter", DynamicList, "white_netids", translate("Network ID List"))
auto_filter = s:taboption("filter", Flag, "filter_enable", translate("Enable Auto Filter"))
bl_duration = s:taboption("filter", Value, "bl_duration", translate("Discard Period"), translate("Period threshold of Discard time for Nodes (in seconds)"))
bl_duration.default = 1800
join_period = s:taboption("filter", Value, "join_period", translate("Join Period"),translate("Period threshold of Statistics on the latest  join request (in seconds)"))
join_period.default = 1800
join_interval=s:taboption("filter", Value, "join_interval", translate("Join Interval"), translate("Time interval threshold of the same device EUI twice consecutive join request  (in seconds)"))
join_interval.default = 6

join_count1 = s:taboption("filter", Value, "join_count1", translate("Join Count 1"), translate("Maximum count of join request allowed during Join Interval"))
join_count1.default = 5
join_count2 = s:taboption("filter", Value, "join_count2", translate("Join Count 2"), translate("Maximum count of join request allowed during Join Period"))
join_count2.default = 20
gateway_ID = s:taboption("general", Value, "gateway_ID", translate("Gateway EUI"))

if has_loraserver then
	proto = s:taboption("general", ListValue, "proto", translate(" Protocol"))

	proto:value("udp", translate("Semtech UDP GWMP Protocol"))
	proto:value("mqtt", translate("LoRa Gateway MQTT Bridge"))
	proto:value("ins", translate("Build-in LoRa Server"))
	proto.widget="radio"
	proto.template = "cbi/lorawan_proto"
end

server_address = s:taboption("general",Value, "server_address", translate("Server Address"))
if has_loraserver then
	server_address:depends("proto", "udp");
end

serv_port_up = s:taboption("general",Value, "serv_port_up", translate("Server Port Up"))
serv_port_up.datatype = "port"

if has_loraserver then
	serv_port_up:depends("proto", "udp");
end
serv_port_down = s:taboption("general",Value, "serv_port_down", translate("Server Port Down"))
serv_port_down.datatype = "port"

if has_loraserver then
	serv_port_down:depends("proto", "udp");
end

push_timeout_ms = s:taboption("general", Value, "push_timeout_ms", translate("Push Timeout (ms)"))


stat_interval = s:taboption("general", Value, "stat_interval", translate("Statistic Interval (s)"))

keepalive_interval = s:taboption("general", Value, "keepalive_interval", translate("Keepalive Interval (s)"));
keepalive_interval.default = "5"

data_recovery = s:taboption("general",Flag, "data_recovery", translate("Automatic data recovery"),translate("Data messages are automatically stored when the connection to the server is lost"))
data_recovery.default = "0"

auto_quit = s:taboption("general", Value, "autoquit_threshold", translate("Auto-restart Threshold"),translate("Packet forwarder will automatically restart when the keepalive timeout exceeds this threshold. Set '0' to disable"))
auto_quit.default = "30"

dbglvl = s:taboption("general", ListValue, "log_level", translate("Log Level"))
dbglvl:value(3, "ERROR")
dbglvl:value(4, "WARNING")
dbglvl:value(5, "NOTICE")
dbglvl:value(6, "INFO")
dbglvl:value(7, "DEBUG")
dbglvl.default = 5
--auto_quit:depends("data_recovery","1")
--network_failure_threshold = s:taboption("general", Value, "network_failure_threshold", translate("Network failure threshold"))

freq_plan = s:taboption("general", DummyValue,"__freq_plan", translate("Import Frequency Plan Template"))
freq_plan.value = "AS923"
freq_plan.template = "lorawan/freq_plan"

fm = s:taboption("general", Button, "__toggle_freq_conf_mode");
if m:get("gateway_conf", "freq_conf_mode") == "standard" then
	fm.title = translate("Standard Frquency Setup Mode")
	fm.inputtitle = translate("Switch to Advanced Mode")
	fm.inputstyle = "apply"
else
	fm.title = translate("Advanced Frquency Setup Mode")
	fm.inputtitle = translate("Switch to Standard Mode")
	fm.inputstyle = "apply"
end

if m:formvalue("cbid.lora_pkt_fwd.gateway_conf.__toggle_freq_conf_mode") then
	if m:get("gateway_conf", "freq_conf_mode") == "standard" then
		m:set("gateway_conf", "freq_conf_mode","advanced")
	else
		m:set("gateway_conf", "freq_conf_mode","standard")
	end
	m.uci:save("lora_pkt_fwd")
	luci.http.redirect(luci.dispatcher.build_url("admin","lorawan","packet_forwarder"))
	return
end

if devman.sys.basemodel() ~= "RAK7258" then
	beacon_period = s:taboption("beacon",Value, "beacon_period", translate("Beacon Period"))


	beacon_freq_hz = s:taboption("beacon",Value, "beacon_freq_hz", translate("Beacon Frequency (Hz)"))


	beacon_freq_nb = s:taboption("beacon",Value, "beacon_freq_nb", translate("Beacon Channel Number"))


	beacon_freq_step = s:taboption("beacon",Value, "beacon_freq_step", translate("Beacon Frequency Step"))


	beacon_datarate = s:taboption("beacon",ListValue, "beacon_datarate", translate("Beacon Datarate"))
	beacon_datarate:value(8, "SF8")
	beacon_datarate:value(9, "SF9")
	beacon_datarate:value(10, "SF10")
	beacon_datarate:value(12, "SF12")

	beacon_bw_hz = s:taboption("beacon",Value, "beacon_bw_hz", translate("Beacon Bandwidth"))


	beacon_power = s:taboption("beacon",Value, "beacon_power", translate("Beacon Tx Power"))


	beacon_infodesc = s:taboption("beacon",Value, "beacon_infodesc", translate("Beaconing information descriptor"))
end

fake_gps = s:taboption("gps", Flag, "fake_gps", translate("Fake GPS"))


latitude = s:taboption("gps",Value, "latitude", translate("Latitude"))
latitude.datatype = "float"

longitude = s:taboption("gps",Value, "longitude", translate("Longitude"))
longitude.datatype = "float"

altitude = s:taboption("gps",Value, "altitude", translate("Altitude"))
altitude.datatype = "float"

local sx1301_nb = 0;
m.uci:foreach("lora_pkt_fwd","sx1301",function(sec)
		sx1301_nb = sx1301_nb + 1;
end)

for i=0,sx1301_nb - 1,1 do
	if m:formvalue("cbid.lora_pkt_fwd.sx1301_"..i..".__toggle") then
		if m:get("sx1301_".. i,"disabled") ~= "1" then
			m:set("sx1301_".. i,"disabled","1")
		else
			m:set("sx1301_".. i,"disabled","0")
		end
		m.uci:save("lora_pkt_fwd")
		m.uci:apply("lora_pkt_fwd")
		luci.http.redirect(luci.dispatcher.build_url("admin/lorawan"))
		return m
	end
end
local band=devman.sys.band()

if m:get("gateway_conf", "freq_conf_mode") == "standard" then

	s = m:section(NamedSection, "freq_plan", "freq_plan", translate("Frequency Plan"))

	region = s:option(ListValue, "region", translate("Region"))
	if string.find(band, "923") or string.find(band, "915") or string.find(band, "920") or string.find(band, "902") or string.find(band, "928") then
		region:value("US915", "US902-928")
		region:value("AU915", "AU915-928")
		region:value("AS923", "AS923")
		region:value("KR920", "KR920-923")
	end

	if string.find(band, "470") or string.find(band, "510") then 
		region:value("CN470", "CN470-510")
	end

	if string.find(band, "868") or string.find(band, "865") or string.find(band, "867") or string.find(band, "870") or string.find(band, "863") then 
		region:value("EU868", "EU863-870")
		region:value("IN865", "IN865-867")
		region:value("RU864", "RU864-870")
	end
	
	if string.find(band, "433") then
		region:value("EU433", "EU433")
	end
	
	

	public = s:option(Flag, "lorawan_public", translate("LoRaWAN Public"))
	public.default = '1'

	fsb = s:option(Value, "FSB", translate('Frequency Sub-Band'))
	fsb.template = 'cbi/FSB'
	fsb.default = 'custom'
	freqs = s:option(DynamicList, "freq", translate("Multi-SF LoRa Channel"))
	freqs.template = "cbi/freq_list"
	freqs.rmempty = false

	loraStd = s:option(DynamicList, "lora_std", translate("Standard LoRa Channel"))
	loraStd.template="cbi/lora_std_freq"

	fskChan = s:option(DynamicList, "fsk",translate("FSK Channel"))
	fskChan.template = "cbi/fsk_freq"

	if m:formvalue("cbi.submit") then
		local lorawan_public = m:formvalue("cbid.lora_pkt_fwd.freq_plan.lorawan_public")
		local form_region = m:formvalue("cbid.lora_pkt_fwd.freq_plan.region")
		
		m:set("sx1301_0", "radio_0_tx_freq_min", m:formvalue("_tx_min"));
		m:set("sx1301_0", "radio_0_tx_freq_max", m:formvalue("_tx_max"));

		for sn=0,sx1301_nb-1,1 do
			m:set("sx1301_" .. sn, "disabled", m:formvalue("_"..sn.."_disabled"))
			if lorawan_public == "1" then
				m:set("sx1301_" .. sn, "lorawan_public", "1")
			else
				m:set("sx1301_" .. sn, "lorawan_public", "0")
			end

			m:set("sx1301_" .. sn, "radio_0_enable", m:formvalue("_"..sn.."_radio0_enable"))
			m:set("sx1301_" .. sn, "radio_1_enable", m:formvalue("_"..sn.."_radio1_enable"))
			m:set("sx1301_" .. sn, "radio_0_freq", m:formvalue("_"..sn.."_radio0_center_freq"))
			m:set("sx1301_" .. sn, "radio_1_freq", m:formvalue("_"..sn.."_radio1_center_freq"))
			for cn=0,7,1 do
				m:set("sx1301_"..sn, "chan_multiSF_" .. cn .. "_enable", m:formvalue("_"..sn.."_ch"..cn.."_enable"))
				m:set("sx1301_"..sn, "chan_multiSF_" .. cn .. "_radio", m:formvalue("_"..sn.."_ch"..cn.."_radio"))
				m:set("sx1301_"..sn, "chan_multiSF_" .. cn .. "_if", m:formvalue("_"..sn.."_ch"..cn.."_if"))
			end
			m:set("sx1301_" .. sn, "chan_Lora_std_enable", m:formvalue("_" .. sn .."_std_enable"))
			m:set("sx1301_" .. sn, "chan_Lora_std_radio",  m:formvalue("_" .. sn .."_std_radio"))
			m:set("sx1301_" .. sn, "chan_Lora_std_if",  m:formvalue("_" .. sn .."_std_if"))
			m:set("sx1301_" .. sn, "chan_Lora_std_bandwidth",  m:formvalue("_" .. sn .."_std_bw"))
			m:set("sx1301_" .. sn, "chan_Lora_std_spread_factor",  m:formvalue("_" .. sn .."_std_sf"))

			m:set("sx1301_" .. sn, "chan_FSK_enable", m:formvalue("_" .. sn .."_fsk_enable"))
			m:set("sx1301_" .. sn, "chan_FSK_radio",  m:formvalue("_" .. sn .."_fsk_radio"))
			m:set("sx1301_" .. sn, "chan_FSK_if",  m:formvalue("_" .. sn .."_fsk_if"))
			m:set("sx1301_" .. sn, "chan_FSK_bandwidth",  125000)
			m:set("sx1301_" .. sn, "chan_FSK_datarate",  m:formvalue("_" .. sn .."_fsk_bps"))
		end
	end

else
	if m:formvalue("cbi.submit") then
		for sn=0,sx1301_nb-1,1 do
			m:set("sx1301_" .. sn, "radio_0_enable", "1")
			m:set("sx1301_" .. sn, "radio_1_enable", "1")
		end
	end

	for i=0,sx1301_nb - 1,1 do
		s = m:section(NamedSection, "sx1301_" .. i, "sx1301", translate("LoRa Concentrator " .. i))
		s.legend="LoRa Concentrator " .. i

		if m:get("sx1301_".. i,"disabled") == "1" then
			en = s:option(Button, "__toggle")
			en.title      = translate("LoRa Concentrator 1 is disabled")
			en.inputtitle = translate("Enable")
			en.inputstyle = "apply"
		else
			s.template="cbi/lora_wan"
			s:option( Value, "radio_0_freq", translate("Radio 0 Center Frequency"))
			s:option( Value, "radio_1_freq", translate("Radio 1 Center Frequency"))

			s:option( Value, "radio_0_tx_freq_min", translate("Minimum Tx Frequency"))
			s:option( Value, "radio_0_tx_freq_max", translate("Maximum Tx Frequency"))
			public = s:option(Flag, "lorawan_public", translate("LoRaWAN Public"))
			public.default = '1'

			s:tab("chan","channel configuration")
			for i=0,7,1 do
				s:option( Value, "chan_multiSF_" .. i .. "_enable", translate("chan_multiSF_" .. i .." enable"))
				s:option( Value, "chan_multiSF_" .. i .. "_radio", translate("chan_multiSF_" .. i .." radio"))
				s:option( Value, "chan_multiSF_" .. i .. "_if", translate("chan_multiSF_" .. i .." if"))
			end

			s:option( Value, "chan_Lora_std_enable", translate("chan_Lora_std enable"))
			s:option( Value, "chan_Lora_std_radio", translate("chan_Lora_std radio"))

			s:option( Value, "chan_Lora_std_if", translate("chan_Lora_std if"))
			s:option( Value, "chan_Lora_std_bandwidth", "chan_Lora_std Bandwidth")
			s:option( Value, "chan_Lora_std_spread_factor", "chan_Lora_std Spread Factor")


			s:option( Value, "chan_FSK_enable", translate("chan_FSK enable"))
			s:option( Value, "chan_FSK_radio", translate("chan_FSK radio"))
			s:option( Value, "chan_FSK_if", translate("chan_FSK if"))
			s:option( Value, "chan_FSK_bandwidth", "chan_FSK Bandwidth")
			s:option( Value, "chan_FSK_datarate", "chan_FSK Datarate")
		end
	end

end

return m