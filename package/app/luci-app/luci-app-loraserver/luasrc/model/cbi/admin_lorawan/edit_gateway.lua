local dsp = require "luci.dispatcher"
m = Map("lorasrv", translate("Gateway Edit - ") .. arg[1]  )

m.redirect = dsp.build_url("admin","loraserver","gateway")
s = m:section(NamedSection, arg[1], "gateway", "Gateway " .. arg[1])

o=s:option(Value, "name", translate("Name"))
o=s:option(Value, "description", translate("Description"))
o=s:option(Value, "latitude", translate("Latitude"))
o=s:option(Value, "longitude", translate("Longitude"))
o=s:option(Value, "altitude", translate("Altitude"))
return m