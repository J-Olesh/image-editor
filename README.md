# image-editor
Edit and create images by directly manipulating raw pixel data.

This is a C++ program that reads pixel data into a vector of RGB values (0-255). Operations can then be performed on the data to appy various effects to the image, which can then be saved as a new image.

### Currently only supports 24-bit bitmap images. 
---
## Examples:
<img src="https://github.com/user-attachments/assets/91334d75-5188-4dda-8dc2-7be28b5f032f" width=300 height=300>

Shown below are examples of some of the functions applied to the above source image. Note that some functions are highly sensitive to the source image, so results may vary. 
### Glitch

#### 1 Iteration:
<img src="https://github.com/user-attachments/assets/46650e1a-37f2-40d7-94f0-42292f8d29a9" width=300 height=300>

#### 5 Iterations:
<img src="https://github.com/user-attachments/assets/c530facd-d66f-4c4b-9504-9e31904e5e21" width=300 height=300>

### Smooth
Each iteration passes over the image row by row, pixel by pixel, and sets the colour of each pixel to the average colour of the current and previous pixel.
This results in a 'smoothing' effect.
#### 100 Iterations:
<img src="https://github.com/user-attachments/assets/27c3232f-3607-4475-aaa9-d8a42403a72a" width=300 height=300>

#### 5000 Iterations:
<img src="https://github.com/user-attachments/assets/dca4a7ed-0474-4b7c-a7a6-ee47bd05d400" width=300 height=300>

Note that using a high iteration value can result in the process taking several minutes when dealing with ~3000x3000 pixel images.
