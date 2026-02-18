// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <glad/glad.h>
#include <limits>
#include <ostream>
#include <random>
#include <Shader.h>
#include <sstream>
#include <string>
#include <vector>

// #include <glm/fwd.hpp> breaks things here for some reason
// something to do with foward declaration, google it later
#include <glm/glm.hpp>
#include <ios>
#include <iostream>
#include <exception>
#include <functional>
#include <glm/ext/matrix_transform.inl>
#include <glm/fwd.hpp>
#include <utility>
#include <cassert>
#include <algorithm>
#include <cctype>


#endif //PCH_H
