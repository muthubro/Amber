#pragma once

#ifdef HZ_PLATFORM_WINDOWS

int main() {
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif // HZ_PLATFORM_WINDOWS
