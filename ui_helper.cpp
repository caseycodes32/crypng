#include "ui_helper.h"

void ImGuiDisplayKeyPhrase(unsigned char* key, int key_length)
{
    if (!(key[0] || key[1] || key[2] || key[3])) return;

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

    if (ImGui::Button("Copy Key Phrase", ImVec2(0.0f, 32.0f))) ImGui::SetClipboardText(str_keyphrase.c_str());

}