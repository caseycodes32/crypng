#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <GL/gl.h>
#include "imgui.h"

bool OpenFileDialog(std::string &file_path, HWND hwnd);
void ImGuiDisplayImage(std::string image_path, int &width, int &height);