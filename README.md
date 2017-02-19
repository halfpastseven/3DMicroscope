# 3DMicroscope
3D Microscope is a program that transforms multiple images at different focus levels into one image and generate a 3D profile

INTRODUCTION

Optical microscopes have the intrinsic disadvantage of having a short depth of view. That means, if the surface is slightly rough, features with different heights will not be focused on the same plane. This limits the application of optical microscopy.
This work addresses this limitation of microscopy. It will transform any phogographic microscope into a 3D microscope. This is done by first taking a series of pictures at different focal levels, then processing them into a single composite image and a 3-dimensional profile.

PRINCIPLES

The ideas behind this program is simple: it looks for regions of the highest contrast in each image, then stiches them together. Regions of the highest contrast almost always correspond to the right-focus.
