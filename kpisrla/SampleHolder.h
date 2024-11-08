#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

struct SampleHolder
{
	uint32_t num_of_samples{0};	// number of samples decoded in this generation
	uint32_t samples_used{ 0 };	// number of samples used in this generation

	using samples_in_channel = std::vector<int32_t>;
	std::vector<samples_in_channel> channels;

	std::vector<int32_t*> buffer_ptrs;

	SampleHolder(int num_channels, uint32_t max_samples)
	{
		resize(num_channels, max_samples);
	}
	~SampleHolder() = default;
	SampleHolder(const SampleHolder& other)
		: num_of_samples(other.num_of_samples), samples_used(other.samples_used), channels(other.channels)
	{
		set_ptrs();
	}
	SampleHolder(SampleHolder&& other) noexcept
	{
		*this = std::move(other);
	}
	SampleHolder& operator=(const SampleHolder& other)
	{
		num_of_samples = other.num_of_samples;
		samples_used = other.samples_used;
		channels = other.channels;
		set_ptrs();
		return *this;
	}
	SampleHolder& operator=(SampleHolder&& other) noexcept
	{
		num_of_samples = other.num_of_samples;
		samples_used = other.samples_used;
		channels = std::move(other.channels);
		set_ptrs();
		other.buffer_ptrs.clear();
		return *this;
	}

	int32_t** getBufferPtrs()
	{
		if (buffer_ptrs.empty())
			set_ptrs();
		return buffer_ptrs.data();
	}

	void set_ptrs()
	{
		int num_channels = static_cast<int>(channels.size());
		buffer_ptrs.resize(num_channels);
		for(int ch = 0; ch < num_channels; ch++)
		{
			buffer_ptrs[ch] = channels[ch].data();
		} 
	}

	void resize(int num_channels, uint32_t max_samples)
	{
		num_of_samples = samples_used = 0;
		channels.resize(num_channels);
		for (auto& samples_in_ch : channels)
		{
			samples_in_ch.resize(max_samples);
		}
	}

	uint32_t Render(BYTE* pBuffer, int bits_per_sample, DWORD dwSampleOffset, DWORD dwSampleUpperLimit);
};
