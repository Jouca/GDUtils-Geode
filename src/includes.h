#pragma once
#include <sio_client.h>
#include <variant>
#include <mutex>
#include <thread>
#ifdef GEODE_IS_WINDOWS
#include <synchapi.h>
#endif
