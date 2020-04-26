# piccolo
piccolo is a modular frontend side implementation of the libretro API.

# Goals
To provide a complete implementation of the libretro API that other applications can re-use without worrying with all the API details.

# Current Progress
## Backend
- [X] core loading
- [X] core initialization
- [X] content loading
- [X] video rendering
- [X] audio rendering
- [ ] input processing

# Using piccolo
As far as the current version there are a few dependencies I need to resolve before being able to just slot the files in the directory and use it.

So far piccolo requires:
- libretro.h
- util.h from invader
- strl.h from libretro-common
- dylib.h from libretro-common

I still need to re-organize both piccolo and invader somewhat to be able to provide an easy way to use it.
As far as implementation details a basic outline is the following:

```C
#include "piccolo.h"
core_option_t core_options[100];
core_info_t core_info;
core_frame_buffer_t frame_buffer;

/* you have to implement this function yourself considering your graphics API of choice*/
void do_stuff_with_video()
{

}

/* you have to implement this function yourself considering your audio API of choice*/
size_t do_stuff_with_audio(const int16_t *data, size_t frames)
{
   return 0;
}

void main()
{
   /*
      void load_core(const char *in, core_info_t *info, core_option_t *options, bool peek);
      - in is the path to the core file
      - info is a structure that will hold information about the running core
      - options is a structure that will hold core variables
      - peek means we're just looking for core_info, it will close the core immediately
   */
   load_core("path_to_core_file", &core_info, core_options, false);

   /*
      bool core_load_game(const char* filename);
      - filename is a valid path to a content file.
      You can also pass NULL if the core accepts running without content
      - returns true in case of success
   */

   bool loaded = core_load_game("path_to_content_file");
   if (loaded)
   {
      /*
         void core_run(core_frame_buffer_t *video_data, audio_cb_t audio_cb);
         - video_data is a structure that contains the actual video frame buffer and other video parameters owned by your application, passed to the core to be populated with data
         - audio_cb is a function callback that will be called by piccolo on every audio frame
      */
      for (;;)
      {
         core_run(&frame_buffer, &do_stuff_with_audio);
         do_stuff_with_video();
      }
   }
}
```

