#ifndef __F_FRAMEWORK_H__
#define __F_FRAMEWORK_H__

#define WIDTH 640
#define HEIGHT 600
#define DEMO_TITLE "Demo"

#include "fcommon.h"

extern void Init(GLFWwindow *window);

extern void Display(GLFWwindow *window, double time);

#endif