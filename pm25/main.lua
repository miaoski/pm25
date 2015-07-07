print ">>> main.lua"
http = require("http")

host = "220.135.254.63"	-- miaoski.idv.tw
port = 2080

hum = 0		-- Humidity
tmp = 0		-- Temperature
mq9 = 0		-- MQ9 voltage
pm25 = 0	-- DN7C3CA006 voltage

-- Send URL every 5s
tmr.alarm(0, 5000, 1, function()
	url = "id/" .. UUID .. "/hum/" .. hum .. "/tmp/" .. tmp .. "/mq9/" .. mq9 .. "/dn7/" .. pm25
	print("GET http://" .. host .. ":" .. port .. "/" .. url)
	http.get(host, port, url, {}, function(conn)
	end)
end)
