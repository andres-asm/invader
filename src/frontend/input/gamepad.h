#ifndef INPUT_H_
#define INPUT_H_

#include "common.h"

class GamePad
{
public:
   /*constructor*/
   GamePad();

   /*control flow functions*/
   bool Initialize(void);
   void Update(void);
   void Release(void);

   /*sdl event handling*/
   void ReceiveEvent(const SDL_Event& oEvent);

   /*button state*/
   bool GetButtonState(const SDL_GameControllerButton iButton) const;

   /*axis state*/
   float GetAxisValue(const SDL_GameControllerAxis iAxis) const;

private:
   /*internal variables*/
   SDL_GameController* gamepad;
   int gamepad_id;

   /*internal state*/
   Uint8 button_state[SDL_CONTROLLER_BUTTON_MAX];
   float axis_values[SDL_CONTROLLER_AXIS_MAX];
};

#endif