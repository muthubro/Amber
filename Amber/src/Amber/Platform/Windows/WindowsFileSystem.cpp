#include "abpch.h"
#include "Amber/Core/FileSystem.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <Windows.h>

#include "Amber/Core/Application.h"

namespace Amber
{

std::string FileSystem::OpenFileDialog(const std::string& filter, const std::string& title)
{
    using namespace std::string_literals;

    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    
    size_t filterSplit = filter.find(':');
    std::string filterName, filterValue;
    if (filterSplit == std::string::npos)
    {
        filterName = "Target";
        filterValue = filter;
    }
    else
    {
        filterName = filter.substr(0, filterSplit);
        filterValue = filter.substr(filterSplit + 1);
    }

    std::string filterText = filterName + "\0"s + filterValue + "\0All\0*.*\0"s;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filterText.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = title.c_str();
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return std::string();
}

size_t FileSystem::ReadFile(const char* filepath, void** data)
{
    HANDLE file = CreateFileA(
        filepath,
        FILE_READ_DATA,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (file == INVALID_HANDLE_VALUE)
        return 0;

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(file, &fileSize))
    {
        CloseHandle(file);
        return 0;
    }
    size_t dataSize = fileSize.QuadPart;

    *data = malloc(dataSize);
    DWORD read = 0;
    if (!::ReadFile(file, *data, fileSize.LowPart, &read, NULL) || fileSize.LowPart != read)
    {
        CloseHandle(file);
        *data = nullptr;
        return 0;
    }

    CloseHandle(file);
    return dataSize;
}

}