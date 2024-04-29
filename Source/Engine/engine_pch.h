#if !defined(WINVER)
#define WINVER 0x0605
#endif

#define IMGUI_DEFINE_MATH_OPERATORS

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <algorithm>
#include <fstream>
#include <array>
#include <exception>
#include <malloc.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <queue>
#include <iostream>
#include <filesystem>
#include <any>
#include <limits>

#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <d3d11.h>


#include <xmmintrin.h>
#include <comdef.h>
#include <assert.h>
#include <WinUser.h>
#include <windows.h>

#include "PropertyDefines.h"

#include "Math/Vector.h"
#include "Math/Matrix.h"

//#define DEBUG_NEW new(__FILE__, __LINE__)
//#define new DEBUG_NEW
