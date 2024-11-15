#pragma once

#include "kpisdk/kpi_decoder.h"
#include "kpisdk/kpi_impl.h"

class KpiSRLADecoderModule: public KbKpiUnknownImpl<IKpiDecoderModule>
{
public:
	void WINAPI GetModuleInfo(const KPI_DECODER_MODULEINFO** ppInfo) override;

	DWORD WINAPI Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile, IKpiFolder* pFolder, IKpiDecoder** ppDecoder) override;
	BOOL WINAPI EnumConfig(IKpiConfigEnumerator* pEnumerator) override
	{
		return FALSE;
	}
	DWORD WINAPI ApplyConfig(const wchar_t* cszSection, const wchar_t* cszKey, INT64 nValue, double dValue, const wchar_t* cszValue) override
	{
		return KPI_CFGRET_OK;
	}
};
