#include "pch.h"
#include "SampleHolder.h"

uint32_t SampleHolder::Render(BYTE* pBuffer, int bits_per_sample, DWORD dwSampleOffset, DWORD dwSampleUpperLimit) const
{
	switch (bits_per_sample)
	{
	case 8:
		return render_8(pBuffer, dwSampleOffset, dwSampleUpperLimit);
	case 16:
		return render_16(pBuffer, dwSampleOffset, dwSampleUpperLimit);
	case 24:
		return render_24(pBuffer, dwSampleOffset, dwSampleUpperLimit);
	case 32:
		return render_32(pBuffer, dwSampleOffset, dwSampleUpperLimit);
	default:
		return 0;
	}
}

uint32_t SampleHolder::render_8(BYTE* pBuffer, DWORD dwSampleOffset, DWORD dwSampleUpperLimit) const
{
	int num_channels = static_cast<int>(channels.size());
	for (uint32_t s = dwSampleOffset, offset = 0; s < dwSampleUpperLimit; s++, offset++)
	{
		for (int ch = 0; ch < num_channels; ch++)
		{
			int32_t sample = channels[ch][s];
			reinterpret_cast<int8_t*>(pBuffer)[offset * num_channels + ch] = static_cast<int8_t>(sample - 128);
		}
	}
	return dwSampleUpperLimit - dwSampleOffset;
}

uint32_t SampleHolder::render_16(BYTE* pBuffer, DWORD dwSampleOffset, DWORD dwSampleUpperLimit) const
{
	int num_channels = static_cast<int>(channels.size());
	for (uint32_t s = dwSampleOffset, offset = 0; s < dwSampleUpperLimit; s++, offset++)
	{
		for (int ch = 0; ch < num_channels; ch++)
		{
			int32_t sample = channels[ch][s];
			reinterpret_cast<int16_t*>(pBuffer)[offset * num_channels + ch] = static_cast<int16_t>(sample);
		}
	}
	return dwSampleUpperLimit - dwSampleOffset;
}

uint32_t SampleHolder::render_24(BYTE* pBuffer, DWORD dwSampleOffset, DWORD dwSampleUpperLimit) const
{
	int num_channels = static_cast<int>(channels.size());
	for (uint32_t s = dwSampleOffset, offset = 0; s < dwSampleUpperLimit; s++, offset++)
	{
		for (int ch = 0; ch < num_channels; ch++)
		{
			int32_t sample = channels[ch][s];
			auto p = reinterpret_cast<int8_t*>(pBuffer);
			auto pp = p + static_cast<size_t>(offset * num_channels + ch) * 3;
			memcpy(pp, &sample, 3);
		}
	}
	return dwSampleUpperLimit - dwSampleOffset;
}

uint32_t SampleHolder::render_32(BYTE* pBuffer, DWORD dwSampleOffset, DWORD dwSampleUpperLimit) const
{
	int num_channels = static_cast<int>(channels.size());
	for (uint32_t s = dwSampleOffset, offset = 0; s < dwSampleUpperLimit; s++, offset++)
	{
		for (int ch = 0; ch < num_channels; ch++)
		{
			int32_t sample = channels[ch][s];
			reinterpret_cast<int32_t*>(pBuffer)[offset * num_channels + ch] = sample;
		}
	}
	return dwSampleUpperLimit - dwSampleOffset;
}
