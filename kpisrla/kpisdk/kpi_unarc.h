#pragma once
#ifndef kpi_unarcH
#define kpi_unarcH

#include "kpi.h"
#pragma pack(push, 1) //�\���̂̃A���C�������g

///////////////////////////////////////////////////////////////////////////////
/*�A�[�J�C�u�v���O�C��
  KbMedia Player �ň��k�t�@�C�����̋Ȃ��Đ��o����悤�ɂ���B

  ��������ł́A���t�@�C�����e���|�����ɍ쐬�����A���k�t�@�C����W�J���Ȃ���
  �Đ����邱�Ƃ��o����B(�f�R�[�_/���t�v���O�C���� IKpiFile �ɂ�����ƑΉ�����
  ����K�v������)

  �����܂ł����y�Đ��̂��߂̃v���O�C���ł����āA�A�[�J�C�o���痘�p��������
  �ł͂Ȃ��̂ŁA�擾�ł����񓙂͍ŏ����ŗǂ��͂��B
  ���y�Đ��Ɍ����Ȃ�����(�p�X���[�h�̓��͂��K�v�Ƃ�)�ɖ������Ă܂őΉ�����
  �d�g�݂��l����K�v�͂Ȃ��A�Ǝv���B

  �A�[�J�C�u�v���O�C�������͓̂���Ǝv����B

  �A�[�J�C�u�v���O�C�����G�N�X�|�[�g����֐�
  typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,    //IKpiXXXModule �� IID
                                                void **ppvObject, //IKpiXXXModule �̖߂�l
                                                IKpiUnknown *pUnknown);
  HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
  riid �� IID_IKpiUnArcModule ���n�����B
  �K�v�ł���� pUnknown ���� IKpiConfig �����擾���邱�ƁB
  kpi_CreateConfig ���̃w���p�[�֐����Ăԏꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ�����B

  *ppvObject �ɃC���X�^���X��n���ꍇ�́A�K�؂Ȍ^�ɃL���X�g���邱�ƁB
  �Ⴆ�΁A

    if(IsEqualIID(riid, IID_IKpiUnArcModule){
        CKpiXXXUnArcModule *pModule = new CKpiXXXUnArcModule;
        // *ppvObject = pModule; //NG
        *ppvObject = (IKpiUnArcModule*)pModule; //OK
    }

  �̂悤�ɂ��邱�ƁBCKpiXXXUnArcModule �������̃N���X����h�����Ă���ꍇ�A
  �K�؂ȃ|�C���^���n����Ȃ��\��������B
*/
///////////////////////////////////////////////////////////////////////////////
#define KPI_UNARC_MODULE_VERSION 2 //�d�l�m�莞�� 100 �Ƃ���\��

class __declspec(uuid("{5496FFEA-0644-4B60-910F-6FFB1A25E298}")) IKpiUnArc;
class __declspec(uuid("{E005FFCC-C053-4E12-8275-77920EFA14F7}")) IKpiUnArcModule;
#define IID_IKpiUnArc       __uuidof(IKpiUnArc)
#define IID_IKpiUnArcModule __uuidof(IKpiUnArcModule)

//kpi_CreateLocalFile/kpi_CreateMemoryFile �p
//(IKpiLocalFile_/IKpiMemoryFile_ �Ƃ����N���X�͎��ۂɂ͑��݂��Ȃ�)
class __declspec(uuid("{53247E3A-3539-4707-A1F5-B052ADBCA3E4}")) IKpiLocalFile_;
class __declspec(uuid("{FDAF31A3-7AEA-4466-A190-16073C3CA2FF}")) IKpiMemoryFile_;
#define IID_LocalFile  __uuidof(IKpiLocalFile_)
#define IID_MemoryFile __uuidof(IKpiMemoryFile_)

///////////////////////////////////////////////////////////////////////////////
struct KPI_UNARC_MODULEINFO
{
    DWORD cb;             //=sizeof(KPI_UNARC_MODULEINFO)
    DWORD dwModuleVersion;//=KPI_UNARC_MODULE_VERSION
    DWORD dwPluginVersion;//�v���O�C���̃o�[�W����
    DWORD dwMultipleInstance;//KPI_MULTINST_INFINITE/ZERO/ONE/UNIQUE
    GUID  guid;           //�v���O�C�����ʎq
    const wchar_t *cszDescription; //�\����
    const wchar_t *cszCopyright;   //���쌠(����������ꍇ�͉��s������)
    const wchar_t *cszSupportExts; //�Ή��g���q('/' �ŋ�؂�)(��F".lzh/.zip", ".aaa/.bbb")(�g���q���܂߂�)
    const wchar_t *cszSolidExts;   //�Ή��g���q�̂����A�\���b�h�`���̉\�����������(�J���Ă݂Ȃ��ƕ�����Ȃ����̂��܂�)
    DWORD dwSupportConfig;         //IKpiConfig �ɂ��ݒ�̓ǂݏ����ɖ��Ή��Ȃ� 0
    DWORD dwReserved[4];           //must be 0
};
///////////////////////////////////////////////////////////////////////////////
struct KPI_ARCHIVEINFO
{//�A�[�J�C�u���̂̏��
    DWORD       cb;            //=sizeof(KPI_ARCHIVEINFO)
    DWORD       dwType;        //=0
    DWORD       dwCount;       //�i�[�t�@�C����(�񋓂��I���܂Ő����킩��Ȃ��ꍇ�� -1)
    DWORD       dwSolid;       //�\���b�h���ɂ��܂ނȂ� 1�A�����łȂ��Ȃ� 0
                               //�񋓂��I���܂ł킩��Ȃ����܂މ\��������Ȃ� -1
    DWORD       dwReserved[12];//must be 0
};
///////////////////////////////////////////////////////////////////////////////
struct KPI_FILEINFO
{//�A�[�J�C�u���i�[�t�@�C���̏��
    DWORD       cb;          //=sizeof(KPI_FILEINFO)
    DWORD       dwType;      //=0
    const wchar_t *cszFileName;//�i�[�t�@�C����(�p�X���̋�؂�� \ �ł͂Ȃ� / ���g�p���Ȃ���΂Ȃ�Ȃ�)
                               //�f�B���N�g���̏ꍇ�̖����� / �łȂ���΂Ȃ�Ȃ�
                               //������R�s�[�̎��Ԃ����������Ȃ��������̐������Ȃ��������̂ŕ�����ւ�
                               //�|�C���^�𒼐ړ����B������ IKpiUnArc �� Release �����܂ŁB
    const wchar_t *cszMethod;//���k�@(���O���Ȃ��ꍇ�� NULL �܂��� "")
                             //������R�s�[�̎��Ԃ����������Ȃ��������̐������Ȃ��������̂ŕ�����ւ�
                             //�|�C���^�𒼐ړ����B������ IKpiUnArc �� Release �����܂ŁB
    BOOL        bCrypted;    //�Í�������Ă�� 0 �ȊO�A�Í�������ĂȂ���� 0
    BOOL        bStored;     //�����k�Ȃ� 0 �ȊO�A���k����Ă�� 0
    UINT64      qwOriginalSize;  //�𓀌�̃T�C�Y
    UINT64      qwCompressedSize;//���k��̃T�C�Y(�킩��Ȃ��ꍇ�� 0)
    FILETIME    ft;              //�X�V����(�킩��Ȃ��ꍇ�� 0)
    DWORD       dwReserved[4];   //must be 0
};
///////////////////////////////////////////////////////////////////////////////
class IKpiUnArc : public IKpiUnknown
{
public:
    virtual BOOL  WINAPI GetArchiveInfo(const KPI_ARCHIVEINFO **ppArchiveInfo)=0;
    virtual BOOL  WINAPI EnumFileInfo(DWORD dwIndex, const KPI_FILEINFO **ppFileInfo)=0;
    virtual BOOL  WINAPI Extract(DWORD dwIndex, IKpiFile **ppFile, UINT64 qwSize)=0;
    virtual DWORD WINAPI UpdateConfig(void *pvReserved)=0; //�ݒ肪�ύX���ꂽ��Ă΂��
/*
    BOOL WINAPI GetArchiveInfo(const KPI_ARCHIVEINFO **ppArchiveInfo);
      �J�����A�[�J�C�u���g�̏���Ԃ��B
      *ppArchiveInfo �� IKpiUnArc �̃C���X�^���X�� Release �����܂ŗL���B
    BOOL WINAPI EnumFileInfo(DWORD dwIndex, const KPI_FILEINFO **ppFileInfo);
      �i�[�t�@�C����񋓂���B
      *ppFileInfo �� dwIndex �ɑΉ�����i�[�t�@�C���̏���Ԃ��B
      ���������ꍇ�� 0 �ȊO��Ԃ��B
      ���s�����ꍇ�� 0 ��Ԃ��ė񋓏I���B(����ȏ�� dwIndex ��n���Ă����s)
      ppFileInfo �ɕԂ����\���̂ւ̃|�C���^�� IKpiUnArc ��
      �C���X�^���X�� Release �����܂ŗL���B
      �\���̃R�s�[�̎��Ԃ����������Ȃ�(�n�R��)
    BOOL WINAPI Extract(DWORD dwIndex, IKpiFile **ppFile, UINT64 qwSize);
      dwIndex �ɑΉ�����i�[�I�u�W�F�N�g���𓀂���B
      qwSize �͉𓀂������T�C�Y�B(UINT64)-1 �̂Ƃ��͑S���𓀂���B
      �K�v�ȕ��������𓀂������ꍇ�Ɏg����B
    DWORD WINAPI UpdateConfig(void *pvReserved);
      IKpiUnArcModule::ApplyConfig �̌Ăяo���� kPI_CFGRET_NOTIFY ���Ԃ��ꂽ
      �ꍇ�̂݁A�{�̂ɂ���� IKpiUnArc �̃C���X�^���X���ƂɌĂ΂��
      �Ă΂ꂽ���_�Őݒ�͊��ɕύX�ς݂ł���A�ǂ̐ݒ肪�ύX���ꂽ����m�邱��
      �͏o���Ȃ�
      pvReserved �͏����̗\��(NULL ���n�����)
      �߂�l�̈Ӗ��͏����̗\�� => 0 ��Ԃ�����
      IkpiDecoder �ɍ��킹�� IKpiUnArc �ɂ��ǉ��������A�����Őݒ�ύX��K�p
      �����闘�_�͂��܂�Ȃ��Ǝv����

    Extract �ɂ��𓀌��ʂ� *ppFile �Ɋւ��Ă̒��ӓ_�B(IKpiFile �����킹�ĎQ��)
      �E�W�J��̃f�[�^�����t�@�C���̏ꍇ�A
        �E�w���p�[�֐� kpi_CreateLocalFile ���Ăяo�����ƂŃA�[�J�C�u�v���O�C�����ł̎������ȗ��\
        �E���t�@�C���� IKpiUnArc �� Release �����܂ł͑��݂��Ă��Ȃ���΂Ȃ�Ȃ�
        �E���t�@�C���̍폜�͕K�v�ɉ����ăv���O�C�����g�p���鑤���K�v�ɉ����čs��(cszOutputPath �ȉ��ɍ쐬�����ꍇ)
      �E�W�J��̃f�[�^���������o�b�t�@�̏ꍇ�A
        �E�w���p�[�֐� kpi_CreateMemoryFile ���Ăяo�����ƂŃA�[�J�C�u�v���O�C�����ł̎������ȗ��\
        �Ekpi_CreateMemoryFile ���g�p����ꍇ�A�������o�b�t�@�� IKpiUnArc �� Release �����܂ŗL���łȂ���΂Ȃ�Ȃ�
      �E�W�J��̃f�[�^�����t�@�C���ł��������o�b�t�@�ł��Ȃ��ꍇ�� IKpiFile ���������Ȃ���΂Ȃ�Ȃ�
      �ECreateClone �͎����s�v�B�{�̂� CreateClone ���ĂԂ��Ƃ͂Ȃ��B
      �ESeek �̖߂�l�́A�v����ʒu����O�ł���΁A�v����ʒu�Ɗ��S�ɓ����łȂ��Ă�
        �ǂ��B�������A�v����ʒu�Ɠ�������O�łȂ���΂Ȃ�Ȃ��B�Ⴆ��
        �E�v����ʒu�� 1024 �̏ꍇ�� 512 �̈ʒu�ɃV�[�N���Ă��ǂ��B
        �E�v����ʒu�� 1024 �̏ꍇ�� 2048 �̈ʒu�ɃV�[�N���Ă͂����Ȃ��B
      �ESeek �Ɏ��s����(=KPI_FILE_EOF ��Ԃ���)��̌��݈ʒu�₻�̌�� Read �̌��ʂ͖���`�B
      �ESeek �Ɏ��s������AdwOrigin==FILE_BEGIN or dwOrigin==FILE_END �ł� Seek �ɂ͏o����
        ���萬�����Ȃ���΂Ȃ�Ȃ��B���� Seek(0, FILE_BEGIN) �ɂ͗]���̂��Ƃ��Ȃ����萬��
        ���Ȃ���΂Ȃ�Ȃ��B
      �EAbort �ɂ͑Ή����Ȃ��Ă��ǂ��B(�Ή����Ȃ��ꍇ�� FALSE ��Ԃ�����)
      �E������ Extract �����ꍇ�A�O��Ăяo�����ɕԂ��� IKpiFile �̃C���X�^���X��
        �܂��c���Ă��Ă����퓮�삵�Ȃ���΂Ȃ炸�A�O��Ăяo�����ƍ���Ăяo����
        �̃C���X�^���X�𓯎��Ɏg�p���Ă�������(�Ɨ�����)���삵�Ȃ���΂Ȃ�Ȃ��B
        �E���삳����������΁AGetRealFile/GetBuffer ���Ԃ��|�C���^�͋��L���ėǂ��B
      �EGetFileName �̓f�B���N�g���p�X�����������̂�Ԃ����ƁB
        (aaa/bbb/ccc.xxx �Ȃ� ccc.xxx ��Ԃ�)
      �E�p�t�H�[�}���X��̃����b�g���Ȃ��Ȃ� GetRealFileW �͎��s���Ă��ǂ��B
      �E�p�t�H�[�}���X��̃����b�g���Ȃ��Ȃ� GetBuffer �͎��s���Ă��ǂ��B
      �EGetRealFileA �͎����s�v�B��������Ȃ� GetRealFileW �̕��ɂ��邱�ƁB
        �{�̂� GetRealFileA ���ĂԂ��Ƃ͂Ȃ��B
      �EGetRealFileW ����������ꍇ�A�ꎞ�t�@�C�����̊g���q�� GetFileName ���Ԃ��̂�
        �����łȂ���΂Ȃ�Ȃ��B
        �Ⴆ�� foo.lzh>xxx/aaa.txt �ɑ΂���ꎞ�t�@�C������
            C:\TEMP\xxxx\aaa.doc �ƂȂ��Ă͂����Ȃ��B
            C:\TEMP\xxxx\bbb.txt �ƂȂ�͔̂񐄏�����OK(�t�@�C���������͕ς���ėǂ�)
      �EGetRealFileW ����������ꍇ�A�ꎞ�t�@�C���̃f�B���N�g���K�w������������
        �t�@�C������ GetFileName ���Ԃ��̂Ɠ����ł��邱�Ƃ𐄏��B
       (�o����΃f�B���N�g���K�w�����̖��O�������ł��邱�Ƃ𐄏�)
        �Ⴆ�� foo.lzh>xxx/aaa.txt �ɑ΂���ꎞ�t�@�C������
            C:\TEMP\xxxx\yyy\aaa.txt �ƂȂ���́A
            C:\TEMP\xxxx\xxx\aaa.txt �ƂȂ邱�Ƃ𐄏��B
      �EGetRealFileW ����������ꍇ�A�ꎞ�t�@�C���o�͐�� Open �̑�3����
        cszOutputPath �ȉ��ł��邱�Ƃ����������B
      �EGetRealFileW ����������ꍇ�A����A�[�J�C�u���̑��̊i�[�t�@�C����
        GetRealFileW ���������A�f�B���N�g���K�w�\������������Ă��邱�Ƃ𐄏��B
      �EGetRealFileW/GetBuffer �̌Ăяo���̑O��Ńt�@�C���|�C���^���ς���Ă͂����Ȃ��B
        (Read/Seek �̌��ʂɉe�����Ă͂����Ȃ�)
      �E���� IKpiFile �̃C���X�^���X�ɑ΂��ĕ����̃X���b�h���瓯����
        �EGetRealFileW/GetBuffer/Read/Seek/GetSize ���Ă΂��\���ɑ΂��Ĕz������K�v�͂Ȃ��B
        �EIKpiFile ���g�����Ŕz������
        �E�قȂ� IKpiFile �̃C���X�^���X�ɑ΂��Ă͔z�����Ȃ���΂Ȃ�Ȃ��B
      �EIKpiUnArc �̎g�p��(KbMedia Player �{�̂܂��̓A�[�J�C�u�v���O�C�����g�p����A�v���P�[�V����)�́A
        Extract �ŕԂ��� *ppFile �� Release ����܂� IKpiUnArc �� Release ���Ă͂����Ȃ��B
      �E�A�[�J�C�u�v���O�C���̍쐬�҂� IKpiUnArc �� Release �����Ƃ��A���̃C���X�^���X���쐬����
        IKpiFile �͊��� Release ����Ă��邱�Ƃ�z�肵�ėǂ��B
      �EGetRealFileW �ō쐬�������t�@�C���̍폜�̓v���O�C�����g�p���鑤���s���B
        (IKpiUnArcModule::Open �� cszOutputPath �ȉ��ɍ쐬�����ꍇ)
        cszOutputPath �ȉ��ł͂Ȃ��t�H���_�ɍ쐬�����ꍇ(��������Ȃ�)�� IKpiFile ��
        IKpiUnArc �� Release ���Ƀv���O�C�����g�ō폜���Ȃ���΂Ȃ�Ȃ��B

    EnumFileInfo �ɂ��񋓂Ɋւ��钍�ӓ_�B
      �E��łȂ��f�B���N�g���́A�񋓂��Ȃ��Ă��ǂ��B
        �E�Ⴆ�� aaa/bbb.txt ���񋓂����ꍇ�Aaaa/ ��񋓂���K�v�͂Ȃ��B
        �E�񋓂��Ă��\��Ȃ��B
      �E�i�[�t�@�C�����̏d����(�啶���Ə���������ʂ��邱�Ƃɂ��d���Ȃ�)��
        �o���邾���N����Ȃ��悤�ɂ��邱�ƁB
        �E�{�̂��d���`�F�b�N������̂ŕK�{�ł͂Ȃ��B
      �EWindows �Ŏg�p�o���Ȃ�����(:*?"<>|)���g�p�����i�[�t�@�C�����́A�o���邾��
        �񋓂��Ȃ��悤�ɂ��邱�ƁB
        �E�{�̂��`�F�b�N����̂ŕK�{�ł͂Ȃ��B
      �EDirectory Traversal �����N�����悤�Ȋi�[�t�@�C�����́A�o���邾����
        ���Ȃ��悤�ɂ��邱�ƁB
        �E�{�̂��`�F�b�N����̂ŕK�{�ł͂Ȃ��B
      �EEnumFileInfo �ŕԂ��� KPI_FILEINFO �̎����� IKpiUnArc �� Release �����
        �܂łł��邱�Ƃɏ\���ɋC���������ƁB
        �E�Ⴆ�Γ����� KPI_FILEINFO �̔z��������Ă��āAEnumFileInfo �̌Ăяo��
          �ɉ����� realloc �Ŕz����g�傷��A�Ƃ��������Ƃ�����ƍČ����̒Ⴂ
          �o�O�������N���������ƂȂ�B
      �EKbMedia Player �{�̂́A�K�v�ɂȂ�܂� EnumFileInfo ���Ă΂Ȃ��B
        �񋓂��n�߂Ă��A�I���܂ň�C�ɗ񋓂���킯�ł͂Ȃ��B
        �]���āA�񋓂Ɏ��Ԃ�������`���ł́AIKpiUnArc ���쐬����Ɠ����ɑS�Ă�
        �i�[�t�@�C����񋓂��Ă����悤�Ȃ��Ƃ͂��Ȃ����Ƃ𐄏��B
      �E���ǉ������������񂾂��킩��Ȃ������B(^^�U
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiUnArcModule : public IKpiUnknown
{
public:
    virtual void  WINAPI GetModuleInfo(const KPI_UNARC_MODULEINFO **ppInfo)=0;
    virtual BOOL  WINAPI Open(IKpiFile   *pFile,  //�A�[�J�C�u�t�@�C��
                              IKpiFolder *pFolder,//�A�[�J�C�u�t�@�C��������t�H���_
                              const wchar_t *cszOutputPath,//��Ɨp�t�H���_
                              IKpiUnArc **ppUnArc)=0;
    virtual BOOL  WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator)=0;
    virtual DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//�Z�N�V������
                                     const wchar_t *cszKey,    //�L�[��
                                     INT64  nValue, //�ύX��̒l(BOOL/INT �̏ꍇ)
                                     double dValue, //�ύX��̒l(FLOAT �̏ꍇ)
                                     const wchar_t *cszValue)=0;//�ύX��̒l(������)
/*
    void  WINAPI GetModuleInfo(const KPI_UNARC_MODULEINFO **ppInfo)=0;
      KPI_UNARC_MODULEINFO �� *ppInfo �ɐݒ肵�ĕԂ��B
      IKpiDecoderModule::GetModuleInfo(kpi_decoder.h) �Ƃقړ������߁A�������
      ���킹�ĎQ�Ƃ��邱�ƁB

    BOOL WINAPI Open(IKpiFile   *pFile,  //�A�[�J�C�u�t�@�C��
                     IKpiFolder *pFolder,//�A�[�J�C�u�t�@�C��������t�H���_
                     const wchar_t *cszWorkPath,//��Ɨp�t�H���_
                     IKpiUnArc **ppUnArc)=0;
      pFile/pFolder ����A�[�J�C�u���J��
      ���������� IKpiUnArc �̃C���X�^���X�� *ppUnArc �ɐݒ肵�� TRUE ��Ԃ�
      (�G���[�̏ꍇ�� *ppUnArc �� NULL ��ݒ肵�� 0 ��Ԃ�)

      ���t�@�C���Ƃ��ēW�J����ꍇ�͂Ȃ�ׂ� cszOutputPath �ȉ��ɍ쐬���邱�ƁB
      cszOutputPath �ȉ��ɍ쐬�����t�@�C��(IKpiUnArc::Extract �ŕԂ� IKpiFile �� GetRealFileW
      �ŕԂ����t�@�C��)�y�уt�H���_(cszOutputPath ��A���̉��ɍ쐬�����t�H���_)�̍폜���v���O
      �C�����ōs���K�v�͂Ȃ��A�v���O�C�����g�p���鑤�ōs��
      �������AcszOutputPath �ȊO�̃t�H���_�Ɏ��t�@�C��/���t�H���_���쐬�����ꍇ�́AExtract ��
      �Ԃ� IKpiFile �� IKpiUnArc �� Release �Ƀv���O�C�����ō폜���Ȃ���΂Ȃ�Ȃ��B

      ���t�@�C��������J�����\�b�h�͒񋟂���Ȃ�
      ���t�@�C��������J�������ꍇ�� IKpiFile::GetRealFileW/A ���Ăяo��

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

    IKpiUnArcModule �̃C���X�^���X�쐬�Ɣj���̓v���O�C�������[�h����Ă����
    ��x�����s�Ȃ���B


    �K�v�ȏ������̓C���X�^���X�쐬���A��n���̓C���X�^���X�j�����ɍs�����ƁB

    �{�̂̓v���O�C�������[�h�� Open �܂ōs�킸�A�Œ���̏�񂾂��擾��ɃA�����[�h
    ���邱�Ƃ�����B�Ⴆ�΋N�����Ƀv���O�C�������߂ĔF�������ꍇ�� GetModuleInfo
    �����Ăяo����ɃA�����[�h����B�ݒ��ʂ̕\�����A�v���O�C�����g�p���łȂ����

    GetModuleInfo/EnumConfig/ApplyConfig

    �����Ă΂��A�ݒ��ʂ�����Ƃ���ŃA�����[�h����B

    �����v���O�C���̏������Ɏ��Ԃ�������ꍇ�� Open ���ɏ�������1�x�������s����ȂǁA
    �v���O�C�����œƎ��ɍH�v���邱�ƁB���̍ۂ͖��������̂܂� IKpiUnArcModule
    ��j�����ꂽ��A�ǂ̃��\�b�h���ŏ��ɌĂ΂�Ă�����ɖ�肪�N����Ȃ��悤��
    ���ӂ��邱�ƁB

*/
};
///////////////////////////////////////////////////////////////////////////////
//�v���O�C�����G�N�X�|�[�g����֐�
//typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,      //IKpiXXXModule �� IID
//                                                void **ppvObject, //IKpiXXXModule �̖߂�l
//                                                IKpiUnknown *pUnknown);
//HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
//IKpiDecoderModule �Ɠ���(kpi.h ���Q��)
//riid �� IID_IKpiUnArcModule ���n�����
//�K�v�ł���� pUnknown ���� IKpiConfig �����擾���邱��
//kpi_CreateConfig ���̃w���p�[�֐����Ăԏꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ�����
///////////////////////////////////////////////////////////////////////////////
//�A�[�J�C�u�v���O�C�������[�J���t�@�C���⃁������ IKpiFile �ɑΉ������邽�߂�
//�w���p�[�֐�
///////////////////////////////////////////////////////////////////////////////
extern DWORD WINAPI kpi_CreateLocalFile(IKpiUnkProvider *pProvider,
                                          const wchar_t *cszFileNameFullPath,
                                          IKpiFile **ppFile);
extern DWORD WINAPI kpi_CreateLocalFile(IKpiUnknown *pUnknown, //kpi_CreateInstance �̑�3�������璼�ڎ擾
                                          const wchar_t *cszFileNameFullPath,
                                          IKpiFile **ppFile);
//�Ekpi_CreateLocalFile �� IKpiUnArc::Extract ���ŌĂяo���A�擾���� IKpiFile �� �C���X�^���X��
//  Extract �̑�2����(*ppFile) �ɂ��̂܂ܓn���Ȃ���΂Ȃ�Ȃ�
//�EcszFileNameFullPath �� IKpiUnArcModule::Open �� cszOutputPath �ȉ��ɍ쐬(�K�w�\�����Č�)���ꂽ���̂ł��邱�Ƃ���������
//�EcszFileNameFullPath �� IKpiUnArc ����������܂ő��݂��Ă��Ȃ���΂Ȃ�Ȃ�
//�EcszFileNameFullPath �� cszOutputPath �ȉ��ɂ���ꍇ�AIKpiUnArc �̉�����Ƀt�@�C�����폜����K�v�͂Ȃ�
//  (�A�[�J�C�u�v���O�C�����g�p���鑤���K�v�ɉ����č폜����)
//
extern DWORD WINAPI kpi_CreateMemoryFile(IKpiUnkProvider *pProvider,
                                         const BYTE *pBuffer,  //�������o�b�t�@�ւ̃|�C���^
                                         size_t stSize,        //�������o�b�t�@�̃T�C�Y
                                         const wchar_t *cszFileName,//GetFileName ���Ԃ��t�@�C����
                                                               //�p�X�̋�؂蕶��(\ �� /)���܂߂Ă��ǂ����A�t�@�C���������݂̂��L��
                                         IKpiFile **ppFile);
extern DWORD WINAPI kpi_CreateMemoryFile(IKpiUnknown *pUnknown,//kpi_CreateInstance �̑�3�������璼�ڎ擾
                                         const BYTE *pBuffer,  //�������o�b�t�@�ւ̃|�C���^
                                         size_t stSize,        //�������o�b�t�@�̃T�C�Y
                                         const wchar_t *cszFileName,//GetFileName ���Ԃ��t�@�C����
                                                               //�p�X�̋�؂蕶��(\ �� /)���܂߂Ă��ǂ����A�t�@�C���������݂̂��L��
                                         IKpiFile **ppFile);
//�Ekpi_CreateMemoryFile �� IKpiUnArc::Extract ���ŌĂяo���A�擾���� IKpiFile �� �C���X�^���X��
//  Extract �̑�2����(*ppFile) �ɂ��̂܂ܓn���Ȃ���΂Ȃ�Ȃ�
//�EpBuffer �͖{�̑��ł̓R�s�[����Ȃ����߁AIKpiUnArc ����������܂ŗL���łȂ���΂Ȃ�Ȃ�
//
//�v���O�C���g�p���p�̃w���p�[�֐�(�v���O�C���쐬�ɂ͕s�v)
//�v���O�C���g�p��(�ʏ�� KbMedia Player)���Ăяo���ꍇ�� kpi_unarc.cpp ��
//�v���W�F�N�g�ɒǉ�����
///////////////////////////////////////////////////////////////////////////////
extern HRESULT WINAPI kpi_CreateUnArcModule(pfn_kpiCreateInstance fnCreateInstance,
                                            IKpiUnArcModule **ppModule, IKpiUnknown *pUnknown);
#pragma pack(pop) //�\���̂̃A���C�������g
#endif
