#include <windows.h>
#include "kpi.h"
#include "kpi_unarc.h"

///////////////////////////////////////////////////////////////////////////////
//�v���O�C�����Ŏg�����߂̃w���p�[�֐�
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI kpi_CreateLocalFile(IKpiUnkProvider *pProvider,
                                 const wchar_t *cszFileNameFullPath,
                                 IKpiFile **ppFile)
{
    *ppFile = NULL;
    return pProvider->CreateInstance(IID_IKpiFile, (void*)&IID_LocalFile, (void*)cszFileNameFullPath, NULL, NULL, (void**)ppFile);
}
DWORD WINAPI kpi_CreateLocalFile(IKpiUnknown *pUnknown, //kpi_CreateInstance �̑�3�������璼�ڎ擾
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
                                  const BYTE *pBuffer,  //�������o�b�t�@�ւ̃|�C���^
                                  size_t stSize,        //�������o�b�t�@�̃T�C�Y
                                  const wchar_t *cszFileName,//GetFileName ���Ԃ��t�@�C����
                                                        //�p�X�̋�؂蕶��(\ �� /)���܂߂Ă��ǂ����A�t�@�C���������݂̂��L��
                                  IKpiFile **ppFile)
{
    *ppFile = NULL;
    return pProvider->CreateInstance(IID_IKpiFile, (void*)&IID_MemoryFile, (void*)pBuffer, (void*)stSize, (void*)cszFileName, (void**)ppFile);
}
DWORD WINAPI kpi_CreateMemoryFile(IKpiUnknown *pUnknown,//kpi_CreateInstance �̑�3�������璼�ڎ擾
                                  const BYTE *pBuffer,  //�������o�b�t�@�ւ̃|�C���^
                                  size_t stSize,        //�������o�b�t�@�̃T�C�Y
                                  const wchar_t *cszFileName,//GetFileName ���Ԃ��t�@�C����
                                                        //�p�X�̋�؂蕶��(\ �� /)���܂߂Ă��ǂ����A�t�@�C���������݂̂��L��
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
//�v���O�C�����G�N�X�|�[�g����֐����g���ۂ̃w���p�[�֐�
//(���ۂɎg���͖̂{�̑�)
///////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI kpi_CreateUnArcModule(pfn_kpiCreateInstance fnCreateInstance,
                                     IKpiUnArcModule **ppModule,
                                     IKpiUnknown *pUnknown)
{
    return fnCreateInstance(IID_IKpiUnArcModule, (void**)ppModule, pUnknown);
}
///////////////////////////////////////////////////////////////////////////////

