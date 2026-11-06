GLFW and GLUT are helper libraries used alongside OpenGL (and related apis like Vulkan or OpenGL ES)
to handle platform specific tasks that OpenGL can't do.

OpenGL is strictly a graphics-rendering specification; it does not know how to open window on your
desktop, talk to your operating system's window manager, or listen for a keyboard press or mouse click. That is where the windowing and utility libraries like GLFW and GLUT come in.



GLFW (Graphics Library Framework)
GLFW is a modern, lightweight, open-source C library designed specifically to provide a minimal, a cross platform interface for creating and managing windows, OpenGL/Vulkan contexts, and user input.

-> Core Purpose: Creating windows, managing multiple montitors/high DPI displays, setting up rendering contexts, and capturing input (keyboard, mouse).

-> Control Flow: GLFW does not take over your application's main loop. You have explicit control over the even polling loop (glfwPollEvents()) and rendering timing, making it ideal for modern games, engines, and performance critical applications.

-> Modern Support: Beyond OpenGL, GLFW officially supports OpenGL ES and Vulkan. 

GLUT(OpenGL Utility Toolkit)
GLUT is an older, classic C/C++ utility library to help write window system independent OpenGL programs.

-> Core Purpose: Opening windows, handling basic input callbacks (like glutDisplayFunc or glutKeyboardFunc), and even rendering simple 3D geometric shapes

-> Control Flow: GLUT owns the event loop (via glutMainLoop()) means the libray dictates how and when your program updates and draws. 


GLEW (OpenGL Extension Wrangler Library)
    Writing Modern OpenGL code requires manually declaring and loading dozen or hundreds of function pointers which varies across drivers and operating systems. GLEW allows you to write portable graphics code that can tap into advanced GPU capabilities cleanly.

    