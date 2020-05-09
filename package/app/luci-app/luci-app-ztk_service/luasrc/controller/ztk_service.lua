module("luci.controller.ztk_service", package.seeall)
function index()





	entry({"admin", "services", "ztk_service"}, alias("admin", "services", "ztk_service", "config"), _("ZTK Service"), 60)
	--entry({"admin", "services", "ztk_service"}, cbi("ztk_service/ztk_service"), _("ZTK Service"), 60)
	--entry({"admin", "services", "ztk_service"}, template("ztk_service/run_status"),_("Run log"), 70)
	entry({"admin", "services","ztk_service", "config"}, cbi("ztk_service/ztk_service"), _("Server Config"), 10).leaf = true
	--entry({"admin", "services","ztk_service", "log"}, cbi("ztk_service/run_status"), _("RunLog"), 20).leaf = true
	entry({"admin", "services","ztk_service", "runlog"}, template("ztk_service/run_status"), _("Run Log"), 30).leaf = true

	--entry({"admin", "services", "ztk_service"}, arcombine(cbi("ztk_service/ztk_service"), cbi("ztk_service/ztk_service_log")), _("ZTK Service"), 60)

	--entry({"admin", "services", "ztk_service", "collect"}, arcombine(cbi("ztk_service/collect"), cbi("ztk_service/collect-details")), _("COLLECT"), 10).leaf = true
	--entry({"admin", "services", "ztk_service", "forward"}, arcombine(cbi("ztk_service/forward"), cbi("ztk_service/forward-details")), _("FORWARD"), 20).leaf = true

--[[
function index()
	entry({"admin", "status"}, alias("admin", "status", "overview"), _("Status"), 20).index = true
	entry({"admin", "status", "overview"}, template("admin_status/index"), _("Overview"), 1)

	entry({"admin", "status", "iptables"}, template("admin_status/iptables"), _("Firewall"), 2).leaf = true
	entry({"admin", "status", "iptables_action"}, post("action_iptables")).leaf = true

	entry({"admin", "status", "routes"}, template("admin_status/routes"), _("Routes"), 3)
	entry({"admin", "status", "syslog"}, call("action_syslog"), _("System Log"), 4)
	entry({"admin", "status", "dmesg"}, call("action_dmesg"), _("Kernel Log"), 5)
	entry({"admin", "status", "processes"}, form("admin_status/processes"), _("Processes"), 6)

	entry({"admin", "status", "realtime"}, alias("admin", "status", "realtime", "load"), _("Realtime Graphs"), 7)

	entry({"admin", "status", "realtime", "load"}, template("admin_status/load"), _("Load"), 1).leaf = true
	entry({"admin", "status", "realtime", "load_status"}, call("action_load")).leaf = true

	entry({"admin", "status", "realtime", "bandwidth"}, template("admin_status/bandwidth"), _("Traffic"), 2).leaf = true
	entry({"admin", "status", "realtime", "bandwidth_status"}, call("action_bandwidth")).leaf = true

	if nixio.fs.access("/etc/config/wireless") then
		entry({"admin", "status", "realtime", "wireless"}, template("admin_status/wireless"), _("Wireless"), 3).leaf = true
		entry({"admin", "status", "realtime", "wireless_status"}, call("action_wireless")).leaf = true
	end

	entry({"admin", "status", "realtime", "connections"}, template("admin_status/connections"), _("Connections"), 4).leaf = true
	entry({"admin", "status", "realtime", "connections_status"}, call("action_connections")).leaf = true

	entry({"admin", "status", "nameinfo"}, call("action_nameinfo")).leaf = true
end
--]]

end
