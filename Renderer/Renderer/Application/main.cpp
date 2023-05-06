
#include <Application/ExampleApp.h>

int main()
{
	NAMESPACE::ExampleApp app(1280, 720);

	if (app.Initialize() == false)
		return -1;

	return app.Run();
}