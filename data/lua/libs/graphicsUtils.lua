local graphicsUtils = {}

function graphicsUtils.rgb565(red, green, blue)
    return ((red & 0xf8) << 8) | ((green & 0xfc) << 3) | (blue >> 3);
end

return graphicsUtils