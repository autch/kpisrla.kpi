#include <windows.h>
#include "kpi_player.h"

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
HRESULT WINAPI kpi_CreatePlayerModule(pfn_kpiCreateInstance fnCreateInstance,
                                      IKpiPlayerModule **ppModule,
                                      IKpiUnknown *pUnknown)
{
    return fnCreateInstance(IID_IKpiPlayerModule, (void**)ppModule, pUnknown);
}
///////////////////////////////////////////////////////////////////////////////

