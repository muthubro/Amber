# Amber

![Amber](/Resources/Branding/Logo.png?raw=true "Amber")

__Amber Engine__ is a game engine being developed as a personal project for fun as well as educational purposes. 
Amber derives heavy inspiration from Hazel Engine being built by __TheCherno__ (Yan Chernikov) in The Game Engine Series on his YouTube channel.

I started this project in order to learn about graphics programming, game engine development, and the software development process 
in general. None of it would have been possible without the help of TheCherno and his series, so I highly recommend checking out the channel.

Amber currently supports only the OpenGL API and Windows (it might work on other 
platforms but they are not officially supported yet). The ultimate plan is for Amber to support all major graphics APIs (OpenGL, Vulkan, DirectX 11, Metal) and platforms (Windows, Linux, Mac, PS, Xbox, and maybe Android).

## Build
Amber uses premake for creating the solution/project files.
Visual Studio 2017 or 2019 is recommended for build. Amber is officially untested on other development environments.

Steps to run Amber using Visual Studio:
- Clone this repository recursively:\
`git clone --recursive https://github.com/muthubro/Amber`
- Run the script file inside Scripts folder.
- Open the `Sandbox.sln` file generated in the repository root folder using Visual Studio.
- Build `Sandbox` using _Build -> Build Solution_ or press _Ctrl+Shift+B_
- Open the `Amber.sln` file generated in the repository root folder using Visual Studio.
- Press `F5` and watch the magic unfold!

## Features
These are the features available in Amber as of 17/08/2020 (113 commits):
- Amber Editor (built using [ImGui](https://github.com/ocornut/imgui) and [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo))
- 3D Rendering using PBR
- 2D Physics ([Box2D](https://github.com/erincatto/box2d))
- C# Scripting
- Basic 2D Rendering with spritesheet support
- Entity Component System (currently using [EnTT](https://github.com/skypjack/entt))

Other Dependencies:
- [GLFW](https://github.com/glfw/glfw) as the graphics and windowing library
- [Glad](https://glad.dav1d.de/) as the loader generator
- [Assimp](https://github.com/assimp/assimp) for importing assets 
- [spdlog](https://github.com/gabime/spdlog) for logging
- [glm](https://github.com/g-truc/glm) for math