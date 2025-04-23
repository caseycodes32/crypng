#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <GL/gl.h>
#include "imgui.h"

struct ImageDetails
{
    unsigned char *data;
    size_t data_size;
    int width;
    int height;
    int channels;
};

bool OpenFileDialog(std::string &file_path, HWND hwnd);
bool LoadDataFromFile(std::string image_path, ImageDetails &image_details);
void LoadTextureFromData(GLuint *out_texture, ImageDetails image_details);
void ImGuiDisplayImage(ImageDetails image_details);
void ZeroLSB(ImageDetails image_details);