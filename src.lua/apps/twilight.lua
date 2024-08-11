SunTime = require("suntime")

function setParams(latitude, longitude, timezone, altitude, year, month, day, dst)
--[[
	print("latitude:", latitude)
	print("longitude:", longitude)
	print("timezone:", timezone)
	print("altitude", altitude)
	print("year", year)
	print("month", month)
	print("day", day)
	print("dst", dst)
]]
	SunTime:setPosition("nixda", latitude, longitude, timezone, altitude, true)
	SunTime:setDate(y, m, d, dst)
	SunTime:calculateTimes()
end

function getTimes(pos)
	return SunTime.times[pos]
end
