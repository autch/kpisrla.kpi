#pragma once
#ifndef kpi_unarcH
#define kpi_unarcH

#include "kpi.h"
#pragma pack(push, 1) //構造体のアラインメント

///////////////////////////////////////////////////////////////////////////////
/*アーカイブプラグイン
  KbMedia Player で圧縮ファイル内の曲を再生出来るようにする。

  作り方次第では、実ファイルをテンポラリに作成せず、圧縮ファイルを展開しながら
  再生することも出来る。(デコーダ/演奏プラグインが IKpiFile にきちんと対応して
  いる必要がある)

  あくまでも音楽再生のためのプラグインであって、アーカイバから利用されるもの
  ではないので、取得できる情報等は最小限で良いはず。
  音楽再生に向かない書庫(パスワードの入力が必要とか)に無理してまで対応する
  仕組みを考える必要はない、と思う。

  アーカイブプラグインを作るのは難しいと思われる。

  アーカイブプラグインがエクスポートする関数
  typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,    //IKpiXXXModule の IID
                                                void **ppvObject, //IKpiXXXModule の戻り値
                                                IKpiUnknown *pUnknown);
  HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
  riid に IID_IKpiUnArcModule が渡される。
  必要であれば pUnknown から IKpiConfig 等を取得すること。
  kpi_CreateConfig 等のヘルパー関数を呼ぶ場合は kpi.cpp もプロジェクトに追加する。

  *ppvObject にインスタンスを渡す場合は、適切な型にキャストすること。
  例えば、

    if(IsEqualIID(riid, IID_IKpiUnArcModule){
        CKpiXXXUnArcModule *pModule = new CKpiXXXUnArcModule;
        // *ppvObject = pModule; //NG
        *ppvObject = (IKpiUnArcModule*)pModule; //OK
    }

  のようにすること。CKpiXXXUnArcModule が複数のクラスから派生している場合、
  適切なポインタが渡されない可能性がある。
*/
///////////////////////////////////////////////////////////////////////////////
#define KPI_UNARC_MODULE_VERSION 2 //仕様確定時に 100 とする予定

class __declspec(uuid("{5496FFEA-0644-4B60-910F-6FFB1A25E298}")) IKpiUnArc;
class __declspec(uuid("{E005FFCC-C053-4E12-8275-77920EFA14F7}")) IKpiUnArcModule;
#define IID_IKpiUnArc       __uuidof(IKpiUnArc)
#define IID_IKpiUnArcModule __uuidof(IKpiUnArcModule)

//kpi_CreateLocalFile/kpi_CreateMemoryFile 用
//(IKpiLocalFile_/IKpiMemoryFile_ というクラスは実際には存在しない)
class __declspec(uuid("{53247E3A-3539-4707-A1F5-B052ADBCA3E4}")) IKpiLocalFile_;
class __declspec(uuid("{FDAF31A3-7AEA-4466-A190-16073C3CA2FF}")) IKpiMemoryFile_;
#define IID_LocalFile  __uuidof(IKpiLocalFile_)
#define IID_MemoryFile __uuidof(IKpiMemoryFile_)

///////////////////////////////////////////////////////////////////////////////
struct KPI_UNARC_MODULEINFO
{
    DWORD cb;             //=sizeof(KPI_UNARC_MODULEINFO)
    DWORD dwModuleVersion;//=KPI_UNARC_MODULE_VERSION
    DWORD dwPluginVersion;//プラグインのバージョン
    DWORD dwMultipleInstance;//KPI_MULTINST_INFINITE/ZERO/ONE/UNIQUE
    GUID  guid;           //プラグイン識別子
    const wchar_t *cszDescription; //表示名
    const wchar_t *cszCopyright;   //著作権(複数名いる場合は改行文字可)
    const wchar_t *cszSupportExts; //対応拡張子('/' で区切る)(例：".lzh/.zip", ".aaa/.bbb")(拡張子も含める)
    const wchar_t *cszSolidExts;   //対応拡張子のうち、ソリッド形式の可能性があるもの(開いてみないと分からないものを含む)
    DWORD dwSupportConfig;         //IKpiConfig による設定の読み書きに未対応なら 0
    DWORD dwReserved[4];           //must be 0
};
///////////////////////////////////////////////////////////////////////////////
struct KPI_ARCHIVEINFO
{//アーカイブ自体の情報
    DWORD       cb;            //=sizeof(KPI_ARCHIVEINFO)
    DWORD       dwType;        //=0
    DWORD       dwCount;       //格納ファイル数(列挙が終わるまで数がわからない場合は -1)
    DWORD       dwSolid;       //ソリッド書庫を含むなら 1、そうでないなら 0
                               //列挙が終わるまでわからないが含む可能性があるなら -1
    DWORD       dwReserved[12];//must be 0
};
///////////////////////////////////////////////////////////////////////////////
struct KPI_FILEINFO
{//アーカイブ内格納ファイルの情報
    DWORD       cb;          //=sizeof(KPI_FILEINFO)
    DWORD       dwType;      //=0
    const wchar_t *cszFileName;//格納ファイル名(パス名の区切りは \ ではなく / を使用しなければならない)
                               //ディレクトリの場合の末尾は / でなければならない
                               //文字列コピーの時間がもったいない＆長さの制限をなくしたいので文字列への
                               //ポインタを直接入れる。寿命は IKpiUnArc が Release されるまで。
    const wchar_t *cszMethod;//圧縮法(名前がない場合は NULL または "")
                             //文字列コピーの時間がもったいない＆長さの制限をなくしたいので文字列への
                             //ポインタを直接入れる。寿命は IKpiUnArc が Release されるまで。
    BOOL        bCrypted;    //暗号化されてれば 0 以外、暗号化されてなければ 0
    BOOL        bStored;     //無圧縮なら 0 以外、圧縮されてれば 0
    UINT64      qwOriginalSize;  //解凍後のサイズ
    UINT64      qwCompressedSize;//圧縮後のサイズ(わからない場合は 0)
    FILETIME    ft;              //更新日時(わからない場合は 0)
    DWORD       dwReserved[4];   //must be 0
};
///////////////////////////////////////////////////////////////////////////////
class IKpiUnArc : public IKpiUnknown
{
public:
    virtual BOOL  WINAPI GetArchiveInfo(const KPI_ARCHIVEINFO **ppArchiveInfo)=0;
    virtual BOOL  WINAPI EnumFileInfo(DWORD dwIndex, const KPI_FILEINFO **ppFileInfo)=0;
    virtual BOOL  WINAPI Extract(DWORD dwIndex, IKpiFile **ppFile, UINT64 qwSize)=0;
    virtual DWORD WINAPI UpdateConfig(void *pvReserved)=0; //設定が変更されたら呼ばれる
/*
    BOOL WINAPI GetArchiveInfo(const KPI_ARCHIVEINFO **ppArchiveInfo);
      開いたアーカイブ自身の情報を返す。
      *ppArchiveInfo は IKpiUnArc のインスタンスが Release されるまで有効。
    BOOL WINAPI EnumFileInfo(DWORD dwIndex, const KPI_FILEINFO **ppFileInfo);
      格納ファイルを列挙する。
      *ppFileInfo に dwIndex に対応する格納ファイルの情報を返す。
      成功した場合は 0 以外を返す。
      失敗した場合は 0 を返して列挙終了。(これ以上の dwIndex を渡しても失敗)
      ppFileInfo に返した構造体へのポインタは IKpiUnArc の
      インスタンスが Release されるまで有効。
      構造体コピーの時間がもったいない(貧乏性)
    BOOL WINAPI Extract(DWORD dwIndex, IKpiFile **ppFile, UINT64 qwSize);
      dwIndex に対応する格納オブジェクトを解凍する。
      qwSize は解凍したいサイズ。(UINT64)-1 のときは全部解凍する。
      必要な部分だけ解凍したい場合に使われる。
    DWORD WINAPI UpdateConfig(void *pvReserved);
      IKpiUnArcModule::ApplyConfig の呼び出しで kPI_CFGRET_NOTIFY が返された
      場合のみ、本体によって IKpiUnArc のインスタンスごとに呼ばれる
      呼ばれた時点で設定は既に変更済みであり、どの設定が変更されたかを知ること
      は出来ない
      pvReserved は将来の予約(NULL が渡される)
      戻り値の意味は将来の予約 => 0 を返すこと
      IkpiDecoder に合わせて IKpiUnArc にも追加したが、ここで設定変更を適用
      させる利点はあまりないと思われる

    Extract による解凍結果の *ppFile に関しての注意点。(IKpiFile も合わせて参照)
      ・展開後のデータが実ファイルの場合、
        ・ヘルパー関数 kpi_CreateLocalFile を呼び出すことでアーカイブプラグイン側での実装を省略可能
        ・実ファイルは IKpiUnArc が Release されるまでは存在していなければならない
        ・実ファイルの削除は必要に応じてプラグインを使用する側が必要に応じて行う(cszOutputPath 以下に作成した場合)
      ・展開後のデータがメモリバッファの場合、
        ・ヘルパー関数 kpi_CreateMemoryFile を呼び出すことでアーカイブプラグイン側での実装を省略可能
        ・kpi_CreateMemoryFile を使用する場合、メモリバッファは IKpiUnArc が Release されるまで有効でなければならない
      ・展開後のデータが実ファイルでもメモリバッファでもない場合は IKpiFile を実装しなければならない
      ・CreateClone は実装不要。本体が CreateClone を呼ぶことはない。
      ・Seek の戻り値は、要求先位置より手前であれば、要求先位置と完全に同じでなくても
        良い。ただし、要求先位置と同じか手前でなければならない。例えば
        ・要求先位置が 1024 の場合に 512 の位置にシークしても良い。
        ・要求先位置が 1024 の場合に 2048 の位置にシークしてはいけない。
      ・Seek に失敗した(=KPI_FILE_EOF を返した)後の現在位置やその後の Read の結果は未定義。
      ・Seek に失敗した後、dwOrigin==FILE_BEGIN or dwOrigin==FILE_END での Seek には出来る
        限り成功しなければならない。特に Seek(0, FILE_BEGIN) には余程のことがない限り成功
        しなければならない。
      ・Abort には対応しなくても良い。(対応しない場合は FALSE を返すこと)
      ・複数回 Extract した場合、前回呼び出し時に返した IKpiFile のインスタンスが
        まだ残っていても正常動作しなければならず、前回呼び出し時と今回呼び出し時
        のインスタンスを同時に使用しても正しく(独立して)動作しなければならない。
        ・動作さえ正しければ、GetRealFile/GetBuffer が返すポインタは共有して良い。
      ・GetFileName はディレクトリパスを除いたものを返すこと。
        (aaa/bbb/ccc.xxx なら ccc.xxx を返す)
      ・パフォーマンス上のメリットがないなら GetRealFileW は失敗しても良い。
      ・パフォーマンス上のメリットがないなら GetBuffer は失敗しても良い。
      ・GetRealFileA は実装不要。実装するなら GetRealFileW の方にすること。
        本体が GetRealFileA を呼ぶことはない。
      ・GetRealFileW が成功する場合、一時ファイル名の拡張子は GetFileName が返すのと
        同じでなければならない。
        例えば foo.lzh>xxx/aaa.txt に対する一時ファイル名が
            C:\TEMP\xxxx\aaa.doc となってはいけない。
            C:\TEMP\xxxx\bbb.txt となるのは非推奨だがOK(ファイル名部分は変わって良い)
      ・GetRealFileW が成功する場合、一時ファイルのディレクトリ階層部分を除いた
        ファイル名は GetFileName が返すのと同じであることを推奨。
       (出来ればディレクトリ階層部分の名前も同じであることを推奨)
        例えば foo.lzh>xxx/aaa.txt に対する一時ファイル名が
            C:\TEMP\xxxx\yyy\aaa.txt となるよりは、
            C:\TEMP\xxxx\xxx\aaa.txt となることを推奨。
      ・GetRealFileW が成功する場合、一時ファイル出力先は Open の第3引数
        cszOutputPath 以下であることを強く推奨。
      ・GetRealFileW が成功する場合、同一アーカイブ内の他の格納ファイルの
        GetRealFileW も成功し、ディレクトリ階層構造も復元されていることを推奨。
      ・GetRealFileW/GetBuffer の呼び出しの前後でファイルポインタが変わってはいけない。
        (Read/Seek の結果に影響してはいけない)
      ・同じ IKpiFile のインスタンスに対して複数のスレッドから同時に
        ・GetRealFileW/GetBuffer/Read/Seek/GetSize が呼ばれる可能性に対して配慮する必要はない。
        ・IKpiFile を使う側で配慮する
        ・異なる IKpiFile のインスタンスに対しては配慮しなければならない。
      ・IKpiUnArc の使用者(KbMedia Player 本体またはアーカイブプラグインを使用するアプリケーション)は、
        Extract で返した *ppFile を Release するまで IKpiUnArc を Release してはいけない。
      ・アーカイブプラグインの作成者は IKpiUnArc が Release されるとき、そのインスタンスが作成した
        IKpiFile は既に Release されていることを想定して良い。
      ・GetRealFileW で作成した実ファイルの削除はプラグインを使用する側が行う。
        (IKpiUnArcModule::Open の cszOutputPath 以下に作成した場合)
        cszOutputPath 以下ではないフォルダに作成した場合(推奨されない)は IKpiFile か
        IKpiUnArc の Release 時にプラグイン自身で削除しなければならない。

    EnumFileInfo による列挙に関する注意点。
      ・空でないディレクトリは、列挙しなくても良い。
        ・例えば aaa/bbb.txt が列挙される場合、aaa/ を列挙する必要はない。
        ・列挙しても構わない。
      ・格納ファイル名の重複列挙(大文字と小文字を区別することによる重複など)は
        出来るだけ起こらないようにすること。
        ・本体が重複チェックをするので必須ではない。
      ・Windows で使用出来ない文字(:*?"<>|)を使用した格納ファイル名は、出来るだけ
        列挙しないようにすること。
        ・本体がチェックするので必須ではない。
      ・Directory Traversal 問題を起こすような格納ファイル名は、出来るだけ列挙
        しないようにすること。
        ・本体がチェックするので必須ではない。
      ・EnumFileInfo で返した KPI_FILEINFO の寿命は IKpiUnArc が Release される
        までであることに十分に気を遣うこと。
        ・例えば内部で KPI_FILEINFO の配列を持っていて、EnumFileInfo の呼び出し
          に応じて realloc で配列を拡大する、といったことをすると再現性の低い
          バグを引き起こす原因となる。
      ・KbMedia Player 本体は、必要になるまで EnumFileInfo を呼ばない。
        列挙を始めても、終了まで一気に列挙するわけではない。
        従って、列挙に時間がかかる形式では、IKpiUnArc を作成すると同時に全ての
        格納ファイルを列挙しておくようなことはしないことを推奨。
      ・結局何が言いたいんだかわからないかも。(^^ゞ
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiUnArcModule : public IKpiUnknown
{
public:
    virtual void  WINAPI GetModuleInfo(const KPI_UNARC_MODULEINFO **ppInfo)=0;
    virtual BOOL  WINAPI Open(IKpiFile   *pFile,  //アーカイブファイル
                              IKpiFolder *pFolder,//アーカイブファイルがあるフォルダ
                              const wchar_t *cszOutputPath,//作業用フォルダ
                              IKpiUnArc **ppUnArc)=0;
    virtual BOOL  WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator)=0;
    virtual DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//セクション名
                                     const wchar_t *cszKey,    //キー名
                                     INT64  nValue, //変更後の値(BOOL/INT の場合)
                                     double dValue, //変更後の値(FLOAT の場合)
                                     const wchar_t *cszValue)=0;//変更後の値(文字列)
/*
    void  WINAPI GetModuleInfo(const KPI_UNARC_MODULEINFO **ppInfo)=0;
      KPI_UNARC_MODULEINFO を *ppInfo に設定して返す。
      IKpiDecoderModule::GetModuleInfo(kpi_decoder.h) とほぼ同じため、そちらも
      合わせて参照すること。

    BOOL WINAPI Open(IKpiFile   *pFile,  //アーカイブファイル
                     IKpiFolder *pFolder,//アーカイブファイルがあるフォルダ
                     const wchar_t *cszWorkPath,//作業用フォルダ
                     IKpiUnArc **ppUnArc)=0;
      pFile/pFolder からアーカイブを開く
      成功したら IKpiUnArc のインスタンスを *ppUnArc に設定して TRUE を返す
      (エラーの場合は *ppUnArc に NULL を設定して 0 を返す)

      実ファイルとして展開する場合はなるべく cszOutputPath 以下に作成すること。
      cszOutputPath 以下に作成したファイル(IKpiUnArc::Extract で返す IKpiFile が GetRealFileW
      で返す実ファイル)及びフォルダ(cszOutputPath や、その下に作成したフォルダ)の削除をプラグ
      イン側で行う必要はなく、プラグインを使用する側で行う
      ただし、cszOutputPath 以外のフォルダに実ファイル/実フォルダを作成した場合は、Extract で
      返す IKpiFile か IKpiUnArc の Release にプラグイン側で削除しなければならない。

      実ファイル名から開くメソッドは提供されない
      実ファイル名から開きたい場合は IKpiFile::GetRealFileW/A を呼び出す

    BOOL WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator);
      設定項目を列挙する。
      本体が列挙された設定項目を元に設定画面を自動生成する。
      設定項目がない場合は FALSE を返す、ある場合は TRUE を返す。
      IKpiDecoderModule::EnumConfig(kpi_decoder.h) とほぼ同じため、そちらも
      合わせて参照すること。

    DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//セクション名
                             const wchar_t *cszKey,    //キー名
                             INT64  nValue, //変更後の値(BOOL/INT の場合)
                             double dValue, //変更後の値(FLOAT の場合)
                             const wchar_t *cszValue)=0;//変更後の値(文字列)
      cszSection と cszKey に該当する設定の変更を「適用」する直前に呼ばれる。
      呼ばれた時点ではまだ設定は変更されていない。
      IKpiDecoderModule::ApplyConfig(kpi_decoder.h) とほぼ同じため、そちらも
      合わせて参照すること。

    IKpiUnArcModule のインスタンス作成と破棄はプラグインがロードされている間
    一度だけ行なわれる。


    必要な初期化はインスタンス作成時、後始末はインスタンス破棄時に行うこと。

    本体はプラグインをロード後 Open まで行わず、最低限の情報だけ取得後にアンロード
    することがある。例えば起動時にプラグインを初めて認識した場合は GetModuleInfo
    だけ呼び出し後にアンロードする。設定画面の表示時、プラグインを使用中でなければ

    GetModuleInfo/EnumConfig/ApplyConfig

    しか呼ばず、設定画面を閉じたところでアンロードする。

    もしプラグインの初期化に時間がかかる場合は Open 時に初期化を1度だけ実行するなど、
    プラグイン側で独自に工夫すること。その際は未初期化のまま IKpiUnArcModule
    を破棄されたり、どのメソッドが最初に呼ばれても動作に問題が起こらないように
    注意すること。

*/
};
///////////////////////////////////////////////////////////////////////////////
//プラグインがエクスポートする関数
//typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,      //IKpiXXXModule の IID
//                                                void **ppvObject, //IKpiXXXModule の戻り値
//                                                IKpiUnknown *pUnknown);
//HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
//IKpiDecoderModule と同じ(kpi.h を参照)
//riid に IID_IKpiUnArcModule が渡される
//必要であれば pUnknown から IKpiConfig 等を取得すること
//kpi_CreateConfig 等のヘルパー関数を呼ぶ場合は kpi.cpp もプロジェクトに追加する
///////////////////////////////////////////////////////////////////////////////
//アーカイブプラグインがローカルファイルやメモリを IKpiFile に対応させるための
//ヘルパー関数
///////////////////////////////////////////////////////////////////////////////
extern DWORD WINAPI kpi_CreateLocalFile(IKpiUnkProvider *pProvider,
                                          const wchar_t *cszFileNameFullPath,
                                          IKpiFile **ppFile);
extern DWORD WINAPI kpi_CreateLocalFile(IKpiUnknown *pUnknown, //kpi_CreateInstance の第3引数から直接取得
                                          const wchar_t *cszFileNameFullPath,
                                          IKpiFile **ppFile);
//・kpi_CreateLocalFile は IKpiUnArc::Extract 内で呼び出し、取得した IKpiFile の インスタンスは
//  Extract の第2引数(*ppFile) にそのまま渡さなければならない
//・cszFileNameFullPath は IKpiUnArcModule::Open の cszOutputPath 以下に作成(階層構造も再現)されたものであることを強く推奨
//・cszFileNameFullPath は IKpiUnArc が解放されるまで存在していなければならない
//・cszFileNameFullPath が cszOutputPath 以下にある場合、IKpiUnArc の解放時にファイルを削除する必要はない
//  (アーカイブプラグインを使用する側が必要に応じて削除する)
//
extern DWORD WINAPI kpi_CreateMemoryFile(IKpiUnkProvider *pProvider,
                                         const BYTE *pBuffer,  //メモリバッファへのポインタ
                                         size_t stSize,        //メモリバッファのサイズ
                                         const wchar_t *cszFileName,//GetFileName が返すファイル名
                                                               //パスの区切り文字(\ や /)を含めても良いが、ファイル名部分のみが有効
                                         IKpiFile **ppFile);
extern DWORD WINAPI kpi_CreateMemoryFile(IKpiUnknown *pUnknown,//kpi_CreateInstance の第3引数から直接取得
                                         const BYTE *pBuffer,  //メモリバッファへのポインタ
                                         size_t stSize,        //メモリバッファのサイズ
                                         const wchar_t *cszFileName,//GetFileName が返すファイル名
                                                               //パスの区切り文字(\ や /)を含めても良いが、ファイル名部分のみが有効
                                         IKpiFile **ppFile);
//・kpi_CreateMemoryFile は IKpiUnArc::Extract 内で呼び出し、取得した IKpiFile の インスタンスは
//  Extract の第2引数(*ppFile) にそのまま渡さなければならない
//・pBuffer は本体側ではコピーされないため、IKpiUnArc が解放されるまで有効でなければならない
//
//プラグイン使用側用のヘルパー関数(プラグイン作成には不要)
//プラグイン使用者(通常は KbMedia Player)が呼び出す場合は kpi_unarc.cpp も
//プロジェクトに追加する
///////////////////////////////////////////////////////////////////////////////
extern HRESULT WINAPI kpi_CreateUnArcModule(pfn_kpiCreateInstance fnCreateInstance,
                                            IKpiUnArcModule **ppModule, IKpiUnknown *pUnknown);
#pragma pack(pop) //構造体のアラインメント
#endif
