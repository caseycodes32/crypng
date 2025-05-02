#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include "imgui.h"
#include "word_list.h"

namespace UIHelper
{
    void ImGuiDisplayKeyPhrase(unsigned char* key, int key_length);
    void ImGuiInputKeyPhrase(unsigned char* key, int key_length);
    std::string ClampFileName(std::string file_name, int chars);
    std::string ChannelCountToDescriptor(int channels);
}