#pragma once

#include "../engine_config.hpp"

#include "stdio.h"
#include "math.h"
#include "assert.h"

#include <string>
#include <vector>
#include <unordered_map>

// kisslib
#include "kisslib/kissmath.hpp"
#include "kisslib/kissmath_colors.hpp"

#include "kisslib/string.hpp"
#include "kisslib/file_io.hpp"
#include "kisslib/circular_buffer.hpp"
#include "kisslib/macros.hpp"
#include "kisslib/random.hpp"
#include "kisslib/raw_array.hpp"
#include "kisslib/image.hpp"
#include "kisslib/read_directory.hpp"
#include "kisslib/running_average.hpp"
#include "kisslib/stl_extensions.hpp"
#include "kisslib/threadpool.hpp"
#include "kisslib/threadsafe_queue.hpp"
#include "kisslib/timer.hpp"
//#include "kisslib/animation.hpp"
#include "kisslib/collision.hpp"
//#include "kisslib/allocator.hpp"
//#include "kisslib/containers.hpp"
#include "kisslib/serialization.hpp"
using namespace kiss;

#include "tracy/Tracy.hpp"
#include "dear_imgui.hpp"
#include "window.hpp"
