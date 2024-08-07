#pragma once

#include <string>

class xbox_graphics
{
public:
    static void Initialize();
	static void SetupTexture();

    static void SwapBuffers();

	static void ShowSplash(std::string text);

	static void UpdateFull();
	static void Update();
};