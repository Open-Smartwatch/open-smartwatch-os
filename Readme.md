# Lib Open-Smartwatch

A library that handles maths, animations, conversions, and drawing.


<img src="./_docs/watch-doom-fire.jpg" width="33%">

## Desktop Compiling Support

You can develop the 2D stuff on your desktop, see the Readme.md in `examples/`.
This speeds up ui development by a lot.

<img src="./_docs/doom-fire.png" width="33%">


## FAQ

<details>
    <summary>
	1. Can you explain what's the variables cx,cy,start,stop and lineradius ?
	</summary>
    `void Graphics2D::drawArc(int32_t cx, int32_t cy, float start, float stop, uint16_t steps, uint16_t radius, uint8_t lineRadius, uint16_t color)`

- 	`cx,cy` : the center coordinates of the arc
- 	`start, stop` : the start and stop angles (0 is on top of the circle)
-	`steps` : number of segments used to draw the arc (bigger is smoother but slower)
-	`radius` : width/2 of the arc
-	`lineRadius` : width of the line defined as a radius of a circle drawing the line
-	`color` : 16 bytes value of the color, check `rgb565(uint8_t red, uint8_t green, uint8_t blue)` to create colors or use consts like `BLACK`, `WHITE`, ...
</details>
