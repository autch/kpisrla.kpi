#pragma once
#ifndef kpi_deviceH
#define kpi_deviceH

#include "kpi.h"
#include "kpi_decoder.h" //KPI_MEDIAINFO::FORMAT_PCM/FORMAT_DOP

#pragma pack(push, 1) //�\���̂̃A���C�������g

///////////////////////////////////////////////////////////////////////////////
/*�f�o�C�X�v���O�C��
  DirectSound �� waveOut/ASIO/WASAPI ���̃f�o�C�X�� PCM ���o�́B
  �o�͐���t�@�C���Ƃ���΁AWAVE �t�@�C���o�̓v���O�C����A���`���ւ�
  �R���o�[�^���쐬�\�B(�{�̂�������ƑΉ����Ȃ��Ǝ��p�����Ⴍ�Ȃ肻������)

  �f�o�C�X�v���O�C�����G�N�X�|�[�g����֐�
  typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,    //IKpiXXXModule �� IID
                                                void **ppvObject, //IKpiXXXModule �̖߂�l
                                                IKpiUnknown *pUnknown);
  HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
  riid �� IID_IKpiOutDeviceModule ���n�����B
  �K�v�ł���� pUnknown ���� IKpiConfig �����擾���邱�ƁB
  kpi_CreateConfig ���̃w���p�[�֐����Ăԏꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ�����B

  *ppvObject �ɃC���X�^���X��n���ꍇ�́A�K�؂Ȍ^�ɃL���X�g���邱�ƁB
  �Ⴆ�΁A

    if(IsEqualIID(riid, IID_IKpiUnArcModule){
        CKpiXXXOutDeviceModule *pModule = new CKpiXXXOutDeviceModule;
        // *ppvObject = pModule; //NG
        *ppvObject = (IKpiOutDeviceModule*)pModule; //OK
    }

  �̂悤�ɂ��邱�ƁBCKpiXXXOutDeviceModule �������̃N���X����h�����Ă���ꍇ�A
  �K�؂ȃ|�C���^���n����Ȃ��\��������B
*/
///////////////////////////////////////////////////////////////////////////////
#define KPI_OUTDEVICE_MODULE_VERSION 9 //�d�l�m�莞�� 100 �Ƃ���\��

class __declspec(uuid("{59AFA38B-17A0-4DDC-952A-3AED1901226D}")) IKpiOutDevice;
class __declspec(uuid("{01CCD9F7-A698-4305-B2D1-8E98D0736471}")) IKpiOutDeviceModule;
class __declspec(uuid("{42C2C87A-96FA-4DE6-8E23-A539F33B6DC5}")) IKpiDeviceEnumerator;

#define IID_IKpiOutDevice        __uuidof(IKpiOutDevice)
#define IID_IKpiOutDeviceModule  __uuidof(IKpiOutDeviceModule)
#define IID_IKpiDeviceEnumerator __uuidof(IKpiDeviceEnumerator)

///////////////////////////////////////////////////////////////////////////////
class IKpiOutDevice : public IKpiUnknown
{
public:
    virtual BOOL   WINAPI Output(BYTE *pOutput, int nSizeSample)=0;
    virtual void   WINAPI Wait(void)=0;
    virtual UINT64 WINAPI GetPosition(void)=0;
    virtual void   WINAPI Stop(void)=0;
    virtual void   WINAPI Pause(void)=0;
    virtual void   WINAPI Restart(void)=0;
    virtual BOOL   WINAPI SetVolume(int nVolume)=0;
    virtual BOOL   WINAPI SetRate(int nRate)=0;
    virtual DWORD  WINAPI UpdateConfig(void *pvReserved)=0;
/*
    BOOL WINAPI Output(BYTE *pOutput, int nSizeSample);
      �f�o�C�X�� PCM ���o��

      IKpiOutDeviceModule::Open �ɂ��C���X�^���X�쐬���A������
      nBufferSizeSample �� 2 �{�ȏ�̃o�b�t�@���m�ۂ��Ă������ƁB
      nSizeSample �̓T���v���P�ʂŒʏ�� nBufferSizeSample �Ɠ������B
      nSizeSample < nBufferSizeSample �̏ꍇ�̓f�R�[�h�I���B
      ���̏ꍇ�AnSizeSample �𒴂��镔���̃o�b�t�@�ɂ͖����������Ă���B

      �o�͐�f�o�C�X�� Open ���� cszDeviceName / cszId �ɉ������f�o�C�X
      pOutput �̃o�b�t�@�T�C�Y�� Open ���� nBufferSizeSample * (abs(nBps)/8) * nChannels �ɓ������B
      ���O�� Wait ���Ăяo���� Output ���\�ɂȂ�܂ő҂��Ă���Ă΂�邱�Ƃ�
      �ۏ؂����B

      �������� TRUE ��Ԃ�
      �f�o�C�X�ɉ������̃G���[���������Ď��s�����ꍇ�� FALSE ��Ԃ�
      FALSE ��Ԃ����ꍇ�A�{�̂͒����ɉ��t���~���A�Ȍ�� Output ���Ăяo�����Ƃ͂Ȃ�
    void WINAPI Wait(void);
      ���� Output ���\�ɂȂ�܂ő҂B

      Wait �̌Ăяo���X���b�h�Ƃ͈قȂ�X���b�h���牺�L�̃��\�b�h���Ă΂�邱�Ƃ�����A
      �r�����������Ȃ��B
      GetPosition/Stop/Pause/Restart/SetVolume/SetRate

      Output �͕K�� Wait �̌�ɁAWait �Ƃ͔r���I�ɌĂ΂��(�����X���b�h����Ă΂��Ƃ͌���Ȃ�)

      Output ���\�ɂȂ����瑬�₩�Ƀ��^�[�����邱�ƁB
      �҂��Ă�Ԃ� Stop ���Ă΂ꂽ�ꍇ�͑��₩�Ƀ��^�[�����邱�ƁB���̏ꍇ�A
      ���� Output ���Ă΂ꂽ��Đ����J�n���Ȃ���΂Ȃ�Ȃ��B

      Output �� Wait �̌Ăяo���͂��� 1 �� 1 �ɑΉ����Ă���Ƃ͌���Ȃ��B
      Output �̌Ăяo���O�� 2 ��ȏ� Wait ���Ă΂�邱�Ƃ�����B
      WaitForSingle/MultipleObject(s) �� AutoReset �ȃC�x���g�I�u�W�F�N�g���w�肷��ꍇ�Ȃǂ�
      ���ӂ��邱�ƁB�i2�x�ڂ̌Ăяo���Ńf�b�h���b�N�ɂȂ�Ȃ��悤�Ɂj

      �������AWait �� 1 �x���Ă΂��� Output ���Ă΂�邱�Ƃ͌����ĂȂ��B

    UINT64 WINAPI GetPosition(void);
      �f�o�C�X�̍Đ��ʒu���T���v���P�ʂŕԂ��B
      Stop ���Ă΂ꂽ��Đ��ʒu�� 0 �ɖ߂����ƁB

      Output �� 1 ����Ă΂�Ă��Ȃ��i�K�ł��̃��\�b�h���Ă΂�邱�Ƃ͂Ȃ��B

    void WINAPI Stop(void);
      �Đ����~����BWait �̏������ɌĂ΂ꂽ�ꍇ�AWait �͑��₩�Ƀ��^�[�����Ȃ���΂Ȃ�Ȃ��B
      Stop �̌�� Output ���Ă΂ꂽ�炻�̎��_�ōĐ����ĊJ���邱�ƁB
      Pause ���� Stop ���Ă΂�邱�Ƃ����邱�Ƃɒ��ӁB

    void WINAPI Pause(void);
      �ꎞ��~�BRestart ���Ă΂��܂ōĐ����ꎞ��~����B
      Pause ���� Stop ���Ă΂ꂽ��Đ����~���邱�ƁB
      Pause ���� Output ���Ă΂�邱�Ƃ͂Ȃ��B
      Pause �����d�ɌĂ΂�邱�Ƃ͂Ȃ��B�i�����ŎQ�ƃJ�E���g�𐔂���K�v�͂Ȃ��j
      Pause ���� Stop ���Ă΂�邱�Ƃ����邱�Ƃɒ��ӁB

    void WINAPI Restart(void);
      �ĊJ�BPause �̌�ɂ����Ă΂�Ȃ��B

    BOOL WINAPI SetVolume(int nVolume);
      ���ʐ���B�P�ʂ� %
      �����͖���(���������� 0 <= nVolume <= 100)
      ���Ή��̏ꍇ�� FALSE ��Ԃ��B(�{�̂ɂ���ă\�t�g�E�F�A�I�ɏ��������)

    BOOL WINAPI SetRate(int nRate);//�T���v�����O���[�g��ϊ�
      �T���v�����O���[�g��ϊ��B�P�ʂ� %
      nRate = 200 : 2 �{(200%)�ɂ���
      nRate = 100 : �ύX���Ȃ�
      nRate =  50 : ����(50%) �ɂ���
      ���Ή��̏ꍇ�� FALSE ��Ԃ��B

    DWORD WINAPI UpdateConfig(void *pvReserved);
      �ݒ肪�ύX���ꂽ��{�̂ɂ���ČĂ΂��B
      pvReserved �ɂ� NULL ���n�����B
      0 ��Ԃ����ƁB

    �e�탁�\�b�h�͂�������X���b�h����Ă΂��Ƃ͌���Ȃ����AWait �ȊO��
    ���\�b�h�͖{�̂ɂ���Ĕr���I�ɌĂ΂�邱�Ƃ��ۏ؂����B
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiDeviceEnumerator : public IKpiUnknown
{//�f�o�C�X����񋓂���
 //Enumarate ���J��Ԃ��Ăяo������
public:
    virtual void WINAPI Enumerate(const wchar_t *cszDeviceName, const wchar_t *cszId)=0;
};
///////////////////////////////////////////////////////////////////////////////
struct KPI_DEVICE_FORMAT
{
    DWORD cb;//=sizeof(DEVICE_FORMAT
    DWORD dwFormatType;//KPI_MEDIAFORMAT::FORMAT_PCM or FORMAT_DOP
    DWORD dwSampleRate;   //�Đ����g��
    INT32 nBitsPerSample; //�r�b�g��(���̂Ƃ���float)
    DWORD dwChannels;     //�`���l����
    DWORD dwSpeakerConfig;//���Ή�(���0)
    DWORD dwReserved[4];  //�\��(���0)
};
///////////////////////////////////////////////////////////////////////////////
struct KPI_DEVICE_MODULEINFO
{
    enum{
        SUPPORT_FLAG_VOLUME = 0x01,   //���ʕύX(SetVolume)�ɑΉ�
        SUPPORT_FLAG_RATE = 0x01 << 1,//�Đ����[�g�ϊ�(SetRate)�ɑΉ�
        SUPPORT_FLAG_BITPERFECT = 0x01 << 2,//�r�b�g�p�[�t�F�N�g(WASAPI/ASIO)
        SUPPORT_FLAG_DISKWRITE = 0x01 << 3,//�t�@�C���o��
    };
    DWORD cb;                //=sizeof(KPI_DEVICE_MODULEINFO)
    DWORD dwModuleVersion;   //=KPI_OUTDEVICE_MODULE_VERSION
    DWORD dwPluginVersion;   //�v���O�C���̃o�[�W����
    DWORD dwMultipleInstance;//KPI_MULTINST_INFINITE/ZERO/ONE/UNIQUE
    GUID  guid;              //�v���O�C�����ʎq
    const wchar_t *cszDescription; //�\����
    const wchar_t *cszCopyright;   //���쌠(����������ꍇ�͉��s������)
    const wchar_t *cszTypeName;    //�f�o�C�X�̎�ޖ�(DirectSound/waveOut/WASAPI/ASIO etc...)
    DWORD dwSupportConfig;  //IKpiConfig �ɂ��ݒ�̓ǂݏ����ɖ��Ή��Ȃ� 0
    DWORD dwSupportFlags;   //SUPPORT_FLAG_VOLUME/RATE/BITPERFECT �̑g�ݍ��킹
    DWORD dwReserved[4];    //must be 0
};
///////////////////////////////////////////////////////////////////////////////
class IKpiOutDeviceModule : public IKpiUnknown
{//IKpiOutDevice �̃C���X�^���X�쐬�ƃf�o�C�X���̗񋓂��s���B
public:
    virtual void  WINAPI GetModuleInfo(const KPI_DEVICE_MODULEINFO **ppInfo)=0;
    virtual BOOL  WINAPI Open(const wchar_t *cszDeviceName, //�f�o�C�X��
                              const wchar_t *cszId,         //���ʕ�����
                              const KPI_DEVICE_FORMAT *cpFormat, //�Đ����g����
                              int nBufferSizeSample,        //�Đ��o�b�t�@�T�C�Y�i�T���v���P�ʁj
                              IKpiOutDevice **ppOutDevice)=0;
    virtual void  WINAPI EnumDevices(IKpiDeviceEnumerator *pEnumerator)=0;//�f�o�C�X���̗�
    virtual BOOL  WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator)=0;
    virtual DWORD WINAPI ApplyConfig(const wchar_t *cszSection, //�Z�N�V������
                                     const wchar_t *cszKey,     //�L�[��
                                     UINT64 nValue,             //�ύX��̒l(INT)
                                     double dValue,             //�ύX��̒l(FLOAT)
                                     const wchar_t *cszValue)=0;//�ύX��̒l(STR)
/*
    void  WINAPI GetModuleInfo(const KPI_DECODER_MODULEINFO **ppInfo);
      KPI_DEVICE_MODULEINFO �� *ppInfo �ɐݒ肵�ĕԂ��B
      IKpiDecoderModule::GetModuleInfo(kpi_decoder.h) �Ƃقړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

    void WINAPI Open(const wchar_t *cszDeviceName, //�f�o�C�X��
                     const wchar_t *cszId, //���ʕ�����
                     const KPI_DEVICE_FORMAT *cpFormat, //�Đ����g����
                     int nBufferSizeSample, //�Đ��o�b�t�@�T�C�Y�i�T���v���P�ʁj
                     IKpiOutDevice **ppOutDevice);
      �f�o�C�X���J��
      ���������� IKpiOutDevice �̃C���X�^���X�� *ppOutDevice �ɐݒ肵�� TRUE ��Ԃ��B
      ���s������ *ppOutDevice �� NULL �ɐݒ肵�� FALSE ��Ԃ�
      cszDeviceName ���󕶎���(cszDeviceName = "")�̂Ƃ�(NULL �͓n����Ȃ�)��
      Enumerate ���Ɉ�ԍŏ��Ɍ�����f�o�C�X�ƌ��Ȃ�����

    void WINAPI EnumDevices(IKpiDeviceEnumerator *pEnumerator);//�f�o�C�X���̗�
      �f�o�C�X����񋓂���B
      IKpiDeviceEnumerator::Enumerate ���J��Ԃ��Ăяo���B
      �Ăяo�����͏�ɍŐV����񋓂��邱�ƁB

    BOOL WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator);
      �ݒ荀�ڂ�񋓂���B
      �{�̂��񋓂��ꂽ�ݒ荀�ڂ����ɐݒ��ʂ�������������B
      �ݒ荀�ڂ��Ȃ��ꍇ�� FALSE ��Ԃ��A����ꍇ�� TRUE ��Ԃ��B
      IKpiDecoderModule::EnumConfig(kpi_decoder.h) �Ƃقړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

      KPI_CFG_INFO::cszSection = �f�o�C�X��-���ʎq �ƂȂ��Ă���ꍇ��
      EnumDevices �ŗ񋓂��ꂽ�f�o�C�X�p�̃Z�N�V�����ƌ��Ȃ��B
      (�ݒ��ʂ𐶐����ɕK�v�ɉ����Ď����őI����Ԃɂ���)

    DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//�Z�N�V������
                             const wchar_t *cszKey,    //�L�[��
                             INT64  nValue, //�ύX��̒l(BOOL/INT �̏ꍇ)
                             double dValue, //�ύX��̒l(FLOAT �̏ꍇ)
                             const wchar_t *cszValue)=0;//�ύX��̒l(������)
      cszSection �� cszKey �ɊY������ݒ�̕ύX���u�K�p�v���钼�O�ɌĂ΂��B
      �Ă΂ꂽ���_�ł͂܂��ݒ�͕ύX����Ă��Ȃ��B
      IKpiDecoderModule::ApplyConfig(kpi_decoder.h) �Ƃقړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

    EnumDevices �� Open �͕����̃X���b�h����Ă΂��\�������邪�A�r���I��
    �Ă΂�邱�Ƃ��ۏ؂����BOpen �̏������͓����I�� EnumDevices �ɑ�������
    �������s���\���������Ǝv���邪�A�{�̑��ł͂����̃��\�b�h�͕K���r��
    �I�ɌĂяo���̂� EnumDevices �̔r������͂قڕs�v�Ǝv����B

    IKpiOutDeviceModule �̃C���X�^���X�쐬�Ɣj���̓v���O�C�������[�h����Ă����
    ��x�����s�Ȃ���B

    �K�v�ȏ������̓C���X�^���X�쐬���A��n���̓C���X�^���X�j�����ɍs�����ƁB

    �{�̂̓v���O�C�������[�h�� Open �܂ōs�킸�A�Œ���̏�񂾂��擾��ɃA�����[�h
    ���邱�Ƃ�����B�Ⴆ�΋N�����Ƀv���O�C�������߂ĔF�������ꍇ�� GetModuleInfo
    �����Ăяo����ɃA�����[�h����B�ݒ��ʂ̕\�����A�v���O�C�����g�p���łȂ����

    GetModuleInfo/EnumConfig/ApplyConfig

    �����Ă΂��A�ݒ��ʂ�����Ƃ���ŃA�����[�h����B

    �����v���O�C���̏������Ɏ��Ԃ�������ꍇ�� Open ���ɏ�������1�x�������s����ȂǁA
    �v���O�C�����œƎ��ɍH�v����B���̍ۂ͖��������̂܂� IKpiOutDeviceModule ��j��
    ���ꂽ��A�ǂ̃��\�b�h���ŏ��ɌĂ΂�Ă�����ɖ�肪�N����Ȃ��悤�ɒ��ӂ��邱�ƁB
*/
};
///////////////////////////////////////////////////////////////////////////////
//�v���O�C�����G�N�X�|�[�g����֐�
//typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,      //IKpiXXXModule �� IID
//                                                void **ppvObject, //IKpiXXXModule �̖߂�l
//                                                IKpiUnknown *pUnknown);
//HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
//IKpiDecoderModule �Ɠ���(kpi.h ���Q��)
//riid �� IID_IKpiOutDeviceModule ���n�����
//�K�v�ł���� pUnknown ���� IKpiConfig �����擾���邱��
//kpi_CreateConfig ���̃w���p�[�֐����Ăԏꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ�����
///////////////////////////////////////////////////////////////////////////////
extern HRESULT WINAPI kpi_CreateOutDeviceModule(pfn_kpiCreateInstance fnCreateInstance,
                                                IKpiOutDeviceModule **ppModule,
                                                IKpiUnknown *pUnknown);
//���v���O�C���g�p���p�̃w���p�[�֐�(�v���O�C���쐬�ɂ͕s�v)
//�v���O�C���g�p��(�ʏ�� KbMedia Player)���Ăяo���ꍇ�� kpi_device.cpp ��
//�v���W�F�N�g�ɒǉ�����
///////////////////////////////////////////////////////////////////////////////
#pragma pack(pop) //�\���̂̃A���C�������g
#endif
