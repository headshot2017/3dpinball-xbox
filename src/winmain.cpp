#include "pch.h"
#include "winmain.h"

#include <malloc.h>

#include "control.h"
#include "midi.h"
#include "options.h"
#include "pb.h"
#include "pinball.h"
#include "render.h"
#include "Sound.h"
#include "xbox_graphics.h"
#include "xbox_input.h"

int winmain::bQuit = 0;
int winmain::activated;
int winmain::DispFrameRate = 0;
int winmain::DispGRhistory = 0;
int winmain::single_step = 0;
int winmain::last_mouse_x;
int winmain::last_mouse_y;
int winmain::mouse_down;
int winmain::no_time_loss;

std::string winmain::DatFileName;
bool winmain::ShowSpriteViewer = false;
bool winmain::LaunchBallEnabled = true;
bool winmain::HighScoresEnabled = true;
bool winmain::DemoActive = false;
char *winmain::BasePath;
std::string winmain::FpsDetails;
double winmain::UpdateToFrameRatio;
winmain::DurationMs winmain::TargetFrameTime;
optionsStruct &winmain::Options = options::Options;

int winmain::WinMain(LPCSTR lpCmdLine)
{
	std::set_new_handler(memalloc_failure);

	// Initialize graphics and input

	xbox_graphics::Initialize();
	xbox_input::Initialize();

	// Set the base path for PINBALL.DAT

	BasePath = (char *)"D:\\cd_root\\";

	pinball::quickFlag = 0; // strstr(lpCmdLine, "-quick") != nullptr;
	DatFileName = options::get_string("Pinball Data", pinball::get_rc_string(168, 0));

	// Check for full tilt .dat file and switch to it automatically

	auto pinballDat = fopen(pinball::make_path_name(DatFileName).c_str(), "rb");
	bool hasPinball = !!(pinballDat);
	if (pinballDat) fclose(pinballDat);

	auto cadetFilePath = pinball::make_path_name("CADET.DAT");
	auto cadetDat = fopen(cadetFilePath.c_str(), "r");
	bool hasCadet = !!(cadetDat);
	if (cadetDat) fclose(cadetDat);

	if (!hasPinball && hasCadet)
	{
		DatFileName = "CADET.DAT";
		pb::FullTiltMode = true;
	}
	else if (hasPinball && hasCadet)
	{
		// pick a game
		xbox_graphics::ShowSplash("Press A to play 3D Pinball Space Cadet\nPress B to play Full Tilt! Pinball");

		while (true)
		{
			xbox_input::ScanPads();

			if (xbox_input::Button1())
				break;
			if (xbox_input::Button2())
			{
				DatFileName = "CADET.DAT";
				pb::FullTiltMode = true;
				break;
			}
		}

		xbox_input::Clear();
	}

	std::string gameName = (pb::FullTiltMode) ? "Full Tilt! Pinball" : "3D Pinball";
	xbox_graphics::ShowSplash("Loading " + gameName);

	// PB init from message handler

	{
		options::init();
		if (!Sound::Init(16, Options.Sounds))
			Options.Sounds = false;

		if (!pinball::quickFlag && !midi::music_init())
			Options.Music = false;

		if (pb::init())
		{
			PrintFatalError("Could not load game data:\n%s file is missing.\n", pinball::make_path_name(DatFileName).c_str());
		}
	}

	xbox_graphics::SetupTexture();

	// Initialize game

	pb::reset_table();
	pb::firsttime_setup();
	pb::replay_level(0);

	xbox_graphics::UpdateFull();

	// Begin main loop

	bQuit = false;

	while (!bQuit)
	{
		// Input

		xbox_input::ScanPads();

		if (xbox_input::Exit())
			break;

		if (xbox_input::Pause())
			pause();

		if (xbox_input::NewGame())
			new_game();

		pb::keydown();
		pb::keyup();

		if (!single_step)
		{
			// Update game when not paused

			pb::frame(1000.0f / 60.0f);
		}

		// Copy game screen buffer to texture
		xbox_graphics::Update();

		xbox_graphics::SwapBuffers();
	}

	printf("Uninitializing...\n");

	end_pause();

	options::uninit();
	midi::music_shutdown();
	pb::uninit();
	Sound::Close();

	return 0;
}

void winmain::memalloc_failure()
{
	midi::music_stop();
	Sound::Close();
	char *caption = pinball::get_rc_string(170, 0);
	char *text = pinball::get_rc_string(179, 0);

	PrintFatalError("%s %s\n", caption, text);
}

void winmain::end_pause()
{
	if (single_step)
	{
		pb::pause_continue();
		no_time_loss = 1;
	}
}

void winmain::new_game()
{
	end_pause();
	pb::replay_level(0);
}

void winmain::pause()
{
	pb::pause_continue();
	no_time_loss = 1;
}

void winmain::UpdateFrameRate()
{
	// UPS >= FPS
	auto fps = Options.FramesPerSecond, ups = Options.UpdatesPerSecond;
	UpdateToFrameRatio = static_cast<double>(ups) / fps;
	TargetFrameTime = DurationMs(1000.0 / ups);
}

void winmain::PrintFatalError(const char *message, ...)
{
	char buf[256] = {0};

	va_list args;
	va_start(args, message);
	vsprintf(buf, message, args);
	va_end(args);

	strcat(buf, "\nPress A to exit");
	debugPrint("%s\n", buf);

	while (true)
	{
		xbox_input::ScanPads();

		if (xbox_input::Button1())
			break;
	}

	XReboot();
}
