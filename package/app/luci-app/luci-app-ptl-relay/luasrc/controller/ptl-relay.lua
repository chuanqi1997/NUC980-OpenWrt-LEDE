module("luci.controller.ptl-relay", package.seeall)
function index()

	entry({"admin", "services", "ptl-relay"}, alias("admin", "services", "ptl-relay", "collect"), _("Ptl Relay"), 60)
	entry({"admin", "services", "ptl-relay", "collect"}, arcombine(cbi("ptl-relay/collect"), cbi("ptl-relay/collect-details")), _("COLLECT"), 10).leaf = true
	entry({"admin", "services", "ptl-relay", "forward"}, arcombine(cbi("ptl-relay/forward"), cbi("ptl-relay/forward-details")), _("FORWARD"), 20).leaf = true

--[[
	entry({"admin", "services", "ptl-relay"},alias("admin", "services", "ptl-relay", "zones"),_("Firewall"), 60)

	entry({"admin", "services", "ptl-relay", "zones"},
		arcombine(cbi("ptl-relay/zones"), cbi("ptl-relay/zone-details")),
		_("General Settings"), 10).leaf = true

	entry({"admin", "services", "ptl-relay", "forwards"},
		arcombine(cbi("ptl-relay/forwards"), cbi("ptl-relay/forward-details")),
		_("Port Forwards"), 20).leaf = true

	entry({"admin", "services", "ptl-relay", "rules"},
		arcombine(cbi("ptl-relay/rules"), cbi("ptl-relay/rule-details")),
		_("Traffic Rules"), 30).leaf = true

	entry({"admin", "services", "ptl-relay", "custom"},
		form("ptl-relay/custom"),
		_("Custom Rules"), 40).leaf = true
--]]

end
