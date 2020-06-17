// imgui
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "input/gamepad.h"
#include "kami.h"
#include "widgets.h"

static const char* tag = "[main]";
static const char* app_name = "invader";

static const char* asset_dir = "./assets/gamepad/generic";

static bool quit = false;

std::vector<Kami*> kami_instances;
Kami* current_kami_instance;

ImVec4 clearColor;
ImGuiIO io;

std::vector<Asset> gamepad_assets;

GamePad* controller;

const char* device_gamepad_asset_names[] = {
   "base.png",         "b.png",     "y.png",  "select.png", "start.png",       "up.png",          "down.png",
   "left.png",         "right.png", "a.png",  "x.png",      "l.png",           "r.png",           "l2.png",
   "r2.png",           "l3.png",    "r3.png", "guide.png",  "left_axis_x.png", "left_axis_y.png", "right_axis_x.png",
   "right_axis_y.png",
};

void init_localization()
{
   setlocale(LC_ALL, "");
   bindtextdomain("invader", "./intl/");
   textdomain("invader");
}

void set_default_style()
{
   ImGuiStyle* style = &ImGui::GetStyle();
   style->WindowPadding = ImVec2(15, 15);
   style->WindowRounding = 0.0f;
   style->FramePadding = ImVec2(5, 5);
   style->FrameRounding = 0.0f;
   style->ItemSpacing = ImVec2(12, 8);
   style->ItemInnerSpacing = ImVec2(8, 6);
   style->IndentSpacing = 25.0f;
   style->ScrollbarSize = 1.0f;
   style->ScrollbarRounding = 0.0f;
   style->GrabMinSize = 10.0f;
   style->GrabRounding = 0.0f;
   style->TabRounding = 0.0f;

   ImVec4 widgets_idle = ImVec4(0.10f, 0.10f, 0.18f, 1.00f);
   ImVec4 widgets_hovered = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);
   ImVec4 widgets_active = ImVec4(0.50f, 0.50f, 0.58f, 1.00f);

   ImVec4 highlights = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);

   ImVec4 grabs_idle = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
   ImVec4 grabs_hovered = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
   ImVec4 grabs_active = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);

   style->Colors[ImGuiCol_Text] = highlights;
   style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
   style->Colors[ImGuiCol_Border] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

   style->Colors[ImGuiCol_WindowBg] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
   style->Colors[ImGuiCol_ChildBg] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
   style->Colors[ImGuiCol_PopupBg] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
   style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);

   style->Colors[ImGuiCol_Border] = highlights;
   style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);

   style->Colors[ImGuiCol_FrameBg] = widgets_hovered;
   style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
   style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);

   style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
   style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
   style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);

   style->Colors[ImGuiCol_ScrollbarBg] = widgets_idle;
   style->Colors[ImGuiCol_ScrollbarGrab] = grabs_idle;
   style->Colors[ImGuiCol_ScrollbarGrabHovered] = grabs_hovered;
   style->Colors[ImGuiCol_ScrollbarGrabActive] = grabs_active;
   style->Colors[ImGuiCol_CheckMark] = highlights;
   style->Colors[ImGuiCol_SliderGrab] = grabs_idle;
   style->Colors[ImGuiCol_SliderGrabActive] = grabs_active;
   style->Colors[ImGuiCol_Button] = widgets_idle;
   style->Colors[ImGuiCol_ButtonHovered] = widgets_hovered;
   style->Colors[ImGuiCol_ButtonActive] = widgets_active;
   style->Colors[ImGuiCol_Header] = widgets_idle;
   style->Colors[ImGuiCol_HeaderHovered] = widgets_hovered;
   style->Colors[ImGuiCol_HeaderActive] = widgets_active;
   style->Colors[ImGuiCol_Separator] = widgets_idle;
   style->Colors[ImGuiCol_SeparatorHovered] = widgets_hovered;
   style->Colors[ImGuiCol_SeparatorActive] = widgets_active;
   style->Colors[ImGuiCol_ResizeGrip] = widgets_idle;
   style->Colors[ImGuiCol_ResizeGripHovered] = widgets_hovered;
   style->Colors[ImGuiCol_ResizeGripActive] = widgets_active;
   style->Colors[ImGuiCol_Tab] = widgets_idle;
   style->Colors[ImGuiCol_TabHovered] = widgets_hovered;
   style->Colors[ImGuiCol_TabActive] = widgets_active;

   style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
   style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
   style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
   style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.70f, 0.70f, 0.78f, 0.70f);

   clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

struct gamepad_lut
{
   int libretro_id;
   int sdl_id;
};

struct gamepad_lut lut[] = {
   {GAMEPAD_B, SDL_CONTROLLER_BUTTON_A},
   {GAMEPAD_Y, SDL_CONTROLLER_BUTTON_X},
   {GAMEPAD_SELECT, SDL_CONTROLLER_BUTTON_BACK},
   {GAMEPAD_START, SDL_CONTROLLER_BUTTON_START},
   {GAMEPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_UP},
   {GAMEPAD_DOWN, SDL_CONTROLLER_BUTTON_DPAD_DOWN},
   {GAMEPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_LEFT},
   {GAMEPAD_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
   {GAMEPAD_A, SDL_CONTROLLER_BUTTON_B},
   {GAMEPAD_X, SDL_CONTROLLER_BUTTON_Y},
   {GAMEPAD_L, SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
   {GAMEPAD_R, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
   {GAMEPAD_L2, SDL_CONTROLLER_AXIS_TRIGGERLEFT},
   {GAMEPAD_R2, SDL_CONTROLLER_AXIS_TRIGGERRIGHT},
   {GAMEPAD_L3, SDL_CONTROLLER_BUTTON_LEFTSTICK},
   {GAMEPAD_R3, SDL_CONTROLLER_BUTTON_RIGHTSTICK},
   {GAMEPAD_GUIDE, SDL_CONTROLLER_BUTTON_GUIDE},
   {GAMEPAD_LEFT_STICK_X, SDL_CONTROLLER_AXIS_LEFTX},
   {GAMEPAD_LEFT_STICK_Y, SDL_CONTROLLER_AXIS_LEFTY},
   {GAMEPAD_RIGHT_STICK_X, SDL_CONTROLLER_AXIS_RIGHTX},
   {GAMEPAD_RIGHT_STICK_Y, SDL_CONTROLLER_AXIS_RIGHTY},
};

void render_frontend_input_device_state()
{
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   Asset asset;
   GLuint base, result;
   asset = gamepad_assets.at(0);
   base = asset.get_texture();

   unsigned width = gamepad_assets[0].get_width() / 2;
   unsigned height = width / gamepad_assets[0].get_aspect();

   ImVec2 p = ImGui::GetCursorScreenPos();
   ImGui::Image((void*)(intptr_t)base, ImVec2(width, height));

   input_state_t state;

   for (unsigned i = 0; i <= GAMEPAD_GUIDE; i++)
   {
      if (i != GAMEPAD_L2 && i != GAMEPAD_R2)
      {
         if (controller->GetButtonState((SDL_GameControllerButton)lut[i].sdl_id))
            asset = gamepad_assets.at(i + 1);
      }
      else
      {
         if (controller->GetAxisValue((SDL_GameControllerAxis)lut[i].sdl_id))
            asset = gamepad_assets.at(i + 1);
      }
      result = asset.get_texture();
      ImGui::GetWindowDrawList()->AddImage(
         (void*)(intptr_t)result, p, ImVec2(p.x + width, p.y + height), ImVec2(0, 0), ImVec2(1, 1));
   }

   for (unsigned i = GAMEPAD_LEFT_STICK_X; i < GAMEPAD_LAST; i++)
   {
      if (abs(controller->GetAxisValue((SDL_GameControllerAxis)lut[i].sdl_id)) > 10000)
         asset = gamepad_assets.at(i + 1);
      result = asset.get_texture();
      ImGui::GetWindowDrawList()->AddImage(
         (void*)(intptr_t)result, p, ImVec2(p.x + width, p.y + height), ImVec2(0, 0), ImVec2(1, 1));
   }
}

void add_instance()
{
   Kami* new_instance;
   new_instance = new Kami();

   new_instance->CoreListInit("./cores");
   new_instance->TextureListInit(asset_dir);
   kami_instances.push_back(new_instance);
}

void invader()
{
   int instance_count = kami_instances.size();
   static int current_instance = 1;

   ImGui::Begin(_("window_title_invader"), NULL, ImGuiWindowFlags_AlwaysAutoResize);

   // if (ImGui::Button(_("invader_file_manager"), ImVec2(120, 0)))
   //{
   //   ImGui::OpenPopup(_("window_title_file_manager"));
   //   file_manager_dialog_is_open = true;
   //}
   // file_manager(".", ".");

   if (ImGui::Button(_("kami_add_instace"), ImVec2(120, 0)))
      add_instance();

   if (instance_count > 1 && ImGui::SliderInt(_("kami_instance_selector"), &current_instance, 1, instance_count))
      current_kami_instance = kami_instances.at(current_instance - 1);

   if (!controller)
   {
      controller = new GamePad();
      controller->Initialize();
   }
   controller->Update();
   render_frontend_input_device_state();

   if (video_fullscreen->Render() || video_fullscreen_windowed->Render())
      set_fullscreen_mode(video_fullscreen->GetValue(), video_fullscreen_windowed->GetValue());

   if (video_vsync->Render())
      set_vsync_mode(video_vsync->GetValue());

   if (video_scale_mode->Render())
   {
   }

   ImGui::End();
}

void imgui_shutdown()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext(NULL);
}

void imgui_setup()
{
   ImGui::CreateContext(NULL);
   io = ImGui::GetIO();
   ImGui_ImplSDL2_InitForOpenGL(invader_window, invader_context);
   ImGui_ImplOpenGL3_Init(glsl_version);

   ImGui::StyleColorsDark(NULL);
   io.Fonts->AddFontDefault();
}

void imgui_draw_frame()
{
   unsigned i = 0;
   SDL_Event e;

   while (SDL_PollEvent(&e) != 0)
   {
      if (controller)
         controller->ReceiveEvent(e);
      ImGui_ImplSDL2_ProcessEvent(&e);
      if (e.type == SDL_QUIT)
         quit = true;
      if (
         e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE
         && e.window.windowID == SDL_GetWindowID(invader_window))
         quit = true;
   }

   // start imgui frame
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplSDL2_NewFrame(invader_window);

   ImGui::NewFrame();

   invader();

   for (Kami* instance : kami_instances)
   {
      std::string title = "Core ";
      title += std::to_string(i + 1);
      instance->Main(title.c_str());
      i++;
   }

   ImGui::Render();

   SDL_GL_MakeCurrent(invader_window, invader_context);
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
   glClear(GL_COLOR_BUFFER_BIT);

   if (current_kami_instance->GetCoreStatus() == CORE_STATUS_RUNNING)
      render_framebuffer(current_kami_instance->GetTextureData(), current_kami_instance->GetCoreInfo());
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   SDL_GL_SwapWindow(invader_window);
}

int main(int argc, char* argv[])
{
   logger_set_level(LOG_DEBUG);

   init_localization();
   common_config_load();

   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      goto shutdown;
   invader_window = get_window();
   invader_context = get_context();

   set_fullscreen_mode(video_fullscreen->GetValue(), video_fullscreen_windowed->GetValue());

   glsl_version = get_glsl_version();

   imgui_setup();
   set_default_style();

   if (!create_framebuffer())
      goto shutdown;
   if (!create_audio_device())
      goto shutdown;

   add_instance();
   current_kami_instance = kami_instances.at(0);

   while (!quit)
   {
      imgui_draw_frame();
   }

shutdown:
   logger(LOG_DEBUG, tag, "shutting down\n");
   // delete kami;

   imgui_shutdown();
   destroy_window();

   return 0;
}