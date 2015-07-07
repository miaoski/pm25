print "g0v PM2.5 Project"

max_retry = 60

if(file.open("config.lua")) then
	file.flush()
	file.close()
	require("config")

	if SSID then print("SSID = " .. SSID) end
	if PASS then print("PASS = " .. PASS) end
	if UUID then print("UUID = " .. UUID) end
	wifi.setmode(wifi.STATION)
	wifi.sta.config(SSID, PASS)
	wifi.sta.connect()

	cnt = 0
	tmr.alarm(0, 1000, 1, function()
  		if(cnt <= max_retry and wifi.sta.getip()) then
    			tmr.stop(0)
    			print("IP:  " .. wifi.sta.getip())
    			print("MAC: " .. wifi.sta.getmac())
    			dofile("main.lua")
  		else
			cnt = cnt + 1
			if cnt <= max_retry then
				print("Getting IP ... #" .. cnt)
			else
				tmr.stop(0)
				print("Unable to get IP.  Reboot.")
				node.restart()
			end
  		end
	end)
else
	print("config.lua not found.  Enter AP mode.")
	dofile("ap.lua")
end
