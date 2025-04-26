#include "ui_helper.h"

void ImGuiDisplayKeyPhrase(unsigned char* key, int key_length)
{
    std::string str_keyphrase = "";
    
    for (int i = 0; i < 16; i++) 
    {
        str_keyphrase.append(key_words[static_cast<int>(key[i])]);
        if ((i + 1) % 4 == 0) str_keyphrase.append("\n");
        else str_keyphrase.append(" ");
    }
    static char *c_keyphrase_buf = new char[str_keyphrase.length() + 1];
    std::strcpy(c_keyphrase_buf, str_keyphrase.c_str());

    static ImGuiInputFlags ig_text_flag = ImGuiInputTextFlags_ReadOnly;
    ImGui::InputTextMultiline("##Keyphrase", c_keyphrase_buf, sizeof(c_keyphrase_buf), ImVec2(0.0f, 0.0f), ImGuiInputTextFlags_ReadOnly);
}