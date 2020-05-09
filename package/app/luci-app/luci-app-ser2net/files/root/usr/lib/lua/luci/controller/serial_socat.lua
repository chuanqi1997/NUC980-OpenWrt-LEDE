module("luci.controller.serial_socat", package.seeall)
function index()
	entry({"admin", "services", "serial_socat"}, cbi("serial_socat"), _("Serial to Net"), 102)
end
