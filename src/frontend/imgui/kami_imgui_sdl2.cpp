#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "kami.h"

static const char* tag = "[kami]";
static const char* app_name = "invader";

static bool quit = false;
static bool showDemoWindow = true;
static bool showAnotherWindow = false;

static bool core_active = false;

const char* core_entries[100];

static char filename[2048] = "";

file_list_t* file_selector_list = NULL;

ImVec4 clearColor;
ImGuiIO io;

GLuint texture;

static void init_localization()
{
   setlocale(LC_ALL, "");
   bindtextdomain("invader", "./intl/");
   textdomain("invader");
}

static void imgui_shutdown()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext(NULL);
}

static void imgui_setup()
{
   ImGui::CreateContext(NULL);
   io = ImGui::GetIO();
   ImGui_ImplSDL2_InitForOpenGL(mywindow, mycontext);
   ImGui_ImplOpenGL3_Init(glsl_version);

   ImGui::StyleColorsDark(NULL);
   io.Fonts->AddFontDefault();
}

static void tooltip(const char* desc)
{
   ImGui::SameLine(0, 0);
   ImGui::TextDisabled("(?)");
   if (ImGui::IsItemHovered(0))
   {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc, (const char*)NULL);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
   }
}

bool string_list_combo(const char* label, int* current_item, struct string_list* list, int popup_max_height_in_items)
{
   int ret = 0;
   char** entries = (char**)calloc(list->size, sizeof(char*));
   for (unsigned i = 0; i < list->size; i++)
   {
      entries[i] = list->elems[i].data;
   }
   if (ImGui::Combo(label, current_item, entries, list->size, 0))
      return true;
   else
      return false;
}

static void window_core_control()
{
   static int current_core = 0;
   static int previous_core = -1;

   static const char* current_core_label = current_core_info.core_name;
   static const char* current_core_version = current_core_info.core_version;
   static const char* current_core_extensions = current_core_info.extensions;

   static bool file_selector_open;

   static bool current_core_supports_no_game;
   static bool current_core_block_extract;
   static bool current_core_full_path;

   unsigned option_count = controller->get_option_count();
   core_option_t* options = controller->get_options();

   ImGui::Begin(_("window_title_core_control"), NULL, 0);

   ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.40f);
   ImGui::Combo(_("core_selector_label"), &current_core, core_entries, core_count);

   if (core_count != 0 && (previous_core != current_core) || previous_core == -1)
   {
      current_core_info = core_info_list[current_core];

      controller->core_deinit();
      delete controller;
      controller = new PiccoloController(&current_core_info);
      controller->peek_core(current_core_info.file_name);

      static bool core_active = false;
      current_core_supports_no_game = current_core_info.supports_no_game;
      previous_core = current_core;
   }

   if (!string_is_empty(current_core_label))
   {
      ImGui::LabelText(_("core_current_version_label"), current_core_version);
      tooltip(_("core_current_version_desc"));
      ImGui::LabelText(_("core_current_extensions_label"), current_core_extensions);
      tooltip(_("core_current_extensions_desc"));

      if (current_core_supports_no_game && !core_active)
      {
         if (ImGui::Button(_("core_current_start_core_label")))
         {
            controller->load_core(current_core_info.file_name);
            core_active = controller->load_game(NULL);
         }
         tooltip(_("core_current_start_core_desc"));
      }
      if (option_count > 0)
      {
         if (ImGui::CollapsingHeader(_("core_current_options_label"), ImGuiTreeNodeFlags_None))
         {
            for (unsigned i = 0; i < option_count; i++)
            {
               core_option_t* option = &options[i];
               char* description = option->description;
               struct string_list* values = kami_core_option_get_values(option);

               int index = kami_core_option_get_index(option, values);
               if (string_list_combo(description, &index, values, 0))
               {
                  char* value = values->elems[index].data;
                  kami_core_option_update(option, value);
               }
               string_list_free(values);
            }
         }
      }
   }
   ImGui::End();
}

static void imgui_draw_frame()
{
   SDL_Event e;

   while (SDL_PollEvent(&e) != 0)
   {
      ImGui_ImplSDL2_ProcessEvent(&e);
      if (e.type == SDL_QUIT)
         quit = true;
      if (
         e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE
         && e.window.windowID == SDL_GetWindowID(mywindow))
         quit = true;
   }

   /* start imgui frame */
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(mywindow);
   ImGui::NewFrame();

   // window_settings();
   // window_status();
   window_core_control();
   /*
   if (core_active)
      window_core();

   if (showDemoWindow)
      igShowDemoWindow(&showDemoWindow);
   */
   bool demo = true;
   ImGui::ShowDemoWindow(&demo);
   ImGui::Render();
   SDL_GL_MakeCurrent(mywindow, mycontext);
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
   glClear(GL_COLOR_BUFFER_BIT);
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   SDL_GL_SwapWindow(mywindow);
}

int main(int argc, char* argv[])
{
   logger_set_level(LOG_DEBUG);
   init_localization();

   // common_config_load();
   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      return -1;
   mywindow = get_window();
   mycontext = get_context();

   glsl_version = get_glsl_version();

   imgui_setup();
   // imgui_set_default_style();

   // kami_init_audio();
   // logger(LOG_INFO, tag, "audio driver: %s\n", SDL_GetCurrentAudioDriver());

   kami_core_list_init("./cores");

   for (unsigned i = 0; i < core_count; i++)
   {
      core_entries[i] = core_info_list[i].core_name;
      logger(LOG_DEBUG, tag, "loading file %s\n", core_entries[i]);
   }

   while (!quit)
   {
      imgui_draw_frame();
   }

   imgui_shutdown();
   destroy_window();

   return 0;
}