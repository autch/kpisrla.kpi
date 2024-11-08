#include "pch.h"
#include "SampleHolder.h"

uint32_t SampleHolder::Render(BYTE* pBuffer, int bits_per_sample, DWORD dwSampleOffset, DWORD dwSampleUpperLimit)
{
	int num_channels = static_cast<int>(channels.size());
	for (uint32_t s = dwSampleOffset, offset = 0; s < dwSampleUpperLimit; s++, offset++)
	{
		for (int ch = 0; ch < num_channels; ch++)
		{
			int32_t sample = channels[ch][s];
			switch (bits_per_sample)
			{
			case 8:
				reinterpret_cast<int8_t*>(pBuffer)[offset * num_channels + ch] = static_cast<int8_t>(sample - 128);
				break;
			case 16:
				reinterpret_cast<int16_t*>(pBuffer)[offset * num_channels + ch] = static_cast<int16_t>(sample);
				break;
			case 24:
			{
				auto p = reinterpret_cast<int8_t*>(pBuffer);
				auto pp = p + static_cast<size_t>(offset * num_channels + ch) * 3;
				memcpy(pp, &sample, 3);
				break;
			}
			case 32:
				reinterpret_cast<int32_t*>(pBuffer)[offset * num_channels + ch] = sample;
				break;
			}
		}
	}
	return dwSampleUpperLimit - dwSampleOffset;
}
