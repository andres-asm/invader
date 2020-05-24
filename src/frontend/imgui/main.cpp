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
static bool second_instance = false;

Kami* kami1;
Kami* kami2;

int kami1_output;
int kami2_output;

ImVec4 clearColor;
ImGuiIO io;

std::vector<Asset> gamead_assets;

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
   asset = gamead_assets.at(0);
   base = asset.get_texture();

   unsigned width = gamead_assets[0].get_width() / 2;
   unsigned height = width / gamead_assets[0].get_aspect();

   ImVec2 p = ImGui::GetCursorScreenPos();
   ImGui::Image((void*)(intptr_t)base, ImVec2(width, height));

   input_state_t state;

   for (unsigned i = 0; i <= GAMEPAD_GUIDE; i++)
   {
      if (i != GAMEPAD_L2 && i != GAMEPAD_R2)
      {
         if (controller->GetButtonState((SDL_GameControllerButton)lut[i].sdl_id))
            asset = gamead_assets.at(i + 1);
      }
      else
      {
         if (controller->GetAxisValue((SDL_GameControllerAxis)lut[i].sdl_id))
            asset = gamead_assets.at(i + 1);
      }
      result = asset.get_texture();
      ImGui::GetWindowDrawList()->AddImage(
         (void*)(intptr_t)result, p, ImVec2(p.x + width, p.y + height), ImVec2(0, 0), ImVec2(1, 1));
   }

   for (unsigned i = GAMEPAD_LEFT_STICK_X; i < GAMEPAD_LAST; i++)
   {
      if (abs(controller->GetAxisValue((SDL_GameControllerAxis)lut[i].sdl_id)) > 10000)
         asset = gamead_assets.at(i + 1);
      result = asset.get_texture();
      ImGui::GetWindowDrawList()->AddImage(
         (void*)(intptr_t)result, p, ImVec2(p.x + width, p.y + height), ImVec2(0, 0), ImVec2(1, 1));
   }
}

void invader()
{
   ImGui::Begin(_("window_title_invader"), NULL, ImGuiWindowFlags_AlwaysAutoResize);

   // if (ImGui::Button(_("invader_file_manager"), ImVec2(120, 0)))
   //{
   //   ImGui::OpenPopup(_("window_title_file_manager"));
   //   file_manager_dialog_is_open = true;
   //}
   // file_manager(".", ".");

   if (!controller)
   {
      controller = new GamePad();
      controller->Initialize();
   }
   controller->Update();
   render_frontend_input_device_state();

   ImGui::Checkbox(video_fullscreen->GetName(), video_fullscreen->GetPtr());
   tooltip(_("NAAA"));

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

const char* vertex_shader_source =
   "#version 330 core\n"
   "layout (location = 0) in vec3 aPos;\n"
   "layout (location = 1) in vec3 aColor;\n"
   "layout (location = 2) in vec2 aTexCoord;\n"
   "out vec3 ourColor;\n"
   "out vec2 TexCoord;\n"
   "void main()\n"
   "{\n"
   "    gl_Position = vec4(aPos, 1.0);\n"
   "    ourColor = aColor;\n"
   "    TexCoord = aTexCoord;\n"
   "}\n";

const char* fragment_shader_source =
   "#version 330 core\n"
   "out vec4 FragColor;\n"
   "in vec3 ourColor;\n"
   "in vec2 TexCoord;\n"
   "uniform sampler2D ourTexture;\n"
   "void main()\n"
   "{\n"
   "    FragColor = texture(ourTexture, TexCoord);\n"
   "}\n";

void framebuffer_setup()
{
   int success;
   char infoLog[512];

   // vertex shader
   int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
   glCompileShader(vertex_shader);

   glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
      logger(LOG_DEBUG, tag, "vertex shader compilation error: %s\n", infoLog);
   }

   // fragment shader
   int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
   glCompileShader(fragment_shader);

   glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
      logger(LOG_DEBUG, tag, "fragment shader compilation error: %s\n", infoLog);
   }

   // link shaders
   int shader_program = glCreateProgram();
   glAttachShader(shader_program, vertex_shader);
   glAttachShader(shader_program, fragment_shader);
   glLinkProgram(shader_program);

   glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
   if (!success)
   {
      glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
      logger(LOG_DEBUG, tag, "shader program linking error: %s\n", infoLog);
   }
   glDeleteShader(vertex_shader);
   glDeleteShader(fragment_shader);

   float vertices[] = {1.0f, -1.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                       0.0f, 1.0f,  0.0f,  1.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                       0.0f, 0.0f,  -1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f};
   unsigned int indices[] = {0, 1, 3, 1, 2, 3};

   unsigned int vbo, vao, ebo;
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);

   glBindVertexArray(vao);

   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   // position attributes
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   // color attributes
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   // texture coordinate attributes
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, kami1_output);

   glUseProgram(shader_program);
   glBindVertexArray(vao);
}

enum scale
{
   SCALE_OFF = 0,
   SCALE_FULL,
   SCALE_INTEGER_UP,
   SCALE_INTEGER_DOWN,
};

void framebuffer_render()
{
   unsigned integer_scale = SCALE_INTEGER_DOWN;

   core_info* info = kami1->GetCoreInfo();
   float aspect = info->av_info.geometry.aspect_ratio;
   int width, height, x, y;

   int base_height = info->av_info.geometry.base_height;

   if (kami1_output)
      switch (integer_scale)
      {
         case SCALE_OFF:
         {
            height = base_height;
            width = height * aspect;

            x = (WINDOW_WIDTH - width) / 2;
            y = (WINDOW_HEIGHT - height) / 2;
            break;
         }
         case SCALE_FULL:
         {
            height = WINDOW_HEIGHT;
            width = height * aspect;
            x = (WINDOW_WIDTH - width) / 2;
            y = 0;
            break;
         }
         case SCALE_INTEGER_UP:
         {
            unsigned scale = WINDOW_HEIGHT / base_height + (WINDOW_HEIGHT % base_height != 0);

            height = base_height * scale;
            width = height * aspect;

            x = (WINDOW_WIDTH - width) / 2;
            y = (abs(WINDOW_HEIGHT - height) / 2) * -1;

            break;
         }
         case SCALE_INTEGER_DOWN:
         {
            unsigned scale = WINDOW_HEIGHT / base_height;
            height = base_height * scale;
            width = height * aspect;

            x = (WINDOW_WIDTH - width) / 2;
            y = (WINDOW_HEIGHT - height) / 2;
            break;
         }
         default:
            break;
      }

   glViewport(x, y, width, height);
   // TODO: setup viewport
   if (kami1_output)
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void imgui_draw_frame()
{
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

   kami1->Main("Core 1");
   if (second_instance)
      kami2->Main("Core 2");

   // TODO: remove this
   bool demo = true;
   ImGui::ShowDemoWindow(&demo);

   ImGui::Render();
   SDL_GL_MakeCurrent(invader_window, invader_context);
   glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
   glClear(GL_COLOR_BUFFER_BIT);
   framebuffer_render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   SDL_GL_SwapWindow(invader_window);
}

int main(int argc, char* argv[])
{
   logger_set_level(LOG_DEBUG);
   init_localization();
   common_config_load();

   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      return -1;
   invader_window = get_window();
   invader_context = get_context();

   // set_fullscreen_mode(video_fullscreen, true);

   glsl_version = get_glsl_version();

   imgui_setup();
   set_default_style();

   kami1 = new Kami();
   kami1->CoreListInit("./cores");
   kami1->TextureListInit(asset_dir);

   if (second_instance)
   {
      kami2 = new Kami();
      kami2->CoreListInit("./cores");
      kami2->TextureListInit(asset_dir);
   }

   framebuffer_setup();

   while (!quit)
   {
      imgui_draw_frame();
   }

   logger(LOG_DEBUG, tag, "shutting down\n");
   delete kami1;
   if (second_instance)
      delete kami2;

   imgui_shutdown();
   destroy_window();

   return 0;
}