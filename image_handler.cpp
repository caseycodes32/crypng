#include "image_handler.h"
#include "logo.h"

std::string FilenameFromPath(std::string path)
{
    size_t idxLastSlash = path.find_last_of("/\\");
    if (idxLastSlash != std::string::npos)
    {
        return path.substr(idxLastSlash + 1);
    }
    return path;
}

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

bool SaveFileDialog(std::string &file_path, HWND hwnd)
{
    char c_FilePath[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = c_FilePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "PNG Files\0*.png\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameA(&ofn) == TRUE)
    {
        file_path = std::string(c_FilePath);
        if (file_path.substr(file_path.length() - 4) != ".png")
            file_path.append(".png");

        return true;
    }
    else file_path = "";
    
    return false;
}

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

bool LoadDataFromFile(std::string image_path, ImageDetails &image_details)
{
    if (image_details.data != NULL)
        return false;

    image_details.data = stbi_load(image_path.c_str(), &image_details.width, &image_details.height, &image_details.channels, 0);
    image_details.name = FilenameFromPath(image_path);

    if (image_details.width > image_details.height)
    {
        image_details.normalized_width = 384;
        image_details.normalized_height = (static_cast<float>(image_details.height) / image_details.width) * 384;
    }
    else
    {
        image_details.normalized_height = 384;
        image_details.normalized_width = (static_cast<float>(image_details.width) / image_details.height) * 384;
    }
    
    return true;
}

bool LoadDataFromArray(unsigned char* buffer, int buffer_length, std::string name, ImageDetails &image_details)
{
    if (image_details.data != NULL)
        return false;

    image_details.data = stbi_load_from_memory(buffer, buffer_length, &image_details.width, &image_details.height, &image_details.channels, 4);
    image_details.name = name;

    return true;
}

void SaveDataToFile(std::string output_path, ImageDetails image_details)
{
    int stride_bytes = image_details.width * image_details.channels;
    stbi_write_png_compression_level = 0;
    stbi_write_png(output_path.c_str(), image_details.width, image_details.height, image_details.channels, image_details.data, stride_bytes);
}

// Image data and texture handling function LoadTextureFromData() derived from LoadTextureFromMemory()
// Courtesy of https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-opengl-users
void LoadTextureFromData(GLuint *out_texture, ImageDetails image_details, bool smooth)
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
            if (smooth)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }

            // Upload pixels into texture
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            switch (image_details.channels)
            {
                case 3:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_details.width, image_details.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_details.data);
                    break;
                case 4:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_details.width, image_details.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_details.data);
                    break;
            }
            //stbi_image_free(image_data);
            *out_texture = gl_ImageTexture;
        }
    }
}

void ImGuiDisplayImage(ImageDetails image_details)
{
    static GLuint gl_ImageTexture = 0;
    static unsigned char *t_ImageData = 0;

    //roLSB(image_details);
    if (t_ImageData != image_details.data)
    {
        gl_ImageTexture = 0;
        stbi_image_free(t_ImageData);
    }
    LoadTextureFromData(&gl_ImageTexture, image_details, true);
    ImGui::Image((ImTextureID)(intptr_t)gl_ImageTexture, ImVec2(image_details.normalized_width, image_details.normalized_height));
    t_ImageData = image_details.data;
}

void ImGuiDisplayLogo()
{
    static ImageDetails t_ImageDetails;
    static GLuint gl_LogoTexture = 0;
    LoadDataFromArray(crypng_logo, 1192, "logo", t_ImageDetails);
    LoadTextureFromData(&gl_LogoTexture, t_ImageDetails, false);
    stbi_image_free(t_ImageDetails.data);
    t_ImageDetails.data = NULL;
    ImGui::SetCursorPos(ImVec2(8, 128));
    ImGui::Image((ImTextureID)(intptr_t)gl_LogoTexture, ImVec2(384, 120));
}

void ZeroLSB(ImageDetails image_details)
{
    for (int y = 0; y < image_details.height; y++)
    {
        for (int x = 0; x < image_details.width; x++)
        {
            int idx = (y * image_details.width + x) * 4;
            
            image_details.data[idx] =  image_details.data[idx] >> 1;
            image_details.data[idx] =  image_details.data[idx] << 1;
            
            image_details.data[idx + 1] =  image_details.data[idx + 1] >> 1;
            image_details.data[idx + 1] =  image_details.data[idx + 1] << 1;

            image_details.data[idx + 2] =  image_details.data[idx + 2] >> 1;
            image_details.data[idx + 2] =  image_details.data[idx + 2] << 1;
            
        }
    }
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