#include "pch.h"

#include "kpisdk/kpi.h"
#include "kpisdk/kpi_decoder.h"
#include "KpiSRLADecoderModule.h"

HRESULT WINAPI kpi_CreateInstance(REFIID riid, void** ppvObject, IKpiUnknown* pUnknown)
{
    if (IsEqualIID(riid, IID_IKpiDecoderModule))
    {
        const auto pModule = new KpiSRLADecoderModule();
        *ppvObject = static_cast<IKpiDecoderModule*>(pModule);
        return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}
