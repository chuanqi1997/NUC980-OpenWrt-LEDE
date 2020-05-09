module("luci.controller.ser2net", package.seeall)
function index()
	entry({"admin", "services", "ser2net"}, cbi("ser2net"), _("ser2net"), 102)
	end
