const char *myLuaExample PROGMEM = R"(
function setup()

end

function loop()
    if (hal:btn1Down() > 0) then
        print("btn1 pressed")
    else
        print("btn1 not pressed")
    end
end

function stop()

end
)";