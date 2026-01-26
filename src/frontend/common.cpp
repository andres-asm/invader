#include "common.h"
#include "util.h"

static const char* tag = "[common]";

const char* glsl_version;

// TODO: abstract this for other possible backends, OpenGL only for now

SDL_Window* invader_window = NULL;
SDL_GLContext invader_context = NULL;

SDL_AudioSpec want, have;
SDL_AudioDeviceID device;

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

bool create_window(const char* app_name, unsigned width, unsigned height)
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) == -1)
      logger(LOG_ERROR, tag, SDL_GetError());
      // select GL+GLSL versions
#if __APPLE__
   // GL 3.2 Core + GLSL 150
   glsl_version = "#version 150";
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // always required on Mac
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
   // GL 3.3 + GLSL 330 (slang-compatible baseline)
   glsl_version = "#version 330";
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

   SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_DisplayMode current;
   SDL_GetCurrentDisplayMode(0, &current);

   uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

   invader_window = SDL_CreateWindow(app_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
   if (invader_window == NULL)
   {
      logger(LOG_DEBUG, tag, "failed to create window: %s", SDL_GetError());
      return -1;
   }

   invader_context = SDL_GL_CreateContext(invader_window);
   // enable vsync
   SDL_GL_SetSwapInterval(1);

   // check opengl version sdl uses
   logger(LOG_DEBUG, tag, "opengl version: %s\n", (char*)glGetString(GL_VERSION));

   glViewport(0, 0, width, height);
   glewExperimental = 1;

   bool err = glewInit() != GLEW_OK;

   if (err)
   {
      logger(LOG_DEBUG, tag, "Failed to initialize OpenGL loader!");
      return 1;
   }

   return true;
}

void destroy_window()
{
   SDL_GL_DeleteContext(invader_context);
   if (invader_window != NULL)
   {
      SDL_DestroyWindow(invader_window);
      invader_window = NULL;
   }
   SDL_Quit();
}

bool create_framebuffer()
{
   int success;
   char log[512];

   // vertex shader
   int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
   glCompileShader(vertex_shader);

   glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(vertex_shader, 512, NULL, log);
      logger(LOG_DEBUG, tag, "vertex shader compilation error: %s\n", log);

      return false;
   }

   // fragment shader
   int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
   glCompileShader(fragment_shader);

   glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(fragment_shader, 512, NULL, log);
      logger(LOG_DEBUG, tag, "fragment shader compilation error: %s\n", log);

      return false;
   }

   // link shaders
   int shader_program = glCreateProgram();
   glAttachShader(shader_program, vertex_shader);
   glAttachShader(shader_program, fragment_shader);
   glLinkProgram(shader_program);

   glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
   if (!success)
   {
      glGetProgramInfoLog(shader_program, 512, NULL, log);
      logger(LOG_DEBUG, tag, "shader program linking error: %s\n", log);

      return false;
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

   glUseProgram(shader_program);
   glBindVertexArray(vao);

   return true;
}

void render_framebuffer(unsigned texture_data, core_info_t* info)
{
   unsigned integer_scale = video_scale_mode->GetValue().m_mode;

   float aspect = info->av_info.geometry.aspect_ratio;
   int width, height, x, y;

   int base_height = info->av_info.geometry.base_height;

   switch (integer_scale)
   {
      case SCALE_MODE_OFF:
      {
         height = base_height;
         width = height * aspect;

         x = (WINDOW_WIDTH - width) / 2;
         y = (WINDOW_HEIGHT - height) / 2;
         break;
      }
      case SCALE_MODE_FULL:
      {
         height = WINDOW_HEIGHT;
         width = height * aspect;
         x = (WINDOW_WIDTH - width) / 2;
         y = 0;
         break;
      }
      case SCALE_MODE_INTEGER_OVERSCALE:
      {
         unsigned scale = WINDOW_HEIGHT / base_height + (WINDOW_HEIGHT % base_height != 0);

         height = base_height * scale;
         width = height * aspect;

         x = (WINDOW_WIDTH - width) / 2;
         y = (abs(WINDOW_HEIGHT - height) / 2) * -1;

         break;
      }
      case SCALE_MODE_INTEGER:
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

   glBindTexture(GL_TEXTURE_2D, texture_data);
   glViewport(x, y, width, height);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool create_audio_device()
{
   if (SDL_Init(SDL_INIT_AUDIO) == -1)
      logger(LOG_ERROR, tag, SDL_GetError());

   unsigned i = 0;
   int devices = SDL_GetNumAudioDevices(0);

   for (i = 0; i < SDL_GetNumAudioDrivers(); ++i)
   {
      logger(LOG_DEBUG, tag, "audio driver %d: %s\n", i, SDL_GetAudioDriver(i));
   }

   logger(LOG_INFO, tag, "current audio driver %s\n", SDL_GetCurrentAudioDriver());
   logger(LOG_INFO, tag, "audio devices: %d\n", devices);

   if (devices >= 0)
   {
      for (i = 0; i < devices; i++)
         logger(LOG_INFO, tag, "device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
   }

   SDL_zero(want);

   want.freq = 48000;
   want.format = AUDIO_S16;
   want.channels = 2;
   want.samples = 4096;
   want.callback = NULL;

   logger(
      LOG_INFO, tag, "want - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d\n", want.freq,
      SDL_AUDIO_ISFLOAT(want.format), SDL_AUDIO_ISSIGNED(want.format), SDL_AUDIO_ISBIGENDIAN(want.format),
      SDL_AUDIO_BITSIZE(want.format), want.channels, want.samples);

   device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

   if (!device)
   {
      logger(LOG_ERROR, tag, "failed to open audio device: %s\n", SDL_GetError());
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
      return false;
   }
   else
      logger(LOG_INFO, tag, "opened audio device: %s\n", SDL_GetAudioDeviceName(0, 0));

   logger(
      LOG_INFO, tag, "have - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d\n", have.freq,
      SDL_AUDIO_ISFLOAT(have.format), SDL_AUDIO_ISSIGNED(have.format), SDL_AUDIO_ISBIGENDIAN(have.format),
      SDL_AUDIO_BITSIZE(have.format), have.channels, have.samples);

   SDL_PauseAudioDevice(device, 0);
   return true;
}

void set_fullscreen_mode()
{
   bool fullscreen = video_fullscreen->GetValue();
   bool desktop = video_fullscreen_windowed->GetValue();

   if (fullscreen && desktop)
      SDL_SetWindowFullscreen(invader_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
   else if (fullscreen)
      SDL_SetWindowFullscreen(invader_window, SDL_WINDOW_FULLSCREEN);
   else
      SDL_SetWindowFullscreen(invader_window, 0);
}

void set_vsync_mode()
{
   bool vsync = video_vsync->GetValue();

   SDL_GL_SetSwapInterval(vsync ? 1 : 0);
}

SDL_GLContext get_context()
{
   return invader_context;
}

SDL_Window* get_window()
{
   return invader_window;
}

const char* get_glsl_version()
{
   return glsl_version;
}

// initialize configuration
void common_config_load()
{
   settings_init("settings.toml");

   video_fullscreen->SetEventCallback(set_fullscreen_mode);
   video_fullscreen_windowed->SetEventCallback(set_fullscreen_mode);
   video_vsync->SetEventCallback(set_vsync_mode);
}
