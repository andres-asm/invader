#include "gamepad.h"

static const char* tag = "[input]";

GamePad::GamePad()
{
   /*initialize variables*/
   gamepad = NULL;
   gamepad_id = -1;

   /*set button and axis states to zero*/
   memset(button_state, 0, sizeof(Uint8) * SDL_CONTROLLER_BUTTON_MAX);
   memset(axis_values, 0, sizeof(float) * SDL_CONTROLLER_AXIS_MAX);
}

bool GamePad::Initialize(void)
{
   /*initialize game controller api*/
   // SDL_Init(SDL_INIT_GAMECONTROLLER);

   /*load mappings from file*/
   int entries = SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
   if (entries == -1)
   {
      logger(LOG_ERROR, tag, "error loading database: %s\n", SDL_GetError());
      return false;
   }
   else
      logger(LOG_INFO, tag, "database loaded succesfully\n");

   SDL_GameControllerEventState(SDL_IGNORE);
   return true;
}

void GamePad::Update(void)
{
   /*early return if no gamepad attached*/
   if (gamepad == NULL)
      return;

   /*update controller info from SDL*/
   SDL_GameControllerUpdate();

   /*update button states*/
   for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; ++b)
      button_state[b] = SDL_GameControllerGetButton(gamepad, (SDL_GameControllerButton)b);

   /*update axis values*/
   for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; ++a)
   {
      axis_values[a] = SDL_GameControllerGetAxis(gamepad, (SDL_GameControllerAxis)a);
   }
}

void GamePad::ReceiveEvent(const SDL_Event& oEvent)
{
   switch (oEvent.type)
   {
      /*controller attached event*/
      case SDL_CONTROLLERDEVICEADDED:
      {
         /*for now only support the first attached gamepad*/
         if (gamepad == NULL)
         {
            /*open the controller*/
            gamepad_id = oEvent.cdevice.which;
            gamepad = SDL_GameControllerOpen(gamepad_id);

            /*set button and axis states to zero*/
            memset(button_state, 0, sizeof(Uint8) * SDL_CONTROLLER_BUTTON_MAX);
            memset(axis_values, 0, sizeof(float) * SDL_CONTROLLER_AXIS_MAX);
            logger(LOG_INFO, tag, "device added %d\n", gamepad);
         }
         break;
      }
      /*controller removed event*/
      case SDL_CONTROLLERDEVICEREMOVED:
      {
         /*only remove if it was the same gamepad*/
         if (gamepad_id = oEvent.cdevice.which)
         {
            gamepad_id = -1;
            gamepad = NULL;
         }
         break;
      }
   }
}

bool GamePad::GetButtonState(const SDL_GameControllerButton iButton) const
{
   return (button_state[iButton] == 1);
}

float GamePad::GetAxisValue(const SDL_GameControllerAxis iAxis) const
{
   return axis_values[iAxis];
}
