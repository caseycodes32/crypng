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