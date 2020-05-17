#ifndef KAMI_H_
#define KAMI_H_

// system
#include <vector>

#include "asset.h"
#include "common.h"
#include "libretro/piccolo.h"

enum device_gamepad_enum
{
   GAMEPAD_B = RETRO_DEVICE_ID_JOYPAD_B,
   GAMEPAD_Y,
   GAMEPAD_SELECT,
   GAMEPAD_START,
   GAMEPAD_UP,
   GAMEPAD_DOWN,
   GAMEPAD_LEFT,
   GAMEPAD_RIGHT,
   GAMEPAD_A,
   GAMEPAD_X,
   GAMEPAD_L,
   GAMEPAD_R,
   GAMEPAD_L2,
   GAMEPAD_R2,
   GAMEPAD_L3,
   GAMEPAD_R3,
   GAMEPAD_GUIDE,
   GAMEPAD_LEFT_STICK_X,
   GAMEPAD_LEFT_STICK_Y,
   GAMEPAD_RIGHT_STICK_X,
   GAMEPAD_RIGHT_STICK_Y,
   GAMEPAD_LAST
};

extern const char* device_gamepad_asset_names[];

// kami class controls a core completely, provides the complete I/O for the core including file I/O, video, audio,
// input. Implementation is GUI toolkit / paradygm specific, only common code is defined in kami.cpp
class Kami
{
private:
   // variables
   // backend related variables
   PiccoloWrapper* piccolo;
   int current_core;
   int previous_core;
   int core_count;
   unsigned status;
   const char* core_entries[100];
   core_info_t* core_info;
   core_info_t core_info_list[100];

   // frontend related variables
   bool frontend_supports_bitmasks;
   bool core_loaded;
   bool file_open_dialog_is_open;
   bool file_open_dialog_result_ok;
   char content_file_name[PATH_MAX_LENGTH];
   input_state_t input_state[MAX_PORTS];
   input_descriptor_t input_descriptors[MAX_PORTS][MAX_IDS];
   core_frame_buffer_t* video_data;

public:
   Kami()
   {
      status = CORE_STATUS_NONE;
      current_core = 0;
      previous_core = -1;
      core_count = 0;
      core_loaded = false;
      this->piccolo = new PiccoloWrapper();
      core_info = piccolo->get_info();
   }

   ~Kami() { delete piccolo; }

   // common functions
   bool CoreListInit(const char* path);
   struct string_list* OptionGetValues(core_option_t* option);
   unsigned OptionGetIndex(core_option_t* option, struct string_list* values);
   void OptionUpdate(core_option_t* option, const char* value);
   void ControllerPortUpdate(int port, int device) { piccolo->set_controller_port_device(port, device); }
   void ParseInputDescriptors();
   input_state_t GetInputState(int port) { return input_state[port]; }

   // implementation specific functions
   void Main(const char* title);
   int RenderVideo();
   static void InputPoll();

   // TODO: this shouldn't be a part of this class
   void TextureListInit(const char* path);
};

// TODO: reimplement these
size_t kami_render_audio(const int16_t* data, size_t frames);
bool kami_init_audio();

#endif