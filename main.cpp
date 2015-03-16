#include "main.h"

#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* system;
	bool result;
	system = new SystemClass();
	if(!system)
	{
		return 0;
	}

	result = system->Initialize();
	if(result)
	{
		system->Run();
	}

	system->Shutdown();
	delete system;
	system = nullptr;
	return 0;
}
