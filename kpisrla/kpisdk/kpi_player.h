#pragma once
#ifndef kpi_playerH
#define kpi_playerH

#include "kpi.h"
#include "kpi_decoder.h"
#pragma pack(push, 1) //�\���̂̃A���C�������g

///////////////////////////////////////////////////////////////////////////////
/*���t�v���O�C��
  �f�R�[�_�v���O�C���̂悤�ɍׂ��ȉ��t����͏o���Ȃ����A������������Ƃ��o����B
  ����łȂ��Ă��A���C�u�����̎d�l�� PCM ���擾�o�����ɉ��t���䂵���o���Ȃ�
  �悤�Ȍ`���ɂ��Ή��\�B

  ����̃E�B���h�E��Đ����x�̕ύX���̓v���O�C�����Ő���B
  Kobarin �̋Z�p�͕s���̂��߁AIKpiDecoder �̉f���Ή��ł݂����Ȃ��͖̂��Ή��B

  ���t�v���O�C�����G�N�X�|�[�g����֐�
  typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,    //IKpiXXXModule �� IID
                                                void **ppvObject, //IKpiXXXModule �̖߂�l
                                                IKpiUnknown *pUnknown);
  HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
  riid �� IID_IKpiPlayerModule ���n�����B
  �K�v�ł���� pUnknown ���� IKpiConfig �����擾���邱�ƁB
  kpi_CreateConfig ���̃w���p�[�֐����Ăԏꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ�����B

  *ppvObject �ɃC���X�^���X��n���ꍇ�́A�K�؂Ȍ^�ɃL���X�g���邱�ƁB
  �Ⴆ�΁A

    if(IsEqualIID(riid, IID_IKpiUnArcModule){
        CKpiXXXPlayerModule *pModule = new CKpiXXXPlayerModule;
        // *ppvObject = pModule; //NG
        *ppvObject = (IKpiPlayerModule*)pModule; //OK
    }

  �̂悤�ɂ��邱�ƁBCKpiXXXPlayerModule �������̃N���X����h�����Ă���ꍇ�A
  �K�؂ȃ|�C���^���n����Ȃ��\��������B
*/
///////////////////////////////////////////////////////////////////////////////
#define KPI_PLAYER_MODULE_VERSION 3 //�d�l�m�莞�� 100 �Ƃ���\��

class __declspec(uuid("{CD083F95-E9D2-4C8A-9FF0-A4003AB220F8}")) IKpiPlayer;
class __declspec(uuid("{20706B19-8A25-4DF6-890E-FF76AE7386C1}")) IKpiPlayerModule;

#define IID_IKpiPlayer       __uuidof(IKpiPlayer)
#define IID_IKpiPlayerModule __uuidof(IKpiPlayerModule)

///////////////////////////////////////////////////////////////////////////////
typedef KPI_DECODER_MODULEINFO KPI_PLAYER_MODULEINFO;
//KPI_PLAYER_MODULEINFO::dwModuleVersion == KPI_PLAYER_MODULE_VERSION
//�Ƃ��Ȃ���΂Ȃ�Ȃ����Ƃ������� KPI_DECODER_MODULEINFO �Ɠ���
//
enum{//IKpiPlayer::GetStatus �̖߂�l
    KPI_STATUS_STOP = 0,    //��~��
    KPI_STATUS_PLAYING = 1, //���t��
    KPI_STATUS_PAUSE =   2, //�ꎞ��~��
};
///////////////////////////////////////////////////////////////////////////////
class IKpiPlayer : public IKpiUnknown
{
public:
    virtual DWORD WINAPI Select(DWORD dwNumber,
                                const KPI_MEDIAINFO **ppInfo,
                                IKpiTagInfo *pTagInfo,
                                DWORD dwTagGetFlags)=0;
    virtual BOOL  WINAPI Play(DWORD dwStartPos, BOOL bVisibile)=0;//dwStartPos �̈ʒu���牉�t�J�n
    virtual BOOL  WINAPI Pause(void)=0;  //�ꎞ��~
    virtual BOOL  WINAPI Restart(void)=0;//�ĊJ
    virtual void  WINAPI Stop(void)=0;   //���t��~
    virtual DWORD WINAPI GetPosition(void)=0;//�Đ��ʒu�擾(�P�ʂ̓~���b)
    virtual DWORD WINAPI GetStatus(void)=0;  //���t���(KPI_STATUS_STOP/PLAYING/PAUSE)
    virtual void  WINAPI SetTempo(int nTempo)=0;
    virtual void  WINAPI SetVolume(int nVolume)=0;
    virtual void  WINAPI SetVisible(BOOL bVisible)=0;
    virtual void  WINAPI GetWindowPosition(int *pLeft, int *pTop,
                                           int *pVideoWidth, int *pVideoHeight)=0;
    virtual void  WINAPI SetWindowPosition(int nLeft, int nTop,
                                           int nVideoWidth, int nVideoHeight)=0;
    virtual void  WINAPI SetTopMost(BOOL bTopMost)=0;
    virtual void  WINAPI SetRepeat(BOOL bRepeat)=0;
    virtual DWORD WINAPI UpdateConfig(void *pvReserved)=0; //�ݒ肪�ύX���ꂽ��Ă΂��
/*
    ���t�v���O�C���̏ڍׂȐ����͏ȗ��B
    ���\�b�h�̖��̂ł�����x�z�������͂��B

    SetWindowPosition �ɂ��E�B���h�E���f�X�N�g�b�v��ʊO�ɂ͂ݏo�����Ƃ���
    �������v���O�C�����ōs��(�{�̂ł�������x�΍􂵂Ă��邪)
    �}���`���j�^�����l���ɓ���邱�ƁB

    ���t�v���O�C���͌����ɍl����ƈӊO�Ɠ���c�B

    Play() �Ăяo������� GetStatus �̌��ʂ����C�u��������Ŏ��ۂ͂܂� STOP
    �������肷��ȂǁB���������ꍇ�� Play() �Ăяo������1�`2�b���x�͖�������
    PLAYING �ƌ��Ȃ��Ȃǂ��đΏ�����B(�{�̑��ł������߂�����悤�ɂ��Ă���̂�
    ���ۂ͕s�v)

    ���ۂɉ��t���n�܂�̂� PostMessage �ɂ�郁�b�Z�[�W���󂯎�����ゾ������
    ����ƁA

    Play();
    Stop();

    �ŉ��t���~�܂�Ȃ��ꍇ���������肷��B�̂� RealMedia �֌W�������Ȃ��Ă���
    �ƂĂ������ɂ��������B(�ŋ߂͂ǂ������m��Ȃ���)

    �e�탁�\�b�h�͂�������X���b�h����Ă΂��Ƃ͌���Ȃ����A�S�Ẵ��\�b�h
    �͖{�̂ɂ���Ĕr���I�ɌĂ΂�邱�Ƃ��ۏ؂����B
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiPlayerModule : public IKpiUnknown
{
public:
    virtual void  WINAPI GetModuleInfo(const KPI_PLAYER_MODULEINFO **ppInfo)=0;
    virtual DWORD WINAPI Open(const KPI_MEDIAINFO *cpRequest,
                              IKpiFile    *pFile,
                              IKpiFolder  *pFolder,
                              IKpiPlayer **ppPlayer)=0;
    virtual BOOL  WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator)=0;
    virtual DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//�Z�N�V������
                                     const wchar_t *cszKey,    //�L�[��
                                     INT64  nValue, //�ύX��̒l(BOOL/INT �̏ꍇ)
                                     double dValue, //�ύX��̒l(FLOAT �̏ꍇ)
                                     const wchar_t *cszValue)=0;//�ύX��̒l(������)
/*
    void  WINAPI GetModuleInfo(const KPI_PLAYER_MODULEINFO **ppInfo)=0;
      KPI_PLAYER_MODULEINFO �� *ppInfo �ɐݒ肵�ĕԂ��B
      IKpiDecoderModule::GetModuleInfo(kpi_decoder.h) �Ƃقړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

    DWORD WINAPI Open(const KPI_MEDIAINFO *cpRequest,//�Đ����g�����̗v���l���܂܂ꂽ KPI_MEDIAINFO
                              IKpiFile    *pFile,    //���y�f�[�^
                              IKpiFolder  *pFolder,  //���y�f�[�^������t�H���_
                              IKpiPlayer **ppPlayer);
      pFile/pFolder ����Ȃ��J���B
      ���������� IKpiPlayer �̃C���X�^���X�� *ppPlayer �ɐݒ肵�A�܂܂�Ă���
      �Ȃ̐���Ԃ��B(�G���[�̏ꍇ�� *ppPlayer �� NULL ��ݒ肵�� 0 ��Ԃ�)

      IKpiDecoderModule::Open(kpi_decoder.h)�ƍl�����͂قړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

    BOOL WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator);
      �ݒ荀�ڂ�񋓂���B
      �{�̂��񋓂��ꂽ�ݒ荀�ڂ����ɐݒ��ʂ�������������B
      �ݒ荀�ڂ��Ȃ��ꍇ�� FALSE ��Ԃ��A����ꍇ�� TRUE ��Ԃ��B
      IKpiDecoderModule::EnumConfig(kpi_decoder.h) �Ƃقړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

    DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//�Z�N�V������
                             const wchar_t *cszKey,    //�L�[��
                             INT64  nValue, //�ύX��̒l(BOOL/INT �̏ꍇ)
                             double dValue, //�ύX��̒l(FLOAT �̏ꍇ)
                             const wchar_t *cszValue)=0;//�ύX��̒l(������)
      cszSection �� cszKey �ɊY������ݒ�̕ύX���u�K�p�v���钼�O�ɌĂ΂��B
      �Ă΂ꂽ���_�ł͂܂��ݒ�͕ύX����Ă��Ȃ��B
      IKpiDecoderModule::ApplyConfig(kpi_decoder.h) �Ƃقړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

    IKpiPlayerModule �̃C���X�^���X�쐬�Ɣj���̓v���O�C�������[�h����Ă����
    ��x�����s�Ȃ���B

    �K�v�ȏ������̓C���X�^���X�쐬���A��n���̓C���X�^���X�j�����ɍs�����ƁB

    �{�̂̓v���O�C�������[�h�� Open �܂ōs�킸�A�Œ���̏�񂾂��擾��ɃA�����[�h
    ���邱�Ƃ�����B�Ⴆ�΋N�����Ƀv���O�C�������߂ĔF�������ꍇ�� GetModuleInfo
    �����Ăяo����ɃA�����[�h����B�ݒ��ʂ̕\�����A�v���O�C�����g�p���łȂ����

    GetModuleInfo/EnumConfig/ApplyConfig

    �����Ă΂��A�ݒ��ʂ�����Ƃ���ŃA�����[�h����B

    �����v���O�C���̏������Ɏ��Ԃ�������ꍇ�� Open ���ɏ�������1�x�������s����ȂǁA
    �v���O�C�����œƎ��ɍH�v���邱�ƁB���̍ۂ͖��������̂܂� IKpiPlayerModule
    ��j�����ꂽ��A�ǂ̃��\�b�h���ŏ��ɌĂ΂�Ă�����ɖ�肪�N����Ȃ��悤��
    ���ӂ��邱�ƁB

*/
};
///////////////////////////////////////////////////////////////////////////////
extern HRESULT WINAPI kpi_CreatePlayerModule(pfn_kpiCreateInstance fnCreateInstance,
                                             IKpiPlayerModule **ppModule, IKpiUnknown *pUnknown);
//���v���O�C���g�p���p�̃w���p�[�֐�(�v���O�C���쐬�ɂ͕s�v)
//�v���O�C���g�p��(�ʏ�� KbMedia Player)���Ăяo���ꍇ�� kpi_player.cpp ��
//�v���W�F�N�g�ɒǉ�����
///////////////////////////////////////////////////////////////////////////////
#pragma pack(pop) //�\���̂̃A���C�������g
#endif

