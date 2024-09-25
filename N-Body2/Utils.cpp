#include "Utils.h"
#include <filesystem>

void Utils::signalHandler(int signum) {
	system("cls");

	exit(signum);
}
//void Utils::outputPositions(std::vector<Node3D> bodies, double time, std::ofstream& file) {
//	file << time;
//	for (auto& node : bodies) {
//		file << "," << node.position.x << "," << node.position.y << "," << node.position.z;
//	}
//	file << "\n";
//}

void Utils::setClipboardText(const std::string& text) {
	// Open the clipboard
	if (!OpenClipboard(nullptr)) {
		std::cerr << "Failed to open clipboard" << std::endl;
		return;
	}

	// Empty the clipboard
	EmptyClipboard();

	// Allocate global memory for the text
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (!hGlobal) {
		std::cerr << "Failed to allocate memory" << std::endl;
		CloseClipboard();
		return;
	}

	// Copy the text into the global memory
	memcpy(GlobalLock(hGlobal), text.c_str(), text.size() + 1);
	GlobalUnlock(hGlobal);

	// Set the clipboard data
	SetClipboardData(CF_TEXT, hGlobal);

	// Close the clipboard
	CloseClipboard();

	// Free the global memory (not needed anymore since it's now managed by the clipboard)
	GlobalFree(hGlobal);
}

void Utils::printProgressBar(int i, int limit, int barWidth, const std::string& process) {

	float progress = float(i + 1) / limit; // Calculate progress as a fraction
	int pos = barWidth * progress;
	//int init_x, init_y;
	int width, height;
	//COORD cursorPosition;
	//COORD currentPosition;
	
	// Get the console handle
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CONSOLE_CURSOR_INFO cursorInfo;

	// Get console screen buffer info (including the cursor position)
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	// Get the current cursor info
	GetConsoleCursorInfo(hConsole, &cursorInfo);

	// Set the cursor to be invisible
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);

	/*
	// Create a COORD structure and set the X and Y coordinates
	currentPosition = csbi.dwCursorPosition;


	init_x = currentPosition.X;
	init_y = currentPosition.Y;
	*/

	// The width is the difference between the rightmost and leftmost columns
	width = csbi.srWindow.Right - csbi.srWindow.Left + 1;

	// The height is the difference between the bottommost and topmost rows
	height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	setCursorPosition(0, height-2);
	std::cout << "Processing: " << process;
	setCursorPosition(i, height-1);
	if (i == 0)
		std::cout << "[";
	else {

		setCursorPosition(pos == 0 ? 1 : pos, height - 1);
		std::cout << "|";
	}
	setCursorPosition(barWidth+1, height-1);
	std::cout << "] " << std::fixed << std::setprecision(2) << progress * 100.0 << " %\r";

	if(pos == 100) cursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);
	setCursorPosition(0, 1);
}

// Function to set the cursor position
void Utils::setCursorPosition(int x, int y) {
	// Get the console handle
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Create a COORD structure and set the X and Y coordinates
	COORD cursorPosition;
	cursorPosition.X = x;
	cursorPosition.Y = y;

	// Use SetConsoleCursorPosition to move the cursor
	SetConsoleCursorPosition(hConsole, cursorPosition);
}

void Utils::getConsoleSize(int& width, int& height) {
	// Get the console handle
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Structure to hold console buffer info
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// Get console screen buffer information
	if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		// The width is the difference between the rightmost and leftmost columns
		width = csbi.srWindow.Right - csbi.srWindow.Left + 1;

		// The height is the difference between the bottommost and topmost rows
		height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	}
	else {
		// In case of failure, return default values
		width = 0;
		height = 0;
	}
}

// expects a csv with data formatted as:
// timestep, body1_x, body1_y, ..., bodyN_x, bodyN_y
void Utils::gpScript(std::string script_name, std::string gif_name, std::string data_name, double size, std::vector<std::string> body_names, std::string path) {

	std::ofstream gnuplot(path + script_name);

	if (!gnuplot.is_open()) {
		std::cerr << "Error: Could not open file for writing: " << path + script_name << "\n";
		std::cerr << "Current directory: " << std::filesystem::current_path() << std::endl;
		return;
	}

	auto setup =			"# Set the terminal to GIF\nset terminal gif size 1024, 1024 enhanced font 'Arial,12'\n\n";
	auto output =			"set output '" + gif_name + ".gif'\n\n";
	auto delimiter =		"# Set the delimiter to comma\nset datafile separator ','\n\n";
	auto axis =				"# Set axis\nset xrange[" + std::to_string(-1 * size) + ":" + std::to_string(size) + "]\nset yrange[" + std::to_string(-1 * size) + ":" + std::to_string(size) + "]\n";

	auto ratio = "set size ratio -1\n\n";

	auto title = "# Set titles and labels\nset title \"Orbits\"\n\n";
	auto loadData = "# Load and plot data from file\nplot ";
	std::string plotData = "";
	int i = 0;
	for (auto& body_name : body_names) {
		plotData += "'" + data_name + "' using " + std::to_string(i + 2) + ":" + std::to_string(i + 3) + "  with lines lw 2 notitle, \\\n";
		i += 2;
	}

	auto setOutput = "# Finalize the output\nset output\n";

	gnuplot << setup << output << delimiter << axis << ratio << title << loadData << plotData << setOutput;
	return;
}


// expects a csv with data formatted as:
// timestep, body1_x, body1_y, ..., bodyN_x, bodyN_y
void Utils::gpScript3d(std::string script_name, std::string gif_name, std::string data_name, double size, std::vector<std::string> body_names, std::string path) {
	std::ofstream gnuplot(path + script_name);

	if (!gnuplot.is_open()) {
		std::cerr << "Error: Could not open file for writing: " << path + script_name << "\n";
		std::cerr << "Current directory: " << std::filesystem::current_path() << std::endl;
		return;
	}

	// Setup Gnuplot for GIF and 3D plot
	auto setup = "# Set the terminal to GIF\nset terminal gif size 1024, 1024 enhanced font 'Arial,12'\n\n";
	auto output = "set output '" + gif_name + ".gif'\n\n";
	auto delimiter = "# Set the delimiter to comma\nset datafile separator ','\n\n";

	// Set the axis ranges for 3D
	auto axis = "# Set axis\nset xrange[" + std::to_string(-1 * size) + ":" + std::to_string(size) + "]\n"
		"set yrange[" + std::to_string(-1 * size) + ":" + std::to_string(size) + "]\n"
		"set zrange[" + std::to_string(-1 * size) + ":" + std::to_string(size) + "]\n";

	auto ratio = "set size ratio -1\n\n";

	auto title = "# Set titles and labels\nset title \"3D Orbits\"\n\n";

	// Prepare to load and plot data
	auto loadData = "# Load and plot data from file\nsplot ";
	std::string plotData = "";
	int i = 0;
	for (auto& body_name : body_names) {
		// Use the i+2, i+3, i+4 columns for x, y, z of each body
		plotData += "'" + data_name + "' using " + std::to_string(i + 2) + ":" + std::to_string(i + 3) + ":" + std::to_string(i + 4) + "  with lines lw 2 notitle, \\\n";
		i += 3; // Increment by 3 since we're using 3 columns per body (x, y, z)
	}

	auto setOutput = "# Finalize the output\nset output\n";

	// Write everything to the Gnuplot script
	gnuplot << setup << output << delimiter << axis << ratio << title << loadData << plotData << setOutput;
	return;
}


