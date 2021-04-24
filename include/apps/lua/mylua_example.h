const char *myLuaExample PROGMEM = R"(
function rgb565(red, green, blue)
    return ((red & 0xf8) << 8) | ((green & 0xfc) << 3) | (blue >> 3);
end


function print2Digits(n)
    if (n < 10) then
        hal:getCanvas():print("0");
    end
    hal:getCanvas():print(n);
end
  
function defaultFontXOffset(numChars, scale)  --works with default font only
    return numChars * 6 * scale;
end

function defaultFontYOffset(numRows, scale)
    return numRows * 8 * scale;
end
  

start = 0;
diff = 0;
running = false;
reset = true;
sumPaused = 0;

function setup()

end

function loop()
    if (hal:btn3Down() > 0) then
        if (reset == true) then
          start = hal:getLocalTime();
        else
          sumPaused = sumPaused + diff;
          start = hal:getLocalTime();
        end
        running = true;
        reset = false;
    end
    
    if (hal:btn2Down() > 0) then
        if (running) then
            running = false
        else
            diff = 0
            sumPaused = 0
            reset = true
        end
    end
    
    hal:getCanvas():fillScreen(0);
    hal:getCanvas():setTextColor(rgb565(255, 255, 255));

    hal:getCanvas():setTextSize(2);
    if (reset == true) then
        hal:getCanvas():setCursor(220 - defaultFontXOffset(5, 2), 42);
        hal:getCanvas():print("Start");
    elseif (running == false) then
        hal:getCanvas():setCursor(220 - defaultFontXOffset(8, 2), 42);
        hal:getCanvas():print("Continue");
    end

    if (running == true) then
        hal:getCanvas():setCursor(220 - defaultFontXOffset(4, 2), 182);
        hal:getCanvas():print("Stop");
    elseif (reset == false) then
        hal:getCanvas():setCursor(220 - defaultFontXOffset(5, 2), 182);
        hal:getCanvas():print("Reset");
    end

    if (running == true) then
        diff = hal:getLocalTime() - start;
    end

    total = math.floor(diff + sumPaused);
    deltaSeconds = (total) % 60;
    deltaMinutes = (total // 60) % 60;
    deltaHours = (total / 60 // 60) % 24;
    deltaDays = total / 60 / 60 // 24;

    hal:getCanvas():setTextSize(4);

    if (deltaDays > 0) then
        hal:getCanvas():setCursor(120 - defaultFontXOffset(deltaDays < 10 and 1 or 2, 4),
                                    120 - defaultFontYOffset(1, 4) * 1.5);
        hal:getCanvas():print(deltaDays);
        hal:getCanvas():print("d");
    end

    hal:getCanvas():setCursor(defaultFontXOffset(1, 4), 120 - defaultFontYOffset(1, 4) / 2);
    print2Digits(deltaHours);
    hal:getCanvas():print(":");
    print2Digits(deltaMinutes);
    hal:getCanvas():print(":");
    print2Digits(deltaSeconds);

    hal:flushCanvas()
end

function stop()
end
)";