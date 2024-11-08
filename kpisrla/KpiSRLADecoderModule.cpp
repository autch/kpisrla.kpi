#include "pch.h"
#include "KpiSRLADecoderModule.h"
#include "KpiSRLADecoder.h"
#include "kpisrla.h"

// {C9CD2FA8-22D0-4F31-B313-375348CE94AE}
static constexpr GUID KPISRLA_GUID = 
{ 0xc9cd2fa8, 0x22d0, 0x4f31, { 0xb3, 0x13, 0x37, 0x53, 0x48, 0xce, 0x94, 0xae } };

static constexpr KPI_DECODER_MODULEINFO KPISRLA_MODULEINFO = {
    .cb = sizeof(KPI_DECODER_MODULEINFO),
    .dwModuleVersion = KPI_DECODER_MODULE_VERSION,
    .dwPluginVersion = KPISRLA_VERSION,
    .dwMultipleInstance = KPI_MULTINST_INFINITE,
    .guid = KPISRLA_GUID,
    .cszDescription = KPISRLA_DESC,
    .cszCopyright = KPISRLA_COPYRIGHT,
    .cszSupportExts = L".srla",
    .cszMultiSongExts = nullptr,
    .pvReserved1 = nullptr,
    .pvReserved2 = nullptr,
    .dwSupportTagInfo = /*KPI_DECODER_MODULEINFO::TAG_EXTRA*/ 0,
    .dwSupportConfig = 0,
    .dwReserved = { 0, 0, 0, 0 }
};

void KpiSRLADecoderModule::GetModuleInfo(const KPI_DECODER_MODULEINFO** ppInfo)
{
	*ppInfo = &KPISRLA_MODULEINFO;
}

DWORD KpiSRLADecoderModule::Open(const KPI_MEDIAINFO* cpRequest, IKpiFile* pFile, IKpiFolder* pFolder, IKpiDecoder** ppDecoder)
{
    auto* decoder = new KpiSRLADecoder();
    const DWORD dwCount = decoder->Open(cpRequest, pFile);
    if (dwCount == 0)
    {
        ::OutputDebugStringW(L"decoder returned no songs");
        delete decoder;
        *ppDecoder = nullptr;
        return 0;
    }
    *ppDecoder = static_cast<IKpiDecoder*>(decoder);
    return dwCount;
}
