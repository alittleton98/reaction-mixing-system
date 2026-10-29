#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>

using namespace std;

#if defined(DEBUG_CONFIG) && DEBUG_CONFIG
#define DISPLAY_LOG(string) std::fprintf(stdout, "%s | %s\n", __func__, string)
#define WARNING_LOG(string) std::fprintf(stdout, "%s | WARNING: %s\n", __func__, string)
#else
#define DISPLAY_LOG(string) ((void)0)
#define WARNING_LOG(string) ((void)0)
#endif
#define ERROR_LOG(string) assert(false); fprintf(stderr, "%s | ERROR: %s", __func__, string)