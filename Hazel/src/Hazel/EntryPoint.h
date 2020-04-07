#pragma once

#ifdef HZ_PLATFORM_WINDOWS

int main() {
	Hazel::Log::Init();
	HZ_CORE_WARN("Hazel Engine!");
	char name[] = "Muathasim";
	HZ_INFO("Hello {}!!!", name);

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif // HZ_PLATFORM_WINDOWS
