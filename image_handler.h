#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <GL/gl.h>
#include "imgui.h"

struct ImageDetails
{
    unsigned char *data;
    std::string name;
    int width;
    int height;
    int normalized_width;
    int normalized_height;
    int channels;
    size_t max_chars;
};

std::string FilenameFromPath(std::string path);
bool OpenFileDialog(std::string &file_path, HWND hwnd);
bool SaveFileDialog(std::string &file_path, HWND hwnd);
bool LoadDataFromFile(std::string image_path, ImageDetails &image_details);
bool LoadDataFromArray(unsigned char* buffer, int buffer_length, std::string name, ImageDetails &image_details);
void SaveDataToFile(std::string output_path, ImageDetails image_details);
void CopyImageInMemory(ImageDetails id_current, ImageDetails &id_new);
void LoadTextureFromData(GLuint *out_texture, ImageDetails image_details, bool smooth);
size_t GetMaximumCharactersFromImage(ImageDetails image_details);
void ImGuiDisplayImage(ImageDetails image_details);
void ImGuiDisplayLogo();