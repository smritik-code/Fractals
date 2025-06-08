# Fractal Renderer

This repository contains C++ implementations of fractal rendering using OpenGL and GLFW. The programs visualize stunning fractal patterns, including Mandelbrot and Julia sets, as well as a blended combination of both.

## Features

### Mandelbrot and Julia Fractal Renderers

* Render the Mandelbrot set and Julia set individually.
* High-performance rendering using GPU shaders.

### Blended Fractal Renderer

* Combines Mandelbrot and Julia fractals with smooth blending.
* Dynamic zooming and panning for detailed exploration.
* Smooth color transitions using a custom palette.
* Combines Mandelbrot and Julia fractals with time-based blending.
* Controls:

  * **Arrow Keys**: Pan camera.
  * Automatic zoom-in and zoom-out animation.
  * Closes automatically after 125 seconds or press `ESC` to exit.

---

## Code Overview

### Mandelbrot and Julia Renderers

* Individual files for each fractal type.
* Each program uses a simple fragment shader for fractal computation.
* High configurability via uniform variables.

### Blended Fractal Renderer

* Combines both fractals in a single program.
* Implements dynamic zooming with easing functions.
* Utilizes a custom color palette for enhanced visuals.

---

## Customization

* Modify shader constants to change fractal behavior, colors, and zoom levels.
* Add keyboard controls to manipulate parameters in real-time.

---

## Acknowledgements

* Uses GLFW for window creation and input handling.
* GLEW simplifies OpenGL extension management.

## Results
![Screenshot 2025-06-08 024204](https://github.com/user-attachments/assets/b57bdc33-9792-4e74-9ad7-e675a0547a0e)
![Screenshot 2025-06-08 024243](https://github.com/user-attachments/assets/0c9322c5-0ec5-4968-a784-5d686bd039c1)
![Screenshot 2025-06-08 024350](https://github.com/user-attachments/assets/989cd123-85c2-408e-adea-ecd1d054d013)





