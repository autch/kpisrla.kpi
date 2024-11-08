#pragma once
#ifndef kpi_playerH
#define kpi_playerH

#include "kpi.h"
#include "kpi_decoder.h"
#pragma pack(push, 1) //構造体のアラインメント

///////////////////////////////////////////////////////////////////////////////
/*演奏プラグイン
  デコーダプラグインのように細かな演奏制御は出来ないが、動画を扱うことが出来る。
  動画でなくても、ライブラリの仕様で PCM を取得出来ずに演奏制御しか出来ない
  ような形式にも対応可能。

  動画のウィンドウや再生速度の変更等はプラグイン側で制御。
  Kobarin の技術力不足のため、IKpiDecoder の映像対応版みたいなものは未対応。

  演奏プラグインがエクスポートする関数
  typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,    //IKpiXXXModule の IID
                                                void **ppvObject, //IKpiXXXModule の戻り値
                                                IKpiUnknown *pUnknown);
  HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
  riid に IID_IKpiPlayerModule が渡される。
  必要であれば pUnknown から IKpiConfig 等を取得すること。
  kpi_CreateConfig 等のヘルパー関数を呼ぶ場合は kpi.cpp もプロジェクトに追加する。

  *ppvObject にインスタンスを渡す場合は、適切な型にキャストすること。
  例えば、

    if(IsEqualIID(riid, IID_IKpiUnArcModule){
        CKpiXXXPlayerModule *pModule = new CKpiXXXPlayerModule;
        // *ppvObject = pModule; //NG
        *ppvObject = (IKpiPlayerModule*)pModule; //OK
    }

  のようにすること。CKpiXXXPlayerModule が複数のクラスから派生している場合、
  適切なポインタが渡されない可能性がある。
*/
///////////////////////////////////////////////////////////////////////////////
#define KPI_PLAYER_MODULE_VERSION 3 //仕様確定時に 100 とする予定

class __declspec(uuid("{CD083F95-E9D2-4C8A-9FF0-A4003AB220F8}")) IKpiPlayer;
class __declspec(uuid("{20706B19-8A25-4DF6-890E-FF76AE7386C1}")) IKpiPlayerModule;

#define IID_IKpiPlayer       __uuidof(IKpiPlayer)
#define IID_IKpiPlayerModule __uuidof(IKpiPlayerModule)

///////////////////////////////////////////////////////////////////////////////
typedef KPI_DECODER_MODULEINFO KPI_PLAYER_MODULEINFO;
//KPI_PLAYER_MODULEINFO::dwModuleVersion == KPI_PLAYER_MODULE_VERSION
//としなければならないことを除けば KPI_DECODER_MODULEINFO と同じ
//
enum{//IKpiPlayer::GetStatus の戻り値
    KPI_STATUS_STOP = 0,    //停止中
    KPI_STATUS_PLAYING = 1, //演奏中
    KPI_STATUS_PAUSE =   2, //一時停止中
};
///////////////////////////////////////////////////////////////////////////////
class IKpiPlayer : public IKpiUnknown
{
public:
    virtual DWORD WINAPI Select(DWORD dwNumber,
                                const KPI_MEDIAINFO **ppInfo,
                                IKpiTagInfo *pTagInfo,
                                DWORD dwTagGetFlags)=0;
    virtual BOOL  WINAPI Play(DWORD dwStartPos, BOOL bVisibile)=0;//dwStartPos の位置から演奏開始
    virtual BOOL  WINAPI Pause(void)=0;  //一時停止
    virtual BOOL  WINAPI Restart(void)=0;//再開
    virtual void  WINAPI Stop(void)=0;   //演奏停止
    virtual DWORD WINAPI GetPosition(void)=0;//再生位置取得(単位はミリ秒)
    virtual DWORD WINAPI GetStatus(void)=0;  //演奏状態(KPI_STATUS_STOP/PLAYING/PAUSE)
    virtual void  WINAPI SetTempo(int nTempo)=0;
    virtual void  WINAPI SetVolume(int nVolume)=0;
    virtual void  WINAPI SetVisible(BOOL bVisible)=0;
    virtual void  WINAPI GetWindowPosition(int *pLeft, int *pTop,
                                           int *pVideoWidth, int *pVideoHeight)=0;
    virtual void  WINAPI SetWindowPosition(int nLeft, int nTop,
                                           int nVideoWidth, int nVideoHeight)=0;
    virtual void  WINAPI SetTopMost(BOOL bTopMost)=0;
    virtual void  WINAPI SetRepeat(BOOL bRepeat)=0;
    virtual DWORD WINAPI UpdateConfig(void *pvReserved)=0; //設定が変更されたら呼ばれる
/*
    演奏プラグインの詳細な説明は省略。
    メソッドの名称である程度想像がつくはず。

    SetWindowPosition によりウィンドウがデスクトップ画面外にはみ出したときの
    処理もプラグイン側で行う(本体でもある程度対策しているが)
    マルチモニタ環境も考慮に入れること。

    演奏プラグインは厳密に考えると意外と難しい…。

    Play() 呼び出し直後の GetStatus の結果がライブラリ次第で実際はまだ STOP
    だったりするなど。そういう場合は Play() 呼び出しから1～2秒程度は無条件に
    PLAYING と見なすなどして対処する。(本体側でそう解釈させるようにしているので
    実際は不要)

    実際に演奏が始まるのが PostMessage によるメッセージを受け取った後だったり
    すると、

    Play();
    Stop();

    で演奏が止まらない場合もあったりする。昔の RealMedia 関係がそうなっていて
    とても扱いにくかった。(最近はどうだか知らないが)

    各種メソッドはいつも同一スレッドから呼ばれるとは限らないが、全てのメソッド
    は本体によって排他的に呼ばれることが保証される。
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
    virtual DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//セクション名
                                     const wchar_t *cszKey,    //キー名
                                     INT64  nValue, //変更後の値(BOOL/INT の場合)
                                     double dValue, //変更後の値(FLOAT の場合)
                                     const wchar_t *cszValue)=0;//変更後の値(文字列)
/*
    void  WINAPI GetModuleInfo(const KPI_PLAYER_MODULEINFO **ppInfo)=0;
      KPI_PLAYER_MODULEINFO を *ppInfo に設定して返す。
      IKpiDecoderModule::GetModuleInfo(kpi_decoder.h) とほぼ同じため、そちらも
      合わせて参照すること。

    DWORD WINAPI Open(const KPI_MEDIAINFO *cpRequest,//再生周波数等の要求値が含まれた KPI_MEDIAINFO
                              IKpiFile    *pFile,    //音楽データ
                              IKpiFolder  *pFolder,  //音楽データがあるフォルダ
                              IKpiPlayer **ppPlayer);
      pFile/pFolder から曲を開く。
      成功したら IKpiPlayer のインスタンスを *ppPlayer に設定し、含まれている
      曲の数を返す。(エラーの場合は *ppPlayer に NULL を設定して 0 を返す)

      IKpiDecoderModule::Open(kpi_decoder.h)と考え方はほぼ同じため、そちらも
      合わせて参照すること。

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

    IKpiPlayerModule のインスタンス作成と破棄はプラグインがロードされている間
    一度だけ行なわれる。

    必要な初期化はインスタンス作成時、後始末はインスタンス破棄時に行うこと。

    本体はプラグインをロード後 Open まで行わず、最低限の情報だけ取得後にアンロード
    することがある。例えば起動時にプラグインを初めて認識した場合は GetModuleInfo
    だけ呼び出し後にアンロードする。設定画面の表示時、プラグインを使用中でなければ

    GetModuleInfo/EnumConfig/ApplyConfig

    しか呼ばず、設定画面を閉じたところでアンロードする。

    もしプラグインの初期化に時間がかかる場合は Open 時に初期化を1度だけ実行するなど、
    プラグイン側で独自に工夫すること。その際は未初期化のまま IKpiPlayerModule
    を破棄されたり、どのメソッドが最初に呼ばれても動作に問題が起こらないように
    注意すること。

*/
};
///////////////////////////////////////////////////////////////////////////////
extern HRESULT WINAPI kpi_CreatePlayerModule(pfn_kpiCreateInstance fnCreateInstance,
                                             IKpiPlayerModule **ppModule, IKpiUnknown *pUnknown);
//↑プラグイン使用側用のヘルパー関数(プラグイン作成には不要)
//プラグイン使用者(通常は KbMedia Player)が呼び出す場合は kpi_player.cpp も
//プロジェクトに追加する
///////////////////////////////////////////////////////////////////////////////
#pragma pack(pop) //構造体のアラインメント
#endif

