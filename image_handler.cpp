#include "image_handler.h"

bool OpenFileDialog(std::string &file_path, HWND hwnd)
{
    char c_FilePath[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = c_FilePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Image Files\0*.png\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        file_path = std::string(c_FilePath);
        return true;
    }
    else file_path = "";
    
    return false;
}

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Image data and texture handling functions LoadDataFromFile() and LoadTextureFromData() derived from LoadTextureFromMemory() and LoadTextureFromFile()
// Courtesy of https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-opengl-users
bool LoadDataFromFile(std::string image_path, ImageDetails &image_details)
{
    if (image_details.data != NULL) return false;

    FILE* f = fopen(image_path.c_str(), "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = malloc(file_size);
    fread(file_data, 1, file_size, f);
    fclose(f);

    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)file_data, (int)file_size, &image_details.width, &image_details.height, &image_details.channels, 4);
    image_details.data = image_data;

    free(file_data);
    return true;
}

void LoadTextureFromData(GLuint *out_texture, ImageDetails image_details)
{
    if (*(out_texture) == 0)
    {
        if (image_details.data != NULL)
        {
            // Create a OpenGL texture identifier
            GLuint gl_ImageTexture;
            glGenTextures(1, &gl_ImageTexture);
            glBindTexture(GL_TEXTURE_2D, gl_ImageTexture);

            // Setup filtering parameters for display
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Upload pixels into texture
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_details.width, image_details.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_details.data);
            //stbi_image_free(image_data);

            *out_texture = gl_ImageTexture;
        }
    }
}

void ImGuiDisplayImage(ImageDetails image_details)
{
    static GLuint gl_ImageTexture = 0;
    static unsigned char *t_ImageData = 0;
    int width = 0;
    int height = 0;

    if (image_details.data != t_ImageData) gl_ImageTexture = 0;

    if (image_details.width > image_details.height)
    {
        width = 350;
        height = (static_cast<float>(image_details.height) / image_details.width) * 350;
    }
    else
    {
        height = 350;
        width = (static_cast<float>(image_details.width) / image_details.height) * 350;
    }

    LoadTextureFromData(&gl_ImageTexture, image_details);
    ImGui::Image((ImTextureID)(intptr_t)gl_ImageTexture, ImVec2(width, height));
    t_ImageData = image_details.data;
}



/*

// invert colors by modifying image_data array
    for (int y = 0; y < image_height; y++)
    {
        for (int x = 0; x < image_width; x++)
        {
            int idx = (y * image_width + x) * image_channels;
            
            image_data[idx] = 255 - image_data[idx];
            image_data[idx + 1] = 255 - image_data[idx + 1]; // Green
            image_data[idx + 2] = 255 - image_data[idx + 2]; // Blue
            
        }
    }
        
*/