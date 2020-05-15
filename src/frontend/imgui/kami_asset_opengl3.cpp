#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include <file/file_path.h>

#include "asset.h"
#include "common.h"

static const char* tag = "[asset]";
static const char* app_name = "invader";

void Asset::Load(const char* filename)
{
   width = 0;
   height = 0;
   aspect = 0;

   logger(LOG_DEBUG, tag, "filename %s\n", filename);
   unsigned char* image_data = stbi_load(filename, &width, &height, NULL, 4);
   if (image_data == NULL)
      return;

   aspect = (float)width / height;

   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
   stbi_image_free(image_data);

   data = texture;
}

void Asset::Render(unsigned width, unsigned height)
{
   ImGui::Image((void*)(intptr_t)data, ImVec2(width, height));
}
