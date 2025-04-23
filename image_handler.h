#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <GL/gl.h>
#include "imgui.h"

struct ImageDetails
{
    unsigned char *data;
    std::string name;
    int width;
    int height;
    int channels;
};

std::string FilenameFromPath(std::string path);
bool OpenFileDialog(std::string &file_path, HWND hwnd);
bool SaveFileDialog(std::string &file_path, HWND hwnd);
bool LoadDataFromFile(std::string image_path, ImageDetails &image_details);
void SaveDataToFile(std::string output_path, ImageDetails image_details);
void LoadTextureFromData(GLuint *out_texture, ImageDetails image_details);
void ImGuiDisplayImage(ImageDetails image_details);
void ZeroLSB(ImageDetails image_details);