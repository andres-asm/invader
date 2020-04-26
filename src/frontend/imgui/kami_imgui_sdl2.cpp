#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "kami.h"

static const char* tag = "[kami]";
static const char* app_name = "invader";

static bool quit = false;
static bool showDemoWindow = true;
static bool showAnotherWindow = false;

static char filename[2048] = "";

file_list_t* file_selector_list = NULL;

ImVec4 clearColor;
ImGuiIO io;

GLuint texture;

Kami* kami;
Kami* kami2;

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

Kami::Kami()
{
   this->piccolo = new PiccoloWrapper(&core_list[0]);
   current_core = 0;
   previous_core = -1;
   active = false;
   core_info = piccolo->get_info();
}

void Kami::Run(const char* title)
{
   bool file_selector_open;
   const char* core_label = core_info->core_name;
   const char* core_version = core_info->core_version;
   const char* supported_extensions = core_info->extensions;

   bool supports_no_game = core_info->supports_no_game;
   bool block_extract = core_info->block_extract;
   bool full_path = core_info->full_path;

   unsigned option_count = piccolo->get_option_count();
   core_option_t* options = piccolo->get_options();

   ImGui::Begin(_(title), NULL, 0);

   ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.40f);
   ImGui::Combo(_("core_selector_label"), &current_core, core_entries, core_count);

   if (core_count != 0 && (previous_core != current_core) || previous_core == -1)
   {
      core_info = &core_list[current_core];

      piccolo = new PiccoloWrapper(core_info);
      piccolo->peek_core(core_info->file_name);
      active = false;
      previous_core = current_core;
   }

   if (!string_is_empty(core_label))
   {
      ImGui::LabelText(_("core_current_version_label"), core_version);
      tooltip(_("core_current_version_desc"));
      ImGui::LabelText(_("core_current_extensions_label"), supported_extensions);
      tooltip(_("core_current_extensions_desc"));

      if (supports_no_game && !active)
      {
         if (ImGui::Button(_("core_current_start_core_label")))
         {
            piccolo->load_core(core_info->file_name);
            active = piccolo->load_game(NULL);
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
   kami->Run("Core 1");
   kami2->Run("Core 2");
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

   // kami_core_list_init("./cores");
   kami = new Kami();
   kami2 = new Kami();

   kami->CoreListInit("./cores");
   kami2->CoreListInit("./cores");

   while (!quit)
   {
      imgui_draw_frame();
   }

   imgui_shutdown();
   destroy_window();

   return 0;
}