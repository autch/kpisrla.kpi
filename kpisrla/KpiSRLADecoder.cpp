#include "pch.h"
#include "KpiSRLADecoder.h"

#include <memory>

DWORD KpiSRLADecoder::Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile)
{
    pFile->AddRef();
    kpifile_ = pFile;

    kpi_InitMediaInfo(&mediainfo_);

    DWORD dwSongs;
    if ((dwSongs = IsSRLAFile(kpifile_)) > 0)
    {
        mediainfo_.dwCount = mediainfo_.dwNumber = dwSongs;
    }
    else
    {
        pFile->Release();
        kpifile_ = nullptr;
        return 0;
    }

    return mediainfo_.dwCount;
}

DWORD KpiSRLADecoder::Select(DWORD dwNumber, const KPI_MEDIAINFO** ppMediaInfo, IKpiTagInfo* pTagInfo, DWORD dwTagGetFlags)
{
    if (dwNumber == 0)
        return 0;

    if (dwNumber > mediainfo_.dwCount)
        return 0;

    if(IsSRLAFile(kpifile_) == 0)
    {
        return 0;
    }
	SRLADecoderConfig config;

    config.max_num_channels = SRLA_MAX_NUM_CHANNELS;
    config.max_num_parameters = SRLA_MAX_COEFFICIENT_ORDER;
    config.check_checksum = 0;
    if (decoder_ == nullptr && (decoder_ = SRLADecoder_Create(&config, nullptr, 0)) == nullptr)
    {
        OutputDebugStringW(L"Failed to create decoder handle.\n");
        return 0;
    }

	uint8_t buffer[SRLA_HEADER_SIZE];
    DWORD bytes_read;
	kpifile_->Seek(0, FILE_BEGIN);
	if ((bytes_read = kpifile_->Read(buffer, SRLA_HEADER_SIZE)) < SRLA_HEADER_SIZE)
	{
		OutputDebugStringW(L"Failed to read header.\n");
        return 0;
	}

    if (SRLADecoder_DecodeHeader(buffer, bytes_read, &header_) != SRLA_APIRESULT_OK)
    {
        OutputDebugStringW(L"Failed to get header information\n");
        return 0;
    }

	SRLADecoder_SetHeader(decoder_, &header_);

    fillMediaInfo(dwNumber);
    reset();

    if (ppMediaInfo != nullptr)
    {
	    *ppMediaInfo = &mediainfo_;
		if (pTagInfo != nullptr)
			fillTagInfo(pTagInfo);
    }

	return dwNumber;
}

void KpiSRLADecoder::fillMediaInfo(DWORD dwNumber)
{
	mediainfo_.dwNumber = dwNumber;
	mediainfo_.dwCount = 1;
	mediainfo_.dwFormatType = KPI_MEDIAINFO::FORMAT_PCM;
	mediainfo_.dwSampleRate = header_.sampling_rate;
    mediainfo_.nBitsPerSample = header_.bits_per_sample;
	mediainfo_.dwChannels = header_.num_channels;
	mediainfo_.dwSpeakerConfig = 0;
	mediainfo_.qwLength = kpi_SampleTo100ns(header_.num_samples, header_.sampling_rate);
    mediainfo_.qwLoop = 0;
	mediainfo_.qwFadeOut = 0;
    mediainfo_.dwLoopCount = 0;
	mediainfo_.dwUnitSample = header_.max_num_samples_per_block;
    mediainfo_.dwSeekableFlags = KPI_MEDIAINFO::SEEK_FLAGS_ROUGH;

	mediainfo_.dwVideoWidth = mediainfo_.dwVideoHeight = 0;
}

void KpiSRLADecoder::fillTagInfo(IKpiTagInfo* pTagInfo) const
{
    auto buf = std::make_shared<wchar_t[]>(128);
    auto set_tag = [buf, pTagInfo](auto name, auto format, auto value)
    {
        wsprintfW(buf.get(), format, value);
        pTagInfo->wSetValueW(name, -1, buf.get(), -1);
    };

	set_tag(L"SRLA_FormatVersion", L"%u", header_.format_version);
	set_tag(L"SRLA_EncoderVersion", L"%u", header_.codec_version);
	set_tag(L"SRLA_MaxSamplesPerBlock", L"%u", header_.max_num_samples_per_block);
	set_tag(L"SRLA_Preset", L"%u", header_.preset);
}

DWORD KpiSRLADecoder::Render(BYTE* pBuffer, DWORD dwSizeSample)
{
	if (decoder_ == nullptr || kpifile_ == nullptr)
	{
		return 0;
	}
	uint32_t samples_rendered = 0;
	block_header b_header;

	while(!holders_.empty() && dwSizeSample > 0)
	{
		SampleHolder& holders_top = holders_.front();
		uint32_t samples_produced = holders_top.Render(pBuffer, header_.bits_per_sample, holders_top.samples_used, holders_top.num_of_samples);
		dwSizeSample -= samples_produced;
		pBuffer += static_cast<size_t>(samples_produced) * header_.num_channels * (header_.bits_per_sample / 8);
		samples_rendered += samples_produced;
		holders_.pop_front();
	}

	while(dwSizeSample > 0)
	{
		uint8_t header_bytes[sizeof(block_header)];
		if (kpifile_->Read(header_bytes, sizeof(block_header)) < sizeof(block_header))
		{
			break;
		}
		memcpy(&b_header, header_bytes, sizeof(block_header));
		b_header.prepare();	

		if (b_header.sync_word != 0xffff)
		{
			// broken?
			break;
		}

		memcpy(source_buffer_.data(), header_bytes, sizeof(block_header));

		if (kpifile_->Read(source_buffer_.data() + sizeof(block_header), b_header.block_size - 5) < b_header.block_size - 5)
		{
			// EOF?
			break;
		}

		uint32_t input_bytes_consumed, samples_produced;
		SampleHolder holder(header_.num_channels, header_.max_num_samples_per_block);

		if (SRLADecoder_DecodeBlock(decoder_, source_buffer_.data(), b_header.block_size + 6, 
		                    holder.getBufferPtrs(), header_.num_channels, b_header.samples_in_block,
		                &input_bytes_consumed, &samples_produced) != SRLA_APIRESULT_OK)
		{
			break;
		}
		holder.num_of_samples = samples_produced;
		holder.samples_used = 0;

		if (samples_produced > dwSizeSample)
		{
			holder.samples_used = samples_produced = holder.Render(pBuffer, header_.bits_per_sample, 0, dwSizeSample);
			holders_.push_back(holder);
		}
		else // samples_produced <= dwSizeSample
		{
			samples_produced = holder.Render(pBuffer, header_.bits_per_sample, holder.samples_used, holder.num_of_samples);
		}
		dwSizeSample -= samples_produced;
		pBuffer += static_cast<size_t>(samples_produced) * header_.num_channels * (header_.bits_per_sample / 8);
		samples_rendered += samples_produced;
	}
	return samples_rendered;
}


UINT64 KpiSRLADecoder::Seek(UINT64 qwPosSample, DWORD dwFlag)
{
	UINT64 qwSoughtPos = 0;
	block_header b_header;

	if(kpifile_ == nullptr)
	{
		return 0;
	}

	reset();
	while(qwSoughtPos < qwPosSample)
	{
		uint8_t header_bytes[sizeof(block_header)];
		if (kpifile_->Read(header_bytes, sizeof(block_header)) < sizeof(block_header))
		{
			break;
		}
		memcpy(&b_header, header_bytes, sizeof(b_header));
		b_header.prepare();	

		if (b_header.sync_word != 0xffff)
		{
			// broken?
			break;
		}

		if (qwSoughtPos + b_header.samples_in_block > qwPosSample)
		{
			kpifile_->Seek(-static_cast<INT64>(sizeof(block_header)), FILE_CURRENT);
			break;
		}
		if(kpifile_->Seek(b_header.block_size - 5, FILE_CURRENT) == KPI_FILE_EOF)
		{
			break;
		}

		qwSoughtPos += b_header.samples_in_block;
	}

	return qwSoughtPos;
}

DWORD KpiSRLADecoder::UpdateConfig(void* pvReserved)
{
	return 0;
}

// determine if the file is a SRLA file, as fast as possible
DWORD KpiSRLADecoder::IsSRLAFile(IKpiFile* pFile)
{
    uint8_t signature[4];

    pFile->Seek(0, FILE_BEGIN);
    if (pFile->Read(signature, 4) < 4)
    {
        return 0;
    }

    if (memcmp(signature, "1249", 4) != 0)
    {
        ::OutputDebugStringW(L"isSRLA: signature is not 1249\n");
        return 0;
    }

	pFile->Seek(0, FILE_BEGIN);

	return 1;       // # of songs in the SRLA file, currently always 1
}

void KpiSRLADecoder::close()
{
	if (decoder_ != nullptr)
	{
		SRLADecoder_Destroy(decoder_);
		decoder_ = nullptr;
	}

	if(kpifile_ != nullptr)
	{
		kpifile_->Release();
		kpifile_ = nullptr;
	}
}

void KpiSRLADecoder::reset()
{
	kpifile_->Seek(SRLA_HEADER_SIZE, FILE_BEGIN);
    source_buffer_.resize(static_cast<size_t>(header_.max_num_samples_per_block) * header_.num_channels * sizeof(int32_t) + sizeof(block_header));

	holders_.clear();
}
