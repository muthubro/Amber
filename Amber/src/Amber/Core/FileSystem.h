#pragma once

#include <string>

namespace Amber
{

class FileSystem
{
public:
    // Pass filter in the format "FILTER_NAME:FILTER_VALUE"
    static std::string OpenFileDialog(const std::string& filter, const std::string& name = "Open");

    static size_t ReadFile(const char* filepath, void** data);
};

}
