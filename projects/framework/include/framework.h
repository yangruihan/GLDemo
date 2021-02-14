#ifndef __F_FRAMEWORK_H__
#define __F_FRAMEWORK_H__

#define WIDTH 640
#define HEIGHT 600
#define DEMO_TITLE "Demo"

#include "fcommon.h"

extern void init(GLFWwindow *window);

extern void display(GLFWwindow *window, double time);

#endif