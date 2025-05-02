#include "image_handler.h"
#include "logo.h"

std::string FilenameFromPath(std::string path)
{
    size_t idx_last_fslash = path.find_last_of("/\\");
    if (idx_last_fslash != std::string::npos)
    {
        return path.substr(idx_last_fslash + 1);
    }
    return path;
}

bool OpenFileDialog(std::string &file_path, HWND hwnd)
{
    char c_file_path[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = c_file_path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Image Files\0*.png\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        file_path = std::string(c_file_path);
        return true;
    }
    else file_path = "";
    
    return false;
}

bool SaveFileDialog(std::string &file_path, HWND hwnd)
{
    char c_file_path[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = c_file_path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "PNG Files\0*.png\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameA(&ofn) == TRUE)
    {
        file_path = std::string(c_file_path);
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

    image_details.max_chars = std::min(GetMaximumCharactersFromImage(image_details), size_t(25536));
    image_details.data_id = 0;
    
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
            GLuint gl_image_texture;
            glGenTextures(1, &gl_image_texture);
            glBindTexture(GL_TEXTURE_2D, gl_image_texture);

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
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            switch (image_details.channels)
            {
                case 1:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_details.width, image_details.height, 0, GL_RED, GL_UNSIGNED_BYTE, image_details.data);
                    break;
                case 3:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_details.width, image_details.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_details.data);
                    break;
                case 4:
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_details.width, image_details.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_details.data);
                    break;
            }

            *out_texture = gl_image_texture;
            return;
        }
    }
}

void CopyImageInMemory(ImageDetails id_current, ImageDetails &id_new)
{
    id_new.channels = id_current.channels;
    id_new.width = id_current.width;
    id_new.height = id_current.height;
    id_new.normalized_width = id_current.normalized_width;
    id_new.normalized_height = id_current.normalized_height;
    id_new.name = id_current.name;

    size_t image_size = (id_current.width * id_current.height * id_current.channels);
    memcpy(id_new.data, id_current.data, image_size);
}

size_t GetMaximumCharactersFromImage(ImageDetails image_details)
{
    size_t blocks = (image_details.width / 8) * (image_details.height / 8);
    int intensity_channels = std::min(image_details.channels, 3);
    size_t max_bytes = blocks * intensity_channels * 8;
    return max_bytes;
}

void ImGuiDisplayImage(ImageDetails image_details)
{
    static GLuint gl_image_texture = 0;
    static unsigned char *ptr_image_data = 0;
    static int image_data_id = image_details.data_id;

    if (ptr_image_data != image_details.data)
    {
        stbi_image_free(ptr_image_data);
        glDeleteTextures(1, &gl_image_texture);
        gl_image_texture = 0;
    }
    else if (image_data_id != image_details.data_id)
    {
        gl_image_texture = 0;
    }
    LoadTextureFromData(&gl_image_texture, image_details, true);
    ImGui::Image((ImTextureID)(intptr_t)gl_image_texture, ImVec2(image_details.normalized_width, image_details.normalized_height));
    ptr_image_data = image_details.data;
    image_data_id = image_details.data_id;
}

void ImGuiDisplayLogo()
{
    static ImageDetails logo_image_details;
    static GLuint gl_logo_texture = 0;
    LoadDataFromArray(crypng_logo, 1192, "logo", logo_image_details);
    LoadTextureFromData(&gl_logo_texture, logo_image_details, false);
    stbi_image_free(logo_image_details.data);
    logo_image_details.data = NULL;
    ImGui::SetCursorPos(ImVec2(8, 148));
    ImGui::Image((ImTextureID)(intptr_t)gl_logo_texture, ImVec2(384, 120));
}