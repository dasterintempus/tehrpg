#include "application.h"

int main(int argc, char** argv)
{
	teh::Application* app = new teh::Application();
	int result = app->start(argc, argv);
	delete app;
	return result;
}