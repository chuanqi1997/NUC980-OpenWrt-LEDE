module("luci.controller.admin.lorawan", package.seeall)

function index()
	entry({"admin", "lorawan"}, alias("admin", "lorawan", "packet_forwarder"), _("LoRa Gateway"),40)
	entry({"admin", "lorawan","packet_forwarder"}, cbi("admin_lorawan/lorawan"), _("LoRa Packet Forwarder"), 10)
	entry({"admin", "lorawan","packet_forwarder","add_sx1301"},call("add_sx1301")).leaf = true
	entry({"admin", "lorawan","packet_forwarder","rm_sx1301"},call("rm_sx1301")).leaf = true	
	--entry({"admin", "lorawan","packet_filter"}, cbi("admin_lorawan/filter"), _("LoRa Packet Filter"),15)

	entry({"admin", "status", "loragw"}, template("lorawan/status"), _("LoRa Packet Logger"),2).leaf = ture
	entry({"admin","lorawan","freq_plan"}, call("action_freq_plan")).leaf = true

end

function action_freq_plan( band )
	os.execute(". /lib/functions/lora_freq_plan.sh; " .. band )
	--os.execute("uci commit lora_pkt_fwd")
	--os.execute("/etc/init.d/lora_pkt_fwd reload")
	luci.http.redirect(luci.dispatcher.build_url("admin","lorawan"))
end

function rm_sx1301( name )
	local uci = require "luci.model.uci".cursor()

	uci:load("lora_pkt_fwd")
	uci:delete("lora_pkt_fwd", name)
	uci:save("lora_pkt_fwd")

	luci.http.redirect(luci.dispatcher.build_url("admin","lorawan","packet_forwarder"))
	return
end

function add_sx1301( name )
	local uci = require "luci.model.uci".cursor()
	local sx1301
	uci:load("lora_pkt_fwd")
	sx1301 = uci:get("lora_pkt_fwd", name)
	radio_type = uci:get("lora_pkt_fwd","sx1301_0","radio_0_type")
	if sx1301 then
		luci.http.redirect(luci.dispatcher.build_url("admin","lorawan","packet_forwarder"))
	end
	local values = {
		spi_type='usb',
		reset_pin='47',
		lorawan_public='1',
		clksrc='1',
		antenna_gain='0',
		radio_0_enable='1',
		radio_0_type = radio_type,
		radio_0_tx_enable='1',
		radio_1_enable='1',
		radio_1_type = radio_type,
		radio_1_tx_enable='0'
	}

	sx1301 = uci:section("lora_pkt_fwd","sx1301", name, values)
	uci:save("lora_pkt_fwd")
	luci.http.redirect(luci.dispatcher.build_url("admin","lorawan","packet_forwarder"))
	return
end