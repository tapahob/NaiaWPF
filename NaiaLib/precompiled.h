#pragma once

#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <dinput.h>
#include <mmsystem.h>
#include <msclr/marshal.h>
#include <wtypes.h>
// STL
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

// OpenGL & GLEW
#include <GL/glew.h>
#include <GL/wglew.h>
#include <gl/GL.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

// Naia
#include "NaiaLib.h"
