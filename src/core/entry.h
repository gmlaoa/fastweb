﻿#if defined(_WIN32) || defined(_WIN64)
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#endif
extern "C" {
	DLL_EXPORT int fastweb(const char* config_filepath);
}
 