#include <exception>

#include "GlobalLog.h"
#include "Application/Application.h"



int main() {
	try {
		Application app;
		app.run();
	}
	catch (const std::exception& error) {
		GlobalLog.Write(std::string("Exception occurred: ") + error.what());
	}
	catch (...) {
		GlobalLog.Write("What the fuck. Something has gone horribly wrong.");
	}

	GlobalLog.Write("Application termination");

	return 0;
}
