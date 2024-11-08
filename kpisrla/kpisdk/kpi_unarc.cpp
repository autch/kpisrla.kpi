#include <windows.h>
#include "kpi.h"
#include "kpi_unarc.h"

///////////////////////////////////////////////////////////////////////////////
//プラグイン側で使うためのヘルパー関数
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI kpi_CreateLocalFile(IKpiUnkProvider *pProvider,
                                 const wchar_t *cszFileNameFullPath,
                                 IKpiFile **ppFile)
{
    *ppFile = NULL;
    return pProvider->CreateInstance(IID_IKpiFile, (void*)&IID_LocalFile, (void*)cszFileNameFullPath, NULL, NULL, (void**)ppFile);
}
DWORD WINAPI kpi_CreateLocalFile(IKpiUnknown *pUnknown, //kpi_CreateInstance の第3引数から直接取得
                                 const wchar_t *cszFileNameFullPath,
                                 IKpiFile **ppFile)
{
    DWORD ret = 0;
    IKpiUnkProvider *pProvider = NULL;
    pUnknown->QueryInterface(IID_IKpiUnkProvider, (void**)&pProvider);
    if(pProvider){
        ret = kpi_CreateLocalFile(pProvider, cszFileNameFullPath, ppFile);
        pProvider->Release();
    }
    else{
        *ppFile = NULL;
    }
    return ret;
}
DWORD WINAPI kpi_CreateMemoryFile(IKpiUnkProvider *pProvider,
                                  const BYTE *pBuffer,  //メモリバッファへのポインタ
                                  size_t stSize,        //メモリバッファのサイズ
                                  const wchar_t *cszFileName,//GetFileName が返すファイル名
                                                        //パスの区切り文字(\ や /)を含めても良いが、ファイル名部分のみが有効
                                  IKpiFile **ppFile)
{
    *ppFile = NULL;
    return pProvider->CreateInstance(IID_IKpiFile, (void*)&IID_MemoryFile, (void*)pBuffer, (void*)stSize, (void*)cszFileName, (void**)ppFile);
}
DWORD WINAPI kpi_CreateMemoryFile(IKpiUnknown *pUnknown,//kpi_CreateInstance の第3引数から直接取得
                                  const BYTE *pBuffer,  //メモリバッファへのポインタ
                                  size_t stSize,        //メモリバッファのサイズ
                                  const wchar_t *cszFileName,//GetFileName が返すファイル名
                                                        //パスの区切り文字(\ や /)を含めても良いが、ファイル名部分のみが有効
                                  IKpiFile **ppFile)
{
    DWORD ret = 0;
    IKpiUnkProvider *pProvider = NULL;
    *ppFile = NULL;
    pUnknown->QueryInterface(IID_IKpiUnkProvider, (void**)&pProvider);
    if(pProvider){
        ret = kpi_CreateMemoryFile(pProvider, pBuffer, stSize, cszFileName, ppFile);
        pProvider->Release();
    }
    return ret;
}
///////////////////////////////////////////////////////////////////////////////
//プラグインがエクスポートする関数を使う際のヘルパー関数
//(実際に使うのは本体側)
///////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI kpi_CreateUnArcModule(pfn_kpiCreateInstance fnCreateInstance,
                                     IKpiUnArcModule **ppModule,
                                     IKpiUnknown *pUnknown)
{
    return fnCreateInstance(IID_IKpiUnArcModule, (void**)ppModule, pUnknown);
}
///////////////////////////////////////////////////////////////////////////////

