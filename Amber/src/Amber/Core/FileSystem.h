#pragma once

#include <string>

namespace Amber
{

class FileSystem
{
public:
    static std::string OpenFileDialog(const std::string& filter, const std::string& name = "Open");
};

}
