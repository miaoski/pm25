print "g0v PM2.5 Project"
print "Pull GPIO2 low to reset password"

pin = 4		-- GPIO2
gpio.mode(pin, gpio.INPUT, gpio.PULLUP)
max_retry = 20

if(gpio.read(pin) == 1 and file.open("config.lua")) then
	file.flush()
	file.close()
	require("config")

	print("SSID = " .. SSID)
	print("PASS = " .. PASS)
	wifi.setmode(wifi.STATION)
	wifi.sta.config(SSID, PASS)
	wifi.sta.connect()

	cnt = 0
	tmr.alarm(0, 1000, 1, function()
  		if cnt < max_retry and wifi.sta.getip() then
    			tmr.stop(0)
    			print("IP:  " .. wifi.sta.getip())
    			print("MAC: " .. wifi.sta.getmac())
    			dofile("main.lua")
  		else
			if cnt < max_retry then
				print("Getting an IP...")
			else
				tmr.stop(0)
				print("Unable to get IP.  Enter AP mode.")
				dofile("ap.lua")
			end
  		end
		cnt = cnt + 1
	end)
else
	if(gpio.read(pin) == 0) then
		print("GPIO2 pulled low.  Enter AP mode.")
	else
		print("config.lua not found.  Enter AP mode.")
	end
	dofile("ap.lua")
end
