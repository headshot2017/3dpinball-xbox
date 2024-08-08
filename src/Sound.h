#pragma once

class Sound
{
public:
	static bool Init(int channels, bool enableFlag);
	static void Enable(bool enableFlag);
	static void Activate();
	static void Deactivate();
	static void Close();
	static void PlaySound(int16_t* buf, int time, int size, int samplerate);
	static int16_t* LoadWaveFile(const std::string& lpName);
	static void FreeSound(int16_t* buf);
	static void SetChannels(int channels);
private:
	static int num_channels;
	static bool enabled_flag;
	static int* TimeStamps;
};
