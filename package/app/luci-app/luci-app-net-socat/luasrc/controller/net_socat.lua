module("luci.controller.net_socat", package.seeall)
function index()
--	entry({"admin", "services", "socat"}, cbi("socat"), _("socat"), 102)
	entry({"admin", "services", "net_socat"},alias("admin", "services", "net_socat", "tcp-udp-forwards"),_("Net Socat"), 60)
	entry({"admin", "services", "net_socat", "tcp-udp-forwards"},
		arcombine(cbi("net_socat/tcp-udp-forwards"), cbi("net_socat/tcp-udp-forwards-details")),
		_("TCP/UDP"), 10).leaf = true
end
