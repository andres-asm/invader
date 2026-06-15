// imgui
#include "imgui.h"

#include "asset.h"
#include "common.h"

// TODO: load the image via stb_image and upload it to a VkImage, registering
// it with ImGui_ImplVulkan_AddTexture (mirrors kami_asset_opengl3.cpp's
// stbi_load + glTexImage2D path). For now just reserve placeholder
// dimensions so layout code that reads get_width()/get_aspect() works.
void Asset::Load(const char* filename)
{
   UNUSED(filename);

   width = 256;
   height = 256;
   aspect = 1.0f;
   data = 0;
}

void Asset::Render(unsigned width, unsigned height)
{
   ImGui::Dummy(ImVec2(width, height));
}
