#include <windows.h>
#include "kpi.h"
#include "kpi_impl.h"

class KbKpiNullConfig : public KbKpiUnknownImpl<IKpiConfig>
{//常にデフォルト値を返す IKpiConfig 
 //本体からの kpi_CreateConfig が失敗したときにこのクラスのインスタンスを返す
public:
    void   WINAPI SetInt(const wchar_t *cszSection, const wchar_t *cszKey, INT64  nValue){}
    INT64  WINAPI GetInt(const wchar_t *cszSection, const wchar_t *cszKey, INT64  nDefault){return nDefault;}
    void   WINAPI SetFloat(const wchar_t *cszSection, const wchar_t *cszKey, double dValue){}
    double WINAPI GetFloat(const wchar_t *cszSection, const wchar_t *cszKey, double dDefault){return dDefault;}
    void   WINAPI SetStr(const wchar_t *cszSection, const wchar_t *cszKey, const wchar_t *cszValue){}
    DWORD  WINAPI GetStr(const wchar_t *cszSection, const wchar_t *cszKey, wchar_t *pszValue,
                         DWORD dwSize, const wchar_t *cszDefault){
        lstrcpynW(pszValue, cszDefault, dwSize/sizeof(wchar_t));return (lstrlenW(cszDefault)+1)*sizeof(wchar_t);}
    //binary
    void   WINAPI SetBin(const wchar_t *cszSection, const wchar_t *cszKey, const BYTE *pBuffer, DWORD dwSize){};
    DWORD  WINAPI GetBin(const wchar_t *cszSection, const wchar_t *cszKey, BYTE *pBuffer, DWORD dwSize){
        ZeroMemory(pBuffer, dwSize); return 0;}
};
///////////////////////////////////////////////////////////////////////////////
//IKpiUnkProvider のヘルパー関数(実際に使うのはプラグイン側)
//IKpiUnkProvider に初めからないのは、機能を追加する度に
//派生クラスを作らないといけなくなるのを回避するため
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI kpi_CreateConfig(IKpiUnkProvider *pProvider,
                              const GUID *pGUID,
                              DWORD *pdwPlatform,
                              IKpiConfig **ppConfig)
{
    if(pdwPlatform){
    //pProvider が仕様変更等により IID_IKpiConfig に対応しない
    //場合、ppvObj 以外の未知の引数は値が変更されないため、
    //変更が期待される引数に対して不定な値にならないように
    //予め何らかの値を代入しておく
        *pdwPlatform = (DWORD)-1;//未対応の場合は -1 を返す
    }
    *ppConfig = NULL;
    DWORD dwRet =pProvider->CreateInstance(IID_IKpiConfig,
                                     (void*)pGUID,
                                     (void*)pdwPlatform,
                                     NULL, NULL,
                                     (void**)ppConfig);
    if(*ppConfig){
        return dwRet;
    }
    //プラグインの呼び出し側(通常は KbMedia Player 本体)が IKpiConfig に未対応
    //常にデフォルト値を返す IKpiConfig を返す
    *ppConfig = new KbKpiNullConfig;
    if(pdwPlatform){
        *pdwPlatform = (DWORD)-1;
    }
    return 1;
}
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI kpi_CreateConfig(IKpiUnknown *pUnknown, //kpi_CreateInstance の第3引数
                              const GUID *pGUID,
                              DWORD *pdwPlatform,
                              IKpiConfig **ppConfig)
{//IKpiUnknown(kpi_CreateInstance の第3引数)から直接取得
 //呼び出し時は pUnknown に kpi_CreateInstance の第3引数をそのまま渡すこと
    IKpiUnkProvider *pProvider = NULL;
    if(pUnknown->QueryInterface(IID_IKpiUnkProvider, (void**)&pProvider) == S_OK){
        DWORD dwRet = kpi_CreateConfig(pProvider, pGUID, pdwPlatform, ppConfig);
        pProvider->Release();//pProvider が不要なら ppConfig を解放後でなくても解放可能
        return dwRet;
    }
    *ppConfig = NULL;
    return 0;
}
/*
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
*/
