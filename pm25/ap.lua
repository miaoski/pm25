wifi.setmode(wifi.STATIONAP)
cfg  =  {}
cfg.ssid  =  "pm25-project"
cfg.pwd  =  "12345678"
wifi.ap.config(cfg)

print(wifi.ap.getip())

srv = net.createServer(net.TCP)
srv:listen(80, function(conn)
	conn:on("receive", function(conn, payload)
		setstate = "waiting"
		k = string.find(payload, "ssid")
		if k then
			str = string.sub(payload, k)
			-- print(str)
			m = string.find(str, "&")
			-- print(m)
			strssid = 'SSID = "'..string.sub(str, 6, (m-1))..'"'
			print(strssid)
			strpass = 'PASS = "'..string.sub(str, (m+10))..'"'
			print(strpass)

			file.open("config.lua", "w+")
			file.writeline(strssid)
			file.writeline(strpass)
			file.close()

			setstate = "configuration saved."
			print("store ok")
		end

		conn:send("HTTP/1.0 200 OK\nContent-type: text/html\nServer: ESP8266\n\n")
		conn:send("<html><head></head><body>")
		conn:send("<p>ESP8266-".. node.chipid().." - NODE.HEAP: ".. node.heap().."</p>\n")

		conn:send('<p style="color:red">Status: '..setstate.."</p>")
		conn:send("<FORM action = \"\" method = \"POST\">")
		conn:send("SSID: <input type = \"test\" name = \"ssid\">")
		conn:send("<br>")
		conn:send("Password: <input type = \"text\" name = \"password\">")
		conn:send("<br>")
		conn:send("<input type = \"submit\" value = \"Submit\">")
		conn:send("</form>")
		conn:send("</body>")
		conn:send("</html>")

	end)
end)
