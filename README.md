# Computer Graphics

This repository contains all the codes that I wrote to my computer graphics  class whose is a obligatory subject in computer science course at UFRJ. Besides that, I put some usage examples of these codes in .gif format.
It contains codes about: 
 - Polygon smoothing: To solve this problem I used the "corner cutting" method.
 - Point-polygon problem: Given a point and a polygon, this code tell us if the point is in or out the polygon.  More details are in this solution's README.
 - Convex hull (Jarvis's algorithm): Given a set of points in 2D, this code calculates the convex hull of theses points. I used the Jarvis's algorithm (Gift wrapping).  
 - Delaunay triangulation: This solution follows an algorithm that was given by my professor in class.
 - 3D Bezier curve (de Casteljau's algorithm): Given a control polygon in 3D, this code calculates the corresponding bezier curve, using de Casteljau's algorithm.
## Using the codes
To run these codes you will need to get installed g++ and the OpenGL.  As I used Microsoft Visual Studio 2017 on Windows to write these codes, I ran it with this software too, but  you can just install g++ and download the OpenGL. Make sure that the library file is in a folder called "GL"  and this folder had to be in the same folder where the main code is before run the codes.

Some links to help you with the environment:

- [How to setup OpenGL in Visual Studio](https://www.youtube.com/watch?v=8p76pJsUP44)
- [Getting started with OpenGL  on Linux and Windows](http://www.cse.iitm.ac.in/~vplab/courses/CG/opengl_start.html)

## Built with
- Windows 10
- [Microsoft Visual Studio 2017 Community Edition](https://visualstudio.microsoft.com/downloads/)
- [OpenGL](https://www.opengl.org/)
- G++/MinGW
- C++/C language 
