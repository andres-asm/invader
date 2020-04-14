#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION

#include <GL/glew.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include "nuklear.h"
#include "kami.h"
#include "nuklear_sdl_gl3.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

struct nk_context *ctx;
struct nk_colorf bg;

static const char* tag = "[kami]";
static const char* app_name = "invader";

GLuint texture;

SDL_AudioSpec want, have;
SDL_AudioDeviceID device;

bool nk_checkbox_bool(struct nk_context* ctx, const char* label, bool *active)
{
   int    x = *active;
   bool ret = nk_checkbox_label(ctx, label, &x);
   *active  = x;

   return ret;
}

/* This is probable very bad for performance */
int nk_combo_string_list(struct nk_context *ctx, struct string_list *list, int selected, int item_height, struct nk_vec2 size)
{
   int ret = selected;
   const char **entries = calloc(list->size, sizeof (char *));
   for (unsigned i = 0; i < list->size; i++)
   {
      entries[i] = list->elems[i].data;
   }

   ret = nk_combo(ctx, entries, list->size, ret, item_height, size);
   free(entries);
   return ret;
}

int kami_render_framebuffer(const core_frame_buffer_t *frame_buffer, unsigned pixel_format)
{
   if (!texture)
      glGenTextures(1, &texture);

   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   switch (pixel_format)
   {
      case RETRO_PIXEL_FORMAT_XRGB8888:
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, frame_buffer->width, frame_buffer->height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, frame_buffer->data);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, frame_buffer->pitch / sizeof(uint32_t));
         break;
      case RETRO_PIXEL_FORMAT_RGB565:
         glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
         glPixelStorei(GL_UNPACK_ROW_LENGTH, frame_buffer->pitch / sizeof(uint16_t));
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, frame_buffer->width, frame_buffer->height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, frame_buffer->data);
         break;
      default:
         logger(LOG_DEBUG, tag, "pixel format: %s (%d) unhandled\n", PRINT_PIXFMT(pixel_format), pixel_format);

   }

   return ((int)texture);
}

/* test code */
size_t kami_render_audio(const int16_t *data, size_t frames)
{
   SDL_QueueAudio(device, data, 4 * frames);
   return frames;
}

void sdl_audio_callback(void *data, uint8_t* stream, int len)
{
   logger(LOG_INFO, tag, "playing audio frames %d\n", 0);
}


bool init_audio_device()
{
   int devices = SDL_GetNumAudioDevices(0);

   logger(LOG_INFO, tag, "audio devices: %d\n", devices);

   for(unsigned i = 0; i < devices; i++)
      logger(LOG_INFO, tag, "device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));

   SDL_zero(want);

   want.freq = 48000;
   want.format = AUDIO_S16;
   want.channels = 2;
   want.samples = 4096;
   want.callback = NULL;

   logger(LOG_INFO, tag, "want - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d\n",
      want.freq, SDL_AUDIO_ISFLOAT(want.format), SDL_AUDIO_ISSIGNED(want.format), SDL_AUDIO_ISBIGENDIAN(want.format), SDL_AUDIO_BITSIZE(want.format), want.channels, want.samples);
   device = SDL_OpenAudioDevice(0, 0, &want, &have, 0);
   if(!device) {
      logger(LOG_ERROR, tag, "failed to open audio device: %s\n", SDL_GetError());
      SDL_Quit();
      return false;
   }
   else
      logger(LOG_ERROR, tag, "opened audio device: %s\n", SDL_GetAudioDeviceName(0, 0));

   logger(LOG_INFO, tag, "have - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d\n",
      have.freq, SDL_AUDIO_ISFLOAT(have.format), SDL_AUDIO_ISSIGNED(have.format), SDL_AUDIO_ISBIGENDIAN(have.format), SDL_AUDIO_BITSIZE(have.format), have.channels, have.samples);

   /* test code */
   Uint32 wavLength;
   Uint8 *wavBuffer;
   //logger(LOG_ERROR, tag, "load wav %d\n", SDL_LoadWAV("test.wav", &have, &wavBuffer, &wavLength));
   logger(LOG_ERROR, tag, "queue audio %d\n", SDL_QueueAudio(device, wavBuffer, wavLength));
   SDL_PauseAudioDevice(device, 0);
   return true;
}

/* Render the main interface */
void gui_render(struct nk_context *ctx)
{
   static bool initialized;
   static unsigned previous_core;
   static bool running;

   if (!initialized)
      kami_core_list_init(setting_string_val("directory_cores"));

   /* GUI */
   if (nk_begin(ctx, "Just a placeholder GUI", nk_rect(10, 10, 500, 700),
         NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
         NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
   {
      nk_layout_row_dynamic(ctx, 30, 2);

      nk_label(ctx, __("directory_cores"), NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      char* string = setting_string_val("directory_cores");
      int len = strlen(setting_string_val("directory_cores"));
      int size = setting_get("directory_cores")->size;
      nk_edit_string(ctx, NK_EDIT_SIMPLE, string, &len, size, nk_filter_default);

      nk_label(ctx, __("directory_games"), NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      string = setting_string_val("directory_games");
      len = strlen(setting_string_val("directory_games"));
      size = setting_get("directory_games")->size;
      nk_edit_string(ctx, NK_EDIT_SIMPLE, string, &len, size, nk_filter_default);

      nk_layout_row_dynamic(ctx, 30, 1);
      nk_checkbox_bool(ctx, __("video_vsync"), setting_bool_val("video_vsync"));
      nk_checkbox_bool(ctx, __("video_fullscreen"), setting_bool_val("video_fullscreen"));

      nk_label(ctx, "Core:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);

      const char* core_entries[100];
      for (unsigned i = 0; i < core_count; i++)
         core_entries[i] = core_info_list[i].core_name;

      current_core = nk_combo(ctx, core_entries, core_count, current_core, 30, nk_vec2(200,200));

      nk_layout_row_dynamic(ctx, 30, 2);
      if(current_core_info.supports_no_game)
      {
         if(nk_button_label(ctx, "Start core"))
         {
            core_load(core_info_list[current_core].file_name, &current_core_info, core_options, false);
            if(core_load_game(NULL))
            {
               running = true;
            }
         }
      }
      if (nk_button_label(ctx, "Load content"))
      {
         core_load(core_info_list[current_core].file_name, &current_core_info, core_options, false);
         if(core_load_game("rom.nes"))
         {
            running = true;
         }
      }

      if (core_count !=0 && (!initialized || previous_core != current_core))
      {
         core_load(core_info_list[current_core].file_name, &current_core_info, core_options, true);
         previous_core = current_core;
      }

      nk_layout_row_dynamic(ctx, 30, 2);
      nk_label(ctx, "Core version:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label(ctx, current_core_info.core_version, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label(ctx, "Valid extensions:", NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
      nk_label_wrap(ctx, current_core_info.extensions);

      nk_layout_row_dynamic(ctx, 160, 1);
      nk_group_begin(ctx, "Core flags", NK_WINDOW_TITLE | NK_WINDOW_ROM);
      nk_layout_row_dynamic(ctx, 30, 1);
      nk_checkbox_bool(ctx, "Supports running without game", &current_core_info.supports_no_game);
      nk_checkbox_bool(ctx, "Requires game full path", &current_core_info.full_path);
      nk_checkbox_bool(ctx, "Block extraction of archives", &current_core_info.block_extract);
      nk_group_end(ctx);

      nk_layout_row_dynamic(ctx, 240, 1);
      int flags = running ? NK_WINDOW_TITLE : NK_WINDOW_TITLE | NK_WINDOW_ROM;
      nk_group_begin(ctx, "Core options", flags);
      for (unsigned i = 0; i < core_option_count(); i++)
      {
         core_option_t* option = &core_options[i];
         char* description = option->description;
         struct string_list* values = kami_core_option_get_values(option);
         nk_layout_row_dynamic(ctx, 30, 1);
         nk_label(ctx, description, NK_TEXT_ALIGN_CENTERED | NK_TEXT_LEFT);
         nk_layout_row_dynamic(ctx, 30, 1);

         int index = kami_core_option_get_index(option, values);

         index = nk_combo_string_list(ctx, values, index, 30, nk_vec2(200,200));
         if (index)
         {
            char* value = values->elems[index].data;
            kami_core_option_update(option, value);
         }
      }
      nk_group_end(ctx);
   }
   nk_end(ctx);

   initialized = true;
   if (running)
   {
      core_run(&frame_buffer, &kami_render_audio);
         if (nk_begin(ctx, "Video output", nk_rect(520, 10, 664, 700),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
         {
            nk_layout_space_begin(ctx, NK_STATIC,400, INT_MAX);
            nk_layout_space_push(ctx, nk_rect(0, 0, 640, 480));
            nk_image(ctx,  nk_image_id(kami_render_framebuffer(&frame_buffer, current_core_info.pixel_format)));
            nk_layout_space_end(ctx);
         }
         nk_end(ctx);
   }
}

int main(int argc, char *argv[])
{
   /* Platform */
   int win_width, win_height;
   int running = 1;

   /* Configuration setup */
   common_config_load();


   if (!create_window(app_name, WINDOW_WIDTH, WINDOW_HEIGHT))
      return -1;
   SDL_Window *window = get_window();
   SDL_GLContext context = get_context();

   const char* glsl_version = get_glsl_version();

   logger(LOG_INFO, tag, "audio driver: %s\n", SDL_GetCurrentAudioDriver());
   init_audio_device();

   ctx = nk_sdl_init(window);
   /* Load Fonts: if none of these are loaded a default font will be used  */
   /* Load Cursor: if you uncomment cursor loading please hide the cursor */
   {
      struct nk_font_atlas *atlas;
      nk_sdl_font_stash_begin(&atlas);
      /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
      nk_sdl_font_stash_end();
      /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
      /*nk_style_set_font(ctx, &roboto->handle);*/}

      bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
      while (running)
      {
         /* Input */
         SDL_Event evt;
         gui_render(ctx);
         nk_input_begin(ctx);
         while (SDL_PollEvent(&evt))
         {
            if (evt.type == SDL_QUIT) goto cleanup;
            nk_sdl_handle_event(&evt);
         }
         nk_input_end(ctx);

         /* Draw */
         SDL_GetWindowSize(window, &win_width, &win_height);
         glViewport(0, 0, win_width, win_height);
         glClear(GL_COLOR_BUFFER_BIT);
         glClearColor(bg.r, bg.g, bg.b, bg.a);
         /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
          * with blending, scissor, face culling, depth test and viewport and
          * defaults everything back into a default state.
          * Make sure to either a.) save and restore or b.) reset your own state after
          * rendering the UI. */
         nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
         SDL_GL_SwapWindow(window);
      }

cleanup:
      nk_sdl_shutdown();
      destroy_window();
      return 0;
}

