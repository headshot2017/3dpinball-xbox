#include "pch.h"
#include "loader.h"
#include "Sound.h"
#include "winmain.h"

#include <SDL.h>

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO
#include "dr_wav.h"

int Sound::num_channels;
bool Sound::enabled_flag = false;
int* Sound::TimeStamps = nullptr;

static SDL_AudioDeviceID dev;

bool Sound::Init(int channels, bool enableFlag)
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO))
	{
		debugPrint("Failed to init audio\n");
		return false;
	}

	SDL_AudioSpec want;
	SDL_zero(want);
	want.freq = 11025;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = 4096;
	want.callback = 0;
	want.userdata = 0;
	dev = SDL_OpenAudioDevice(0, 0, &want, 0, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (!dev)
	{
		debugPrint("Failed to open audio\n");
		return false;
	}
	SDL_PauseAudioDevice(dev, 0);

	SetChannels(channels);
	Enable(enableFlag);
	return true;
}

void Sound::Enable(bool enableFlag)
{
	enabled_flag = enableFlag;
}

void Sound::Activate()
{
	SDL_PauseAudioDevice(dev, 0);
}

void Sound::Deactivate()
{
	SDL_PauseAudioDevice(dev, 1);
}

void Sound::Close()
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	delete[] TimeStamps;
	TimeStamps = nullptr;
}

void Sound::PlaySound(int16_t* buf, int time, int size, int samplerate)
{
	if (!enabled_flag || !buf) return;
	SDL_QueueAudio(dev, buf, size);
	/*
	if (channel >= 0)
		TimeStamps[channel] = time;
	*/
}

int16_t* Sound::LoadWaveFile(const std::string& lpName)
{
	FILE* f = fopen(lpName.c_str(), "rb");
	if (!f) return 0;

	fseek(f, 0, SEEK_END);
	uint32_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* fileData = new uint8_t[size];
	fread(fileData, size, 1, f);
	fclose(f);

	drwav wavfp;

	if (!drwav_init_memory(&wavfp, fileData, size, NULL))
	{
		delete[] fileData;
		return 0;
	}

	int16_t* pSampleData = new int16_t[(uint32_t)wavfp.totalPCMFrameCount * wavfp.channels];
	if (pSampleData == 0)
	{
		drwav_uninit(&wavfp);
		delete[] fileData;
		return 0;
	}

	uint32_t totalRead = drwav_read_pcm_frames(&wavfp, wavfp.totalPCMFrameCount, pSampleData);
	if (!totalRead)
	{
		drwav_uninit(&wavfp);
		delete[] pSampleData;
		delete[] fileData;
		return 0;
	}

	if (wavfp.bitsPerSample == 8) // 8 bit
	{
		int16_t* _8bitdata = new int16_t[(uint32_t)wavfp.totalPCMFrameCount * wavfp.channels];
		drwav_u8_to_s16((drwav_int16*)_8bitdata, (drwav_uint8*)pSampleData, wavfp.totalPCMFrameCount);
		delete[] pSampleData;
		pSampleData = _8bitdata;
	}

	drwav_uninit(&wavfp);
	delete[] fileData;

	return pSampleData;
}

void Sound::FreeSound(int16_t* wave)
{
	if (wave)
		delete[] wave;
}

void Sound::SetChannels(int channels)
{
	if (channels <= 0)
		channels = 8;

	num_channels = channels;
	delete[] TimeStamps;
	TimeStamps = new int[num_channels]();
}
