module("luci.controller.sub-ghz", package.seeall)
function index()
	entry({"admin", "services", "sub-ghz"}, cbi("sub-ghz"), _("SUB GHZ"), 102)
end
