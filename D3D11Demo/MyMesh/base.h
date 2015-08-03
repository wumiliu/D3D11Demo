#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cassert>

//STL
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

//tinyxml
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

typedef std::string STRING;


// TODO: 在此处引用程序需要的其他头文件