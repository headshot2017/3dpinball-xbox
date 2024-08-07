#include "xbox_input.h"

#include <unordered_map>
#include <SDL.h>

#include "pch.h"
#include "winmain.h"

enum
{
	CONT_A     = 1<<0,
	CONT_B     = 1<<1,
	CONT_X     = 1<<2,
	CONT_Y     = 1<<3,
	CONT_BACK  = 1<<4,
	CONT_START = 1<<5,
	CONT_LB    = 1<<6,
	CONT_RB    = 1<<7,
	CONT_UP    = 1<<8,
	CONT_DOWN  = 1<<9,
	CONT_LEFT  = 1<<10,
	CONT_RIGHT = 1<<11,
	CONT_LS    = 1<<12,
	CONT_RS    = 1<<13
};

static SDL_GameController *pad = NULL;
static std::unordered_map<int, int> btnMappings = {
	{SDL_CONTROLLER_BUTTON_A, CONT_A},
	{SDL_CONTROLLER_BUTTON_B, CONT_B},
	{SDL_CONTROLLER_BUTTON_X, CONT_X},
	{SDL_CONTROLLER_BUTTON_Y, CONT_Y},
	{SDL_CONTROLLER_BUTTON_BACK, CONT_BACK},
	{SDL_CONTROLLER_BUTTON_START, CONT_START},
	{SDL_CONTROLLER_BUTTON_LEFTSHOULDER, CONT_LB},
	{SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, CONT_RB},
	{SDL_CONTROLLER_BUTTON_DPAD_UP, CONT_UP},
	{SDL_CONTROLLER_BUTTON_DPAD_DOWN, CONT_DOWN},
	{SDL_CONTROLLER_BUTTON_DPAD_LEFT, CONT_LEFT},
	{SDL_CONTROLLER_BUTTON_DPAD_RIGHT, CONT_RIGHT},
	{SDL_CONTROLLER_BUTTON_LEFTSTICK, CONT_LS},
	{SDL_CONTROLLER_BUTTON_RIGHTSTICK, CONT_RS},
};

unsigned int xbox_input::xboxButtonsDown = 0;
unsigned int xbox_input::xboxButtonsUp = 0;
unsigned int xbox_input::xboxButtonsHeld = 0;
bool xbox_input::xboxLeftTriggerDown = false;
bool xbox_input::xboxLeftTriggerUp = false;
bool xbox_input::xboxRightTriggerDown = false;
bool xbox_input::xboxRightTriggerUp = false;

static unsigned int leftKey, rightKey, launchKey, upKey, nudgeKey;

void xbox_input::Initialize()
{
	if (SDL_Init(SDL_INIT_GAMECONTROLLER))
	{
		winmain::PrintFatalError("Failed to init gamecontroller\n");
		return;
	}

	leftKey = CONT_LEFT | CONT_LS | CONT_X;
	rightKey = CONT_RIGHT | CONT_RS | CONT_B;
	launchKey = CONT_DOWN | CONT_A;
	upKey = CONT_UP;
	nudgeKey = CONT_Y;
}

void xbox_input::ScanPads()
{
	xboxButtonsDown = 0;
	xboxButtonsUp = 0;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_CONTROLLERDEVICEADDED:
			{
				SDL_GameController *newPad = SDL_GameControllerOpen(e.cdevice.which);
				if (!pad) pad = newPad;
				break;
			}

			case SDL_CONTROLLERDEVICEREMOVED:
				if (pad == SDL_GameControllerFromInstanceID(e.cdevice.which))
					pad = NULL;
				SDL_GameControllerClose(SDL_GameControllerFromInstanceID(e.cdevice.which));
				break;

			case SDL_CONTROLLERBUTTONDOWN:
				if (pad != SDL_GameControllerFromInstanceID(e.cdevice.which))
					break;
				xboxButtonsDown |= btnMappings[e.cbutton.button];
				xboxButtonsHeld |= btnMappings[e.cbutton.button];
				break;

			case SDL_CONTROLLERBUTTONUP:
				if (pad != SDL_GameControllerFromInstanceID(e.cdevice.which))
					break;
				xboxButtonsUp |= btnMappings[e.cbutton.button];
				xboxButtonsHeld &= ~btnMappings[e.cbutton.button];
				break;
		}
	}

	SDL_GameControllerUpdate();
}

void xbox_input::Clear()
{
	
}

bool xbox_input::Exit()
{
	return false;
}

bool xbox_input::Pause()
{
	return xboxButtonsDown & CONT_START;
}

bool xbox_input::NewGame()
{
	return xboxButtonsDown & CONT_BACK;
}

bool xbox_input::LaunchBallDown()
{
	return !(xboxButtonsHeld & nudgeKey) && (xboxButtonsDown & launchKey);
}

bool xbox_input::LaunchBallUp()
{
	return !(xboxButtonsHeld & nudgeKey) && (xboxButtonsUp & launchKey);
}

bool xbox_input::MoveLeftPaddleDown()
{
	return !(xboxButtonsHeld & nudgeKey) && ((xboxButtonsDown & leftKey) || xboxLeftTriggerDown);
}

bool xbox_input::MoveLeftPaddleUp()
{
	return !(xboxButtonsHeld & nudgeKey) && ((xboxButtonsUp & leftKey) || xboxLeftTriggerUp);
}

bool xbox_input::MoveRightPaddleDown()
{
	return !(xboxButtonsHeld & nudgeKey) && ((xboxButtonsDown & rightKey) || xboxRightTriggerDown);
}

bool xbox_input::MoveRightPaddleUp()
{
	return !(xboxButtonsHeld & nudgeKey) && ((xboxButtonsUp & rightKey) || xboxRightTriggerUp);
}

bool xbox_input::NudgeLeftDown()
{
	return (xboxButtonsHeld & nudgeKey) && (xboxButtonsDown & leftKey);
}

bool xbox_input::NudgeLeftUp()
{
	return (xboxButtonsHeld & nudgeKey) && (xboxButtonsUp & leftKey);
}

bool xbox_input::NudgeRightDown()
{
	return (xboxButtonsHeld & nudgeKey) && (xboxButtonsDown & rightKey);
}

bool xbox_input::NudgeRightUp()
{
	return (xboxButtonsHeld & nudgeKey) && (xboxButtonsUp & rightKey);
}

bool xbox_input::NudgeUpDown()
{
	return (xboxButtonsHeld & nudgeKey) && (xboxButtonsDown & upKey);
}

bool xbox_input::NudgeUpUp()
{
	return (xboxButtonsHeld & nudgeKey) && (xboxButtonsUp & upKey);
}

bool xbox_input::Button1()
{
	return xboxButtonsDown & CONT_A;
}

bool xbox_input::Button2()
{
	return xboxButtonsDown & CONT_B;
}
