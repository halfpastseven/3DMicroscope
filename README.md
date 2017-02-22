# 3DMicroscope
3D Microscope is a program that transforms multiple images at different focus levels into one image and generates a 3D profile. It's written and compiled using Qt(TM).

INTRODUCTION

Optical microscopes have the intrinsic disadvantage of short depth of field. That means, if the surface is slightly rough, features with different heights will not be focused on the same plane. This limits the application of optical microscopy.

This work addresses this limitation of microscopy. It will transform any phogographic microscope into a 3D microscope. This is done by first taking a series of pictures at different focal levels, then processing them into a single composite image and a 3-dimensional profile.

PRINCIPLES

The idea behind this program is simple: it looks for regions of the highest contrast in each image, then stiches them together. Regions of the highest contrast are almost always in best focus. While identifying the regions of highest contrast, it also notes the depth information (at this point, it only means the position of the picture in the sequence) and can use it to generate a depth map.</p>
Each input image is a <b>X</b> (width) by <b>Y</b> (height) pixel map in RGB color space. First, a spot of radius (float) <b>r</b> is defined. A margin value <b>M</b> is also defined that corresponds to the biggest integer no bigger than <b>r</b>. For each pixel (x, y) in the picture, a Sharpness term is defined as the summation of these terms within a radius <b>r</b> from (x,y)

<p>Sharpness = (sum(R*R)*N - sum(R)*sum(R))+(sum(G*G)*N - sum(G)*sum(G))+(sum(B*B)*N - sum(B)*sum(B))</p>

where R, G, B are the corresponding values of each pixel (between 0 and 255).

All pixels of a image, except for those within the margin R from all four edges of the image, will be calculated to generate a "sharpness" map. Once all the images are analyzed, for each point (x, y) except the margin, the Sharpness will be compared for the same spot across the images. The image that has the highest contrast on this pixel will contribute to the composite image. The sequence of the image will determine the "height" of the pixel in the depth iamge.

USAGE

1. Take a series of pictures from a microscope, while gradually changing the focus in one direction. These pictures must have the same
  * Magnification (microscope)
  * Position of the sample (microscope)
  * Apeture (microscope)
  * Lighting (microscope)
  * Shutter Speed and ISO (camera)
  * White balance (camera)
  * Image size (camera)
2. Start 3DMicrosope and load the images. (Note: the images will be automatically sorted based on the file name)
3. Set the radius of the spot analysis. The default is 3.0 pixels, which will work on most images
4. Analyze the images. Depending on the size and number of images, the analysis process may take several minutes. During the analysis, the GUI will become non-responsive.
5. Upon completion, the results will show in a separate window. Two images will be shown: a composite image and a depth image. The composite image is a collage of the regions will the highest sharpness, while the depth image uses color to show the depth of the region.
  * You may save each image individually.
  * You may adjust the "threshold" (between 0 and 100) to set the regions that does not have significant sharpness in any of the images. In depth profile, these regions are treated as "bottom". (In future updates, the definition and use of threshold are subject to change)
  * Depending on the direction of focusing the camera, the pictures may be going top-down or bottom-up. This can be changed in the results window.
  * In the main window, the gradient map of each image will be shown on the right side. You may adjust the "contrast magnification" to adjust the color shade of the gradient map.

COMPLEXITY

The present algorithms have a time complexity of O(XYRn). Here n signifies the total number of images. The space complexity is O(XYn).
Modern imaging devices are capable of producing 10 Megapixel images or better. On the testing computer, it took approximately 10 seconds to process one 10 Megapixel image.

CAUTION

1. It is extremely important that the images are taken at the exact same condition except for focus level. This formulates the basic assumption of the present algorithm.
2. Do not rename, change or delete pictures after they are loaded into 3D Microscope. Doing so may result in exception that's not being addressed in this software.
3. The color in the depth iamge does not correspond to the actual z-distance. It only represents from which picture the pixel is in focus.

KNOWN ISSUES AND FUTURE DEVELOPMENT

* The present algorithm is a bit clumsy and can possibly be improved by matrix operation. The future goal is to reduce time complexity to O(XYn).
* The purpose of setting a "threshold" for depth information is as follows: in some cases a region may appear as very little variation in sharpness in all images. This may be due to severall reasons: contamination on the lens, or a hole in the sample that doesn't focus on any of the images. In the future, the threshold will be defined based on the variation of sharpness across images. The height of the regions falling below the threshold will be evaluated by interpolation.
* In future updates, two more images will be added to the results: a 3D contour plot, and a gradient map overlayed onto the composite map.
* The z-axis is merely sequence of image, which may not correspond to the actual z-distance. This will most likely solved by hardware support (such as the height information input from the microscope to the precision of micrometers). No plan as to how to address this issue is not available yet.

LICENSE AND DISCLAIMER

This software is provided under GNU General Public License (GPL) v3. This basically means that you're free to copy, distribute and reuse the binary files, source codes and artwork. However, in reusing the source code, you must a) not use for commercial purposes and b) make the sourcecode available to the public at no charge. Use of content provided by Qt(TM) is under the Lesser General Public License (LGPL) v3. For more information on Qt, visit http://qt.io

This software is provided as-is. User assumes all risks in the use or distribution of this software. The author(s) of this software does not accept any liability that may arise from the use, distribution or publication of this software, or any data or information generated by this software.

Finally, if you would like to publish the results generated by this software, please cite, where appropriate, this software:

<b><q>Qing Ni, "3D Microscope [version code]", available at http://github.com/halfpastseven/3DMicroscope/</q></b>
