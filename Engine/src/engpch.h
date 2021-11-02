#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <array>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Engine/Core/Log.h"
#include "Engine/Debug/Instrumentor.h"

#ifdef ENG_PLATFORM_WINDOWS
#include <Windows.h>
#endif