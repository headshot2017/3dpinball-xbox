#include "xbox_graphics.h"
#include "pch.h"
#include "maths.h"
#include "render.h"
#include "winmain.h"

#include <hal/video.h>
#include <SDL.h>

#include <cstdio>
#include <cstring>
#include <malloc.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

void xbox_graphics::Initialize()
{
	XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

	if (SDL_VideoInit(0) < 0)
		winmain::PrintFatalError("Failed to init SDL video\n");

	window = SDL_CreateWindow("3D Pinball", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if(!window)
	{
		SDL_VideoQuit();
		winmain::PrintFatalError("Failed to create window\n");
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		SDL_VideoQuit();
		winmain::PrintFatalError("Failed to create renderer\n");
	}

	SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
    SDL_RenderClear(renderer);

	// splash image is 320x222 RGB32
	uint8_t* splash_raw = new uint8_t[320*222*3];

	FILE* f = fopen("D:\\cd_root\\splash.raw", "rb");
	fread(splash_raw, 320*222*3, 1, f);
	fclose(f);

	delete[] splash_raw;
}

void xbox_graphics::SetupTexture()
{
	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		render::vscreen->Width,
		render::vscreen->Height
	);
}

void xbox_graphics::SwapBuffers()
{
	SDL_Rect pos = {
		640/2 - (render::vscreen->Width/2),
		480/2 - (render::vscreen->Height/2),
		render::vscreen->Width,
		render::vscreen->Height
	};
	SDL_RenderCopy(renderer, texture, NULL, &pos);
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}

void xbox_graphics::ShowSplash(std::string text)
{
	debugPrint("%s\n", text.c_str());
	/*
	vid_clear(0,0,0);

	int startX = (vid_mode->width/2 - 320/2);
	int startY = (vid_mode->height/2 - 222/2);
	int startInd = (startY * vid_mode->width + startX);

	for (int y=0; y<222; y++)
	{
		for (int x=0; x<320; x++)
		{
			vram_s[startInd + y*vid_mode->width+x] = splash_raw[y*320+x];
		}
	}

	int y = startY+222+4;
	int textWidth = 0, x = 0;

	// determine text width first
	for (uint32_t i=0; i<text.size(); i++)
	{
		if (text.at(i) == '\n')
		{
			x = 0;
			continue;
		}

		x += 12;
		if (x > textWidth) textWidth = x;
	}

	// now we can draw
	startX = (vid_mode->width/2 - textWidth/2);
	x = startX;
	for (uint32_t i=0; i<text.size(); i++)
	{
		if (text.at(i) == '\n')
		{
			x = startX;
			y += 24;
			continue;
		}

		bfont_draw(vram_s + y*vid_mode->width+x, vid_mode->width, 1, text.at(i));
		x += 12;
	}
	*/
}

void xbox_graphics::UpdateFull()
{
	SDL_UpdateTexture(texture, 0, render::vscreen->BmpBufPtr1, render::vscreen->Width*4);

	render::get_dirty_regions().clear();
}

void xbox_graphics::Update()
{
	/*
	int startPos = (vid_mode->height/2 - render::vscreen->Height/2) * vid_mode->width + (vid_mode->width/2 - render::vscreen->Width/2);

	for (uint32_t i=0; i<render::get_dirty_regions().size(); i++)
	{
		const rectangle_type& dirty = render::get_dirty_regions()[i];
		for (int y=dirty.YPosition; y<dirty.YPosition + dirty.Height; y++)
		{
			for (int x=dirty.XPosition; x<dirty.XPosition + dirty.Width; x++)
			{
				Rgba c = render::vscreen->BmpBufPtr1[y*render::vscreen->Width+x].rgba;
				vram_s[startPos + y*vid_mode->width+x] =
					((c.Red >> 3))
					| ((c.Green & 0xFC) << 3)
					| ((c.Blue & 0xF8) << 8);
			}
		}
	}
	*/

	render::get_dirty_regions().clear();
}
