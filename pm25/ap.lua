wifi.setmode(wifi.STATIONAP)
cfg  =  {}
cfg.ssid  =  "pm25-project"
cfg.pwd  =  "12345678"
wifi.ap.config(cfg)

print(wifi.ap.getip())

srv = net.createServer(net.TCP)
srv:listen(80, function(conn)
	conn:on("receive", function(conn, payload)
		k = string.find(payload, "uuid")
		if k then
			str = string.sub(payload, k)
			m = string.find(str, "&")
			struuid = 'UUID="'..string.sub(str, 6, (m-1))..'"'
			n = string.find(str, "&", m+6)
			strssid = 'SSID="'..string.sub(str, m+6, (n-1))..'"'
			strpass = 'PASS="'..string.sub(str, (n+6))..'"'

			file.open("config.lua", "w")
			file.writeline(struuid)
			file.writeline(strssid)
			file.writeline(strpass)
			file.close()
			print("store ok")
		end
		conn:send("HTTP/1.0 200 OK\nContent-type: text/html\n\n")
		conn:send("<html><body>")
		conn:send("<p>ESP8266-".. node.chipid().." - NODE.HEAP: ".. node.heap().."</p>\n")
		conn:send("<FORM action=\"\" method=\"POST\">")
		conn:send("UUID: <input type=\"text\" name=\"uuid\"><br>")
		conn:send("SSID: <input type=\"text\" name=\"ssid\"><br>")
		conn:send("Pass: <input type=\"text\" name=\"pass\"><br>")
		conn:send("<input type=\"submit\" value=\"Submit\">")
		conn:send("</form>")
		conn:send("</body>")
		conn:send("</html>")
	end)
end)
