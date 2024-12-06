#pragma once

#include <functional>
#include <cstdint>
#include <memory>

class Deinterleaver
{
public:
	using fnReadSample = std::function<int32_t(void* input, size_t sample, int ch, int num_channels)>;

	Deinterleaver(int num_channels, int num_samples)
		: num_channels(num_channels), num_samples(num_samples)
	{
		buffer = std::make_unique<int32_t[]>(static_cast<size_t>(num_channels) * num_samples);
	}

	fnReadSample get_accessor(int bits_per_sample)
	{
		switch(bits_per_sample)
		{
		case 8:
			return [](void* input, size_t sample, int ch, int num_channels) -> int32_t
				{
					auto p = static_cast<uint8_t*>(input);
					return static_cast<int32_t>(p[sample * num_channels + ch]) - 128;
				};
		case 16:
			return [](void* input, size_t sample, int ch, int num_channels) -> int32_t
				{
					auto p = static_cast<int16_t*>(input);
					return (p[sample * num_channels + ch] << 16) >> 16;
				};
		case 24:
			return [](void* input, size_t sample, int ch, int num_channels) -> int32_t
				{
					auto p = static_cast<uint8_t*>(input);
					// 24bit signed little endian
					int32_t s = *reinterpret_cast<int32_t*>(p + (sample * num_channels + ch) * 3);
					return (s << 8) >> 8;
				};
		case 32:
			return [](void* input, size_t sample, int ch, int num_channels) -> int32_t
				{
					auto p = static_cast<int32_t*>(input);
					return p[sample * num_channels + ch];
				};
		default:
			return [](void* input, size_t sample, int ch, int num_channels) -> int32_t
				{
					return 0;
				};
		}
	}

	template<typename Conv>
	void deinterleave(void* input, size_t samples_have, Conv fn, int32_t** output)
	{
		for (int ch = 0; ch < num_channels; ch++)
		{
			for (size_t s = 0; s < samples_have; s++)
			{
				buffer[ch * num_samples + s] = fn(input, s, ch, num_channels);
			}
		}
		for (int ch = 0; ch < num_channels; ch++)
		{
			output[ch] = buffer.get() + static_cast<ptrdiff_t>(ch * num_samples);
		}
	}

private:
	int num_channels;
	int num_samples;
	std::unique_ptr<int32_t[]> buffer;
};

