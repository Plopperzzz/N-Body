#pragma once
#include "Tree.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <chrono>
#include <functional>
//#define _DEBUG


#ifdef _DEBUG
#define DEBUG_LOG(message, ...) Utils::DebugLog(message, ##__VA_ARGS__)
#else
#define DEBUG_LOG(message, ...)
#endif

class Utils
{
public:
	static void setClipboardText(const std::string& text);

	static void outputPositions(std::vector<Node*> bodies, double time, std::ofstream& file);

	// Function to measure execution time with std::invoke
	template <typename Func, typename Obj, typename... Args>
	static std::chrono::duration<double> measureInvokeCall(Func&& func, Obj&& obj, Args&&... args) {
		auto start = std::chrono::high_resolution_clock::now();

		// Call the function/method with std::invoke
		std::invoke(std::forward<Func>(func), std::forward<Obj>(obj), std::forward<Args>(args)...);

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;

		return elapsed;
	}

	static void printPrograssBar(int i, int limit, int barWidth=100, const std::string& process = "");
	static void setCursorPosition(int x, int y);
	static void getConsoleSize(int& width, int& height);
	static void signalHandler(int signum);

	static void gpScript(std::string script_name, std::string gif_name,std::string dataName, double size, std::vector<std::string> body_names, std::string path = "");

	// Variadic template to allow for multiple arguments like printf
	template<typename... Args>
	static void DebugLog(const std::string& format, Args... args) {
#ifdef _DEBUG  // Only compile this block in debug mode
		std::ostringstream ss;
		ss << FormatString(format, args...) << "\n";  // Format the message
		OutputDebugStringA(ss.str().c_str());  // Output to Visual Studio's debug output
#endif
	}

private:
	// Helper function to format strings like printf
	template<typename... Args>
	static std::string FormatString(const std::string& format, Args... args) {
		size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Calculate the size needed
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args...);
		return std::string(buf.get(), buf.get() + size - 1);  // Remove the null terminator
	}
};

