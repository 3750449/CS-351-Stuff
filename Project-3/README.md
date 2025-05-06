# Project 3 - Procedural WebGL Shapes

## Overview
This project demonstrates procedural geometry and shader programming using WebGL and GLSL. We incrementally build up from a triangle to an animated, colorful 5-pointed star.

---

## Versions

###  [Wireframe Triangle](triangle.html)
Draws a wireframe triangle using `gl.LINE_LOOP` and `gl_VertexID`.

###  [10-Sided Filled Polygon](polygon.html)
Filled decagon using `gl.TRIANGLE_FAN` and a uniform variable for vertex count.

###  [Five-Pointed Star](star.html)
Alternates radius to produce a star shape.

###  [Rotating Star](spinny.html)
Adds time-based rotation to animate the star.

###  [Rotating Colored Star](shine.html)
Interpolates between colors using vertex radius. **Extra credit**

---

## How to Run
Open each HTML file in a modern browser that supports WebGL2 (e.g., Chrome, Firefox). Or serve the directory via a local server:

```bash
python -m http.server
