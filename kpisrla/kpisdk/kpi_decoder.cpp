#include <windows.h>
#include "kpi_decoder.h"

///////////////////////////////////////////////////////////////////////////////
//プラグイン側で使うためのヘルパー関数
//(実際に使うのはプラグイン側)
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI kpi_CreateDecoder(IKpiUnkProvider *pProvider,
                               const KPI_MEDIAINFO *cpRequest,//再生周波数等の要求値が含まれた KPI_MEDIAINFO
                               IKpiFile     *pFile,   //音楽データ
                               IKpiFolder   *pFolder, //音楽データがあるフォルダ
                               IKpiDecoder **ppDecoder)
{
    return pProvider->CreateInstance(IID_IKpiDecoder,
                                     (void*)cpRequest, pFile, pFolder,
                                     NULL,
                                     (void**)ppDecoder);
}
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI kpi_CreateDecoder(IKpiUnknown *pUnknown,  //kpi_CreateInstance の第3引数から直接取得
                               const KPI_MEDIAINFO *cpRequest,//再生周波数等の要求値が含まれた KPI_MEDIAINFO
                               IKpiFile     *pFile,    //音楽データ
                               IKpiFolder   *pFolder,  //音楽データがあるフォルダ
                               IKpiDecoder **ppDecoder)
{//IKpiUnknown(kpi_CreateInstance の第3引数)から直接取得
 //呼び出し時は pUnknown に kpi_CreateInstance の第3引数をそのまま渡すこと
    IKpiUnkProvider *pProvider = NULL;
    if(pUnknown->QueryInterface(IID_IKpiUnkProvider, (void**)&pProvider) == S_OK){
        DWORD dwRet = kpi_CreateDecoder(pProvider,
                                        cpRequest, pFile, pFolder, ppDecoder);
        pProvider->Release();//pProvider が不要なら ppDecoder を解放後でなくても解放可能
        return dwRet;
    }
    *ppDecoder = NULL;
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
//プラグインがエクスポートする関数を使う際のヘルパー関数
//(実際に使うのは本体側)
///////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI kpi_CreateDecoderModule(pfn_kpiCreateInstance fnCreateInstance,
                                       IKpiDecoderModule **ppModule,
                                       IKpiUnknown *pUnknown)
{
    return fnCreateInstance(IID_IKpiDecoderModule, (void**)ppModule, pUnknown);
}
///////////////////////////////////////////////////////////////////////////////

