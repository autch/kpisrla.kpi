#pragma once

#pragma comment(lib, "winmm")

#include <Windows.h>
#include <string>
#include <stack>
#include <memory>

class WaveReader
{
public:
	using ChunkStack = std::stack<MMCKINFO>;

	WaveReader() = default;
	WaveReader(const WaveReader& other) = delete;
	WaveReader(WaveReader&& other) noexcept: hMMIO(other.hMMIO), chunks(std::move(other.chunks))
	{
		other.hMMIO = nullptr;
	}

	WaveReader& operator=(const WaveReader& other) = delete;
	WaveReader& operator=(WaveReader&& other) noexcept
	{
		hMMIO = other.hMMIO;
		chunks = std::move(other.chunks);

		other.hMMIO = nullptr;
		return *this;
	}

	bool Open(const std::wstring& filename)
	{
		hMMIO = mmioOpenW(const_cast<LPWSTR>(filename.c_str()), nullptr, MMIO_READ);
		return hMMIO != nullptr;
	}

	bool Open(const std::string& filename)
	{
		hMMIO = mmioOpenA(const_cast<LPSTR>(filename.c_str()), nullptr, MMIO_READ);
		return hMMIO != nullptr;
	}

	// waveformat must be a pointer to a buffer of, at least sizeof(WAVEFORMATEXTENSIBLE)
	bool ReadWaveFormat(void* waveformat, size_t* ckSize)
	{
		MMCKINFO RIFFchunk{}, fmtChunk{};

		RIFFchunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if (Descend(RIFFchunk, MMIO_FINDRIFF) != 0)
			return false;

		fmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if (Descend(fmtChunk, MMIO_FINDCHUNK) != 0)
			return false;

		if (Read(waveformat, fmtChunk.cksize) != fmtChunk.cksize)
			return false;
		*ckSize = fmtChunk.cksize;

		Ascend();
		return true;
	}

	bool FindDataChunk(MMCKINFO* chunkRet)
	{
		MMCKINFO dataChunk;
		dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		auto ret = Descend(dataChunk, MMIO_FINDCHUNK);
		if(ret == MMSYSERR_NOERROR)
		{
			*chunkRet = dataChunk;
			return true;
		}
		return false;
	}

	LONG Read(void* buffer, LONG size)
	{
		return mmioRead(hMMIO, reinterpret_cast<HPSTR>(static_cast<uint8_t*>(buffer)), size);
	}

	LONG Seek(LONG offset, int origin)
	{
		return mmioSeek(hMMIO, offset, origin);
	}

	LONG SeekWithinChunk(LONG offset)
	{
		MMCKINFO& ck = chunks.top();
		return mmioSeek(hMMIO, ck.dwDataOffset + offset, SEEK_SET);
	}

	MMRESULT Descend(MMCKINFO& ck, UINT fuDescend)
	{
		auto ret = mmioDescend(hMMIO, &ck, fuDescend == MMIO_FINDRIFF ? nullptr : &chunks.top(), fuDescend);
		if (ret == MMSYSERR_NOERROR)
		{
			chunks.push(ck);
		}
		return ret;
	}
	
	MMRESULT Ascend()
	{
		MMCKINFO ck = chunks.top();
		auto ret = mmioAscend(hMMIO, &ck, 0);
		if(ret == MMSYSERR_NOERROR)
		{
			chunks.pop();
		}
		return ret;
	}

	void Close()
	{
		if (hMMIO == nullptr)
			return;
		while (!chunks.empty())
		{
			mmioAscend(hMMIO, &chunks.top(), 0);
			chunks.pop();
		}
		mmioClose(hMMIO, 0);
		hMMIO = nullptr;
	}

	~WaveReader()
	{
		Close();
	}

private:
	HMMIO hMMIO{nullptr};
	ChunkStack chunks;
};
