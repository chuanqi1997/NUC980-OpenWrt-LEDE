-- Licensed to the public under the Apache License 2.0.

local ds = require "luci.dispatcher"
local st = require "luci.tools.socat"
--local st = require "luci.tools.firewall"

m = Map("net_socat", translate("TCP/UDP - Port Forwards"),
	translate("TCP/UDP - Port Forwards Bidirectional forwarding."))

--
-- Port Forwards
--

s = m:section(TypedSection, "net_socat", translate("TCP/UDP Port Forwards Table"))
s.template  = "cbi/tblsection"
s.addremove = true
s.anonymous = true
s.sortable  = true
s.extedit   = ds.build_url("admin/services/net_socat/tcp-udp-forwards/%s")
s.template_addremove = "net_socat/cbi_add_tcp_udp_forward"



-- function 
function opt_source_name(s, t, ...)
	local o = s:option(t, "source_name", ...)

	function o.cfgvalue(self, section)
		return self.map:get(section, "source_name") or
			self.map:get(section, "_source_name") or "-"
	end

	function o.write(self, section, value)
		if value ~= "-" then
			self.map:set(section, "source_name", value)
			self.map:del(section, "_source_name")
		else
			self:remove(section)
		end
	end

	function o.remove(self, section)
		self.map:del(section, "source_name")
		self.map:del(section, "_source_name")
	end

	return o
end


function opt_target_name(s, t, ...)
	local o = s:option(t, "target_name", ...)

	function o.cfgvalue(self, section)
		return self.map:get(section, "target_name") or
			self.map:get(section, "_target_name") or "-"
	end

	function o.write(self, section, value)
		if value ~= "-" then
			self.map:set(section, "target_name", value)
			self.map:del(section, "_target_name")
		else
			self:remove(section)
		end
	end

	function o.remove(self, section)
		self.map:del(section, "target_name")
		self.map:del(section, "_target_name")
	end

	return o
end




function s.create(self, section)
	local sn = m:formvalue("_newfwd.source_name")
	local sz = m:formvalue("_newfwd.source_zone")
	local sp = m:formvalue("_newfwd.source_proto")
	local sa = m:formvalue("_newfwd.source_addr")
	local spt = m:formvalue("_newfwd.source_port")
	local sr = m:formvalue("_newfwd.source_rectime")

	local tn = m:formvalue("_newfwd.target_name")
	local tz = m:formvalue("_newfwd.target_zone")
	local tp = m:formvalue("_newfwd.target_proto")
	local ta = m:formvalue("_newfwd.target_addr")
	local tt = m:formvalue("_newfwd.target_port")
	local tr = m:formvalue("_newfwd.target_rectime")

	if sr == "" then
	sr = "5"
	end

	if tr == "" then
	tr = "5"	
	end

	if sp == "other" or (sp and sa) then
		created = TypedSection.create(self, section)

		
		
		self.map:set(created, "src",       tz or "wan")
		self.map:set(created, "dest",      sz or "lan")

		self.map:set(created, "source_name",      sn)
		self.map:set(created, "source_proto",sp)
		self.map:set(created, "source_port", spt)
		self.map:set(created, "source_addr", sa)
		self.map:set(created, "source_rectime", sr)

		self.map:set(created, "target_name",      tn)
		self.map:set(created, "target_proto",tp)
		self.map:set(created, "target_addr", ta)
		self.map:set(created, "target_port", tt)
		self.map:set(created, "target_rectime", tr)
	
	end

	if sp ~= "other" then
		created = nil
	end

end

function s.parse(self, ...)
	TypedSection.parse(self, ...)
	if created then
		m.uci:save("net_socat")
		luci.http.forward(ds.build_url(
			"admin/services/net_socat/net_socat", created
		))
	end
end

--[[
function s.filter(self, sid)
	return (self.map:get(sid, "target") ~= "SNAT")
end
-]]

--st.opt_name(s, DummyValue, translate("Source Name"))
opt_source_name(s, DummyValue, translate("Source Name"))

local function forward_proto_txt(self, s)
	return "%s - %s to %s" %{
		translate("IPv4"),
	    self.map:get(s, "source_proto"),
		self.map:get(s, "target_proto"),
	}
end

local function forward_src_txt(self, s)
	local z = st.fmt_zone(self.map:get(s, "src"), translate("any zone"))
--	local a = st.fmt_ip(self.map:get(s, "src_ip"), translate("any host"))
--	local p = st.fmt_port(self.map:get(s, "src_port"))
	local sa = st.fmt_ip(self.map:get(s, "source_addr"), translate("any host"))
	local p = st.fmt_port(self.map:get(s, "source_port"))
	local m = st.fmt_mac(self.map:get(s, "src_mac"))

	if p and m then
		return translatef("From %s in %s with source %s and %s", sa, z, p, m)
	elseif p or m then
		return translatef("From %s in %s with source %s", sa, z, p or m)
	else
		return translatef("From %s in %s", sa, z)
	end
end

local function forward_via_txt(self, s)
	local a = st.fmt_ip(self.map:get(s, "target_addr"), translate("any router IP"))
	local p = st.fmt_port(self.map:get(s, "target_port"))

	if p then
		return translatef("Via %s at %s", a, p)
	else
		return translatef("Via %s", a)
	end
end

match = s:option(DummyValue, "match", translate("Match"))
match.rawhtml = true
match.width   = "50%"
function match.cfgvalue(self, s)
--	return "<small>%s<br />%s<br />%s</small>" % {
	return "<small>%s<br />%s<br /></small>" % {
		forward_proto_txt(self, s),
		forward_src_txt(self, s),
--		forward_via_txt(self, s)
	}
end

--st.opt_target_name(s, DummyValue, translate("Target Name"))
opt_target_name(s, DummyValue, translate("Target Name"))

dest = s:option(DummyValue, "dest", translate("Forward to"))
dest.rawhtml = true
dest.width   = "40%"
function dest.cfgvalue(self, s)
	local z = st.fmt_zone(self.map:get(s, "dest"), translate("any zone"))
	local a = st.fmt_ip(self.map:get(s, "target_addr"), translate("any host"))
	local p = st.fmt_port(self.map:get(s, "target_port"))

	if p then
		return translatef("%s, %s in %s", a, p, z)
	else
		return translatef("%s in %s", a, z)
	end
end

st.opt_enabled(s, Flag, translate("Enable")).width = "1%"



--[[
local apply = luci.http.formvalue("cbi.apply")
if apply then
	io.popen("/etc/init.d/net_socat restart")
end
--]]

return m
