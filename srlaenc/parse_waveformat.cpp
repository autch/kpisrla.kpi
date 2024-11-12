#include <Windows.h>
#include <mmreg.h>

#include "srlaenc.h"
#include "WaveReader.h"

int parse_waveformat(WaveReader& wav, pcm_params* params)
{
	uint8_t waveformat[128];
    size_t ckSize = 0;

	if(!wav.ReadWaveFormat(waveformat, &ckSize))
	{
		fprintf(stderr, "Failed to read waveformat\n");
		return 1;
	}

	auto wf = reinterpret_cast<WAVEFORMAT*>(waveformat);
	auto pcmwf = reinterpret_cast<PCMWAVEFORMAT*>(waveformat);
	auto wfex = reinterpret_cast<WAVEFORMATEX*>(waveformat);
	auto wfext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(waveformat);

	if (wf->wFormatTag == WAVE_FORMAT_PCM)
	{
		params->channels = wf->nChannels;
		params->samples_per_sec = wf->nSamplesPerSec;
		params->block_align = wf->nBlockAlign;
		if (ckSize == sizeof(WAVEFORMATEX))
		{
			params->bits_per_sample = wfex->wBitsPerSample;
		}
		else if(ckSize == sizeof(PCMWAVEFORMAT))
		{
			params->bits_per_sample = pcmwf->wBitsPerSample;
		}
		else
		{
			// some WAV files have invalid ckSize, so we need to calculate bits per sample
			printf("ckSize: %llu, guessing bits_per_sample\n", ckSize);
			params->bits_per_sample = wf->nAvgBytesPerSec / wf->nSamplesPerSec / wf->nChannels * 8;
		}
		return 0;		// ok
	}
    if(wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
		if (ckSize < sizeof(WAVEFORMATEXTENSIBLE))
		{
			fprintf(stderr, "Invalid WAVEFORMATEXTENSIBLE size\n");
			return 1;
		}
		if (wfex->cbSize < sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))
		{
			fprintf(stderr, "Invalid WAVEFORMATEXTENSIBLE cbSize\n");
			return 1;
		}
		if (wfext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
		{
			params->channels = wf->nChannels;
			params->samples_per_sec = wf->nSamplesPerSec;
			params->block_align = wf->nBlockAlign;
			params->bits_per_sample = wfex->wBitsPerSample;
			return 0;		// ok
		}
		fprintf(stderr, "Unsupported WAVEFORMATEXTENSIBLE SubFormat\n");
	}
	else
	{
		fprintf(stderr, "Unknown format\n");
	}

	return 1;
}
