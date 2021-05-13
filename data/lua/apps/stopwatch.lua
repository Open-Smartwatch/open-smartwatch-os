graphicsUtils = require("graphicsUtils")

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
    print("Started stop watch")
end

function loop()
    if (hal:btnHasGoneDown(osw.BUTTON_3)) then
        if (reset == true) then
          start = hal:getLocalTime();
        else
          sumPaused = sumPaused + diff;
          start = hal:getLocalTime();
        end
        running = true;
        reset = false;
    end
    if (hal:btnHasGoneDown(osw.BUTTON_2)) then
        if (running) then
            running = false
        else
            diff = 0
            sumPaused = 0
            reset = true
        end
    end

    hal:getCanvas():fillScreen(0);
    hal:getCanvas():setTextColor(graphicsUtils.rgb565(255, 255, 255));

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
        hal:getCanvas():print(string.format("%dd", deltaDays));
    end

    hal:getCanvas():setCursor(defaultFontXOffset(1, 4), 120 - defaultFontYOffset(1, 4) / 2);
    hal:getCanvas():print(string.format("%02d:%02d:%02d", deltaHours, deltaMinutes, deltaSeconds))

    hal:flushCanvas()
end

function stop()
end
