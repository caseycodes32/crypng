#include "ui_helper.h"

void UIHelper::ImGuiDisplayKeyPhrase(unsigned char* key, int key_length)
{
    if (!(key[0] || key[1] || key[2] || key[3]))
        return;

    ImGui::Separator();
    ImGui::Text("Key Phrase (Please Copy!):");

    std::string str_keyphrase = "";
    for (int i = 0; i < 16; i++) 
    {
        str_keyphrase.append(key_words[static_cast<int>(key[i])]);
        if ((i + 1) % 4 == 0) str_keyphrase.append("\n");
        else str_keyphrase.append(" ");
    }
    ImGui::BeginChild("##KeyphraseDisplay", ImVec2(0.0f, 72.0f), ImGuiChildFlags_Border);
    ImGui::Text(str_keyphrase.c_str());
    ImGui::EndChild();

    if (ImGui::Button("Copy Key Phrase", ImVec2(0.0f, 32.0f)))
        ImGui::SetClipboardText(str_keyphrase.c_str());
}

void UIHelper::ImGuiInputKeyPhrase(unsigned char* key, int key_length)
{
    static bool error_invalid_keyphrase = false;
    static bool input_complete = false;
    static char keyphrase_input_buf[1024];
    ImGui::InputTextMultiline("##Keyphrase Entry", keyphrase_input_buf, sizeof(keyphrase_input_buf), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
    if (ImGui::Button("Confirm Keyphrase", ImVec2(0.0f, 32.0f)))
    {
        error_invalid_keyphrase = false;
        input_complete = false;
        unsigned char possible_key[16];
        int key_iterator = 0;
        std::string s_keyphrase(keyphrase_input_buf);
        std::stringstream ss_keyphrase(s_keyphrase);

        std::string cur_word;

        while (ss_keyphrase >> cur_word)
        {
            static int key_words_len = sizeof(key_words) / sizeof(key_words[0]);
            const std::string *loc_word = std::find(key_words, key_words + key_words_len, cur_word);

            if (loc_word != key_words + key_words_len)
            {
                unsigned char idx = std::distance(key_words, loc_word);
                if (key_iterator <=15) possible_key[key_iterator] = idx;
                key_iterator++;
            }
            else
            {
                error_invalid_keyphrase = true;
                return;
            }
        }
        if (key_iterator == 16) 
        {
            memcpy(key, possible_key, 16);
            input_complete = true;
        }
        else error_invalid_keyphrase = true;
    }
    if (error_invalid_keyphrase)
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error - Invalid Key Phrase Entered");
    else if (input_complete)
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Key Phrase Accepted");

}

std::string UIHelper::ClampFileName(std::string file_name, int chars)
{
    if (file_name.length() > chars)
        return ("..." + file_name.substr(chars - 3));
    else return file_name;
}