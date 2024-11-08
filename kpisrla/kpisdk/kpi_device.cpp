#include <windows.h>
#include "kpi_device.h"

///////////////////////////////////////////////////////////////////////////////
//�v���O�C�����Ŏg�����߂̃w���p�[�֐�
//(���ۂɎg���̂̓v���O�C����)
//���̂Ƃ���Ȃ��̂ŁA�v���O�C�����ł̓v���W�F�N�g�ɒǉ�����K�v�Ȃ�
//kpi_CreateConfig �� kpi.cpp �Ɏ�������Ă���̂ŁA�g���ꍇ�� kpi.cpp ��
//�v���W�F�N�g�ɒǉ����邱��
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//�v���O�C�����G�N�X�|�[�g����֐����g���ۂ̃w���p�[�֐�
//(���ۂɎg���͖̂{�̑�)
///////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI kpi_CreateOutDeviceModule(pfn_kpiCreateInstance fnCreateInstance,
                                         IKpiOutDeviceModule **ppModule,
                                         IKpiUnknown *pUnknown)
{
    return fnCreateInstance(IID_IKpiOutDeviceModule, (void**)ppModule, pUnknown);
}
///////////////////////////////////////////////////////////////////////////////

