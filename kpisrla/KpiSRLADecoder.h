#pragma once

#include <vector>
#include <list>

#include "kpisdk/kpi_decoder.h"
#include "kpisdk/kpi_impl.h"
#include "srla_decoder.h"
#include "SampleHolder.h"

#pragma pack(push, 1)

struct block_header
{
	uint16_t sync_word;
	uint32_t block_size;
	uint16_t cksum;
	uint8_t  data_type;
	uint16_t samples_in_block;

	void prepare()
	{
		sync_word = _byteswap_ushort(sync_word);
		block_size = _byteswap_ulong(block_size);
		cksum = _byteswap_ushort(cksum);
		samples_in_block = _byteswap_ushort(samples_in_block);
	}
};

static_assert(sizeof(block_header) == 11, "size of block header must be exactly 11bytes");

#pragma pack(pop)

class KpiSRLADecoder: public KbKpiDecoderImpl
{
public:
	KpiSRLADecoder() = default;
	KpiSRLADecoder(const KpiSRLADecoder& other) = delete;
	KpiSRLADecoder(KpiSRLADecoder&& other) noexcept
	{
		*this = std::move(other);
	}
	KpiSRLADecoder& operator=(KpiSRLADecoder&& other) noexcept
	{
		close();
		decoder_ = other.decoder_;
		header_ = other.header_;
		kpifile_ = other.kpifile_;
		mediainfo_ = other.mediainfo_;
		source_buffer_ = std::move(other.source_buffer_);
		holders_ = std::move(other.holders_);

		other.decoder_ = nullptr;
		other.kpifile_ = nullptr;
		return *this;
	}
	~KpiSRLADecoder() override
	{
		close();
	}

	const KpiSRLADecoder& operator=(const KpiSRLADecoder& other) = delete;

	DWORD Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile);

	DWORD Select(DWORD dwNumber, const KPI_MEDIAINFO** ppMediaInfo, IKpiTagInfo* pTagInfo, DWORD dwTagGetFlags) override;
	DWORD Render(BYTE* pBuffer, DWORD dwSizeSample) override;
	UINT64 Seek(UINT64 qwPosSample, DWORD dwFlag) override;
	DWORD UpdateConfig(void* pvReserved) override;

	DWORD IsSRLAFile(IKpiFile* pFile);
private:
	void close();
	void reset();
	void fillMediaInfo(DWORD dwNumber);
	void fillTagInfo(IKpiTagInfo* pTagInfo) const;

	SRLADecoder* decoder_ = nullptr;
	SRLAHeader header_{};

	IKpiFile* kpifile_ = nullptr;
	KPI_MEDIAINFO mediainfo_{};

	std::vector<uint8_t> source_buffer_;
	std::list<SampleHolder> holders_;	// samples_[generations].samples[ch][offset]
};
