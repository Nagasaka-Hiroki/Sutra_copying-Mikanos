# 疑問(第1章~第2章)
　osbook_day02aで疑問をソースコード内にべた書きしたところ、想像以上に多くなったのでファイルを分ける。
git diff と　みかん本を眺めて疑問に感じたことを即座に書く。可能であればすべて書くのが望ましいが、効率が良くないと感じたら書かない。そういったものは後日、または2周目以降に調べる。  
　ただ悶々と疑問を内に秘めるのは効率が良くない。できるだけ言語化しようと思う。  
　以下に[UEFIの仕様書](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_9_2021_03_18.pdf)のページ数を書く。みかん本と区別するためにspec.p.~と記す。

## osbook_day02a
　Main.cにおおよそ書いている。が一応再度記述する。

情報源はUEFIの仕様書。リンクは以下の通り。以降、仕様書と略す。  
[https://uefi.org/sites/default/files/resources/UEFI_Spec_2_9_2021_03_18.pdf](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_9_2021_03_18.pdf)  
また、以降仕様書のページ数とpdfのページ数を書くが、それぞれspec.p.とpdf.p.と略すことにする。  
英語になれるためにちょっとだけ英語で書く。  

### EFI_STATUS  ?
spec.p.20、pdf.p.95。表2-3より、  
データ型の一種でStatus code。型としてはUINTN。  

### EFIAPI ?
spec.p.19、pdf.p.94。2.3 Calling Conventions  
Calling Conventionsは呼び出し規約のこと。EFIAPIが仕様書ではじめて出てきたところの文章は以下のようになっている。  
以下仕様書より引用。
> In all cases, all pointers to UEFI functions are cast with the word EFIAPI. This allows the compiler for each architecture to supply the proper compiler keywords to achieve the needed calling conventions. 

意味的には、「すべての、UEFI関数へのポインタは EFIAPIでキャストされる。これによって、各アーキテクチャのコンパイラは必要な呼び出し規約を実現するために適切なコンパイラキーワードを提供できる」という感じ。
呼び出し規約は以前WinAPIを学習しているときに出てきた。関数を呼び出すときに引数などをスタック領域に積んでいく必要があるが、その時のルールを表していた。この文章的にはハードウェアの種類によって適切な積み方が違うからとりあえずC言語のコードではEFIAPIでキャストしておいて、内容は各コンパイラが環境に合わせておいてねという感じだろうか？費用対効果的にこれ以上調べる価値がなさそうなのでここまでにする。
とりあえずEFIAPIを見たら、WinAPIの __stdcallの仲間だと思うことにする。

たまたま見つけたので以下もメモ。  
spec.p.21、pdf.p.96。表2-4より。  
EFIAPI : Define the calling convention for UEFI interfaces.

### UefiMain
Loader.infで定義したエントリポイント。

### EFI_HANDLE ?
spec.p.20、pdf.p.95。表2-3より、引用。  
> A collection of realated interfaces. Type VOID*.  

ちなみにVOIDは以下の通り。同様に引用。  
> Undecleared type.

多分、普通のc言語のvoid型と同じだと思われる。なので、EFI_HANDLEはいわゆるvoidポインタというやつだと推測される。

### EFI_SYSTEM_TABLE ?
spec.p.62、pdf.p.137。表2-11より。 EFI_SYSTEM_TABLEは  
> Provide acsess to UEFI Boot Services, UEFI Runtime Services, consoles, firmware vendor infomation, and the system congigration tabeles.
となっている。

spec.p.93、pdf.p.168。 4.3. EFI System Table  
ここでは、具体的な内容、EFI_SYSTEM_TABLE構造体の中身について言及している。
初期の段階であるのであまり踏み込んで調べないが、読んでいるとシステムで使われる様々なサービス、例えばコンソールやブートサービスなど、のハンドルやサービスに関する入出力のポインタ（ここは少しあいまい、誤りの可能性大）が集められているといった印象。
書いた後で気づいたが、そのことは上記の引用文と言っていることはほどんど同じの気がする。なぜなら、Provide acsess to ~であるから。ブートサービスやランタイムサービス、コンソールなどへのアクセスを提供するという文言的には正しく見える。  
そのため、EFI_SYSTEM_TABLEで宣言された変数は、UEFIが提供するいろんなサービスへアクセスできるものだと考えて進める。  
（間違っていたら戻ってくる）

### EFI_STATUS EFIAPI UefiMain( EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) ? 
なぜ2つの変数を引数として持っているのか？  
spec.p.24、pdf.p.99。図2-2の説明文に以下のことが書かれている。  
> Figure 2-2 shows the stack after AddressOfEntryPoint in the image's PE32+ header has been called on supported 32-bit systems. All UEFI image entry points take two parameters. These are the image handle to the UEFI image, and a pointer to the EFI System Table.

この図はあくまでIA-32の説明だが、文中にはすべてのアーキテクチャに共通することを述べている。  
すべての UEFI image entry pointsは2つのパラメータをとる、と書かれている。  
そして、それがimage handle to the UEFI image　と　a pointer to the EFI System Tableということである。  

同様のことが以下にも記述されている。  
spec.p.91、pdf.p.166。  
```
4.1. UEFI Image Entry Point
    EFI_IMAGE_ENTRY_POINT
        Summary
            This is the main entry point for a UEFI image. This entry point is the same for UEFI applications and UEFI drivers.
        
        Prototype
            typedef
            EFI_STATUS
            (EFIAPI *EFI_IMAGE_ENTRY_POINT)(
                IN EFI_HANDLE ImageHandle,
                IN EFI_SYSTEM_TABLE *SystemTable
            );

        Parameters
            ImageHandle     The firmware allocated handle for the UEFI image. 
            SystemTable     A pointer to the EFI System Table.
```

つまり、UEFI application や　UEFI imageのエントリポイントはEFI_IMAGE_ENTRY_POINTでイメージハンドルとEFI System Tableへのポインタを引数として持つようにプロトタイプ宣言されているということだと思う。
また推測であるが、プロトタイプ宣言で型名のない（具体的にエントリポイント名が決まっていないということ）エントリポイント関数に名前をつけるという操作をしている、といった認識。これは別のc言語の本、独習Cのp.299に書かれている。この本によればtypedefを使うと関数ポインタのように型名をを持たない型に対して名前を付けるととができる。書き方は以下の通り。  
```
typedef 関数の戻り値の型　(*適当な関数名)(適当な関数名の関数が持つ引数リスト)
```
といったように書く。こうすることで適当な関数名といった型名を作ることができる。（具体的には関数ポインタを返り値として持つ関数の型名などに使える）
これを今回の内容に当てはめると以下の通りになる。

typedef EFI_STATUS (EFIAPI *EFI_IMAGE_ENTRY_POINT)(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
こう書くことでEFI_IMAGE_ENTRY_POINTという型名をつけて使うことができる。
なので、これも推測に過ぎないが、エントリポイントをLoader.infで定めるのはこういったようにエントリポイント名が関数名で決まっているのではなく、あくまで型名として決まっていて実体を定義していない（コンソールアプリケーションでは具体的にmain()、WinAPIではMinMain()であると決まっている）からではないかと思う。

読み返すと結構滅茶苦茶な文章を書いているがなんとなく趣旨は書けているので良いものとする。
以上がエントリポイントに関する認識である。間違っているところもあると思うが、とりあえず進めていく。

### Print ? 
仕様書にPrintの説明がない。仕様書には一応Print文が使われているところはある。例えば以下のページ。
spec.p.425、pdf.p.500。しかし、Printに関する説明はない。
疑問に思って、このファイルのヘッダファイルのインクルードなしにコンパイルしたが、コンパイルに失敗した。
しかし、上記のコードでは特にヘッダファイルなしに記述されている。コードのすべてが表示されているわけではないのかもしれないが不思議である。
edk2のリポジトリを確認したら発見できた。場所は次。  
[https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Library/UefiLib.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Library/UefiLib.h)  
上記URLにあるファイルの1089行目からにあった。定義は以下の通り。
```
UINTN EFIAPI Print (IN CONST CHAR16 *Format, ... );
```
ここでUINTNはデータ型の一種。spec.p.20、pdf.p.95。表2-3に以下の文面が書かれている。  
> Unsigned value of native width. (4 bytes on suppoeted 32-bit processor instruction, 8 bytes on supported 64-bit processor instructions, 16 bytes on supported 128-bit processor instructions)

native widht の意味のとり方があいまいだが、括弧書きでおおよその意味が取れると思う。ここでのnativeとは使用しているコンピュータのCPUのbit数によって変わるということを表しているという意味でそこまで深読みする必要はないと思う。
単純に、私の環境だとCPUはx86_64アーキテクチャなので UINTNは8 bytes　の符号なしの値である。しかし、整数なのか実数なのか明記されていない。推測に過ぎないが整数だと思う。その理由は、UINTN なので次の2つに分解できると思う。  
U - INT - N  
で内訳は、  
U   : Unsigned   
INT : INTeger  
N   : Number (CPUのビット数に依存して変わる値の意。)  
という認識で読んでいる。

### EFI_SUCCESS ? 
仕様書には説明はない。しかし大量に使用されている。文字通り処理が成功したときに返される。  
edk2のGithubリポジトリにあった。以下にURLを記載する。  
EFI_SUCCESS 自体のマクロは以下のURLであるが、マクロの定義がマクロを介している。  
書き方的には、　#define EFI_SUCCESS  RETURN_SUCCESS　といった状態。  
[https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Uefi/UefiBaseType.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Uefi/UefiBaseType.h)  
RETURN_SUCCESS　のマクロ定義は以下のURL。値は 0 として定義している。  
[https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Base.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Base.h)  

結論としては値は0。

### 疑問
なぜUEFI仕様書の中に、edk2のヘッダファイルで定義されている関数やマクロが使用されているのだろうか？  
edk2のヘッダファイル内の関数やマクロは独自のものではなくある程度共通の記述によるものなのだろうか？  
これに回答するわけではないが、仕様書とedk2について少しだけ言及。  
仕様書はUEFIはこうあるべきという、UEFIの目指すべき将来像を厳密に詳細に書いたもの。  
edk2はあくまでもSDK、UEFIアプリを便利に作るためのツール。ライブラリを提供してくれるが、そのライブラリが従うルールは仕様書である。  
という感じ？なので使用する関数などがすべて厳密に書かれているのはedk2が提供するライブラリなのでそちらを見るといいかもしれない。  
しかし、仕様書の方が探しやすいので先に仕様書を見るのがいいと思う。実際、Print関数のヘッダファイルは簡単に見つかったので、関数の引数と返り値は簡単にわかる。しかし、探し方が単純に悪いだけだとは思うがPrint関数の実体（Print関数の処理内容が明記されている .cファイル）は現状見つかっていない。  
->書いている途中で思ったが、仕様書に関数について書いているのだろうか？基本的な枠組みが書かれているだけなのか？  
はっきりしないので今後作業を進めるうちにわかっていけたらと思う。  
疑問を疑問のままでおいておくのは良くないことだが今後調べないといけないことが多く出てくるはずなのでときに合わせて知ることができたらラッキー程度に思っておこうと思う。  


## osbook_day02b
### Loader.infのProtocolが追加されている。目的は？  
　新しく追加されたインクルードファイルとある程度対応がとれているのでそこと関係している？  

### グローバル変数はどこで定義される？
### グローバル変数はどこで定義されているのか？ソースコードは Main.c しかないが、ビルド時にファイルが追加されるのか？  
### gBS->GetMemoryMap()がUEFIの仕様書に書かれているとするならば、グローバル変数は仕様書に書かれているのか？  
　edk2の仕様書（いろいろ種類があったけど）を見て、グローバル変数がどこで定義されるか探してみた。
結果は、どのようにファイルが作成されて最終的に.efiができるかの概要は出てきたがどこでどのように定義されるかはわからなかった。
より時間を書けて調べたら出てくるかもしれないが、1周目なのでここではもう「gで始まったらどこかで定義されたグローバル変数」と思っておこう。でないと進まない。

追記->Loader.inf　にグローバル変数と同じ名前の単語が書かれているのでここで有効にしているのだろうか？

### メモリ管理関連の機能はブート時以外も使いそうだが、なぜブートサービス関連に入っている？ランタイムサービスが提供するメモリ管理機能もあるのか？  
### edk2とUEFI仕様書の関係が謎。仕様書に従って作られるのがedk2(SDK)ではないのか？説明文にINやOUTはedk2独自のマクロとあるがどうなっているのだろうか？  
### メモリディスクリプタの大きさはUEFIの実装によってなぜ変わる？  

### EFI_FILE_PROTOCOLとは？
　spec.p.48の表2-10に書かれている。
 provide access to to supported file systems.
　プロトコルを調べると、通信手段についてばかり出てきましたが、UEFIに関するプロトコルについて以下のURLで少し言及されていました。  
> [http://yuma.ohgami.jp/UEFI-Bare-Metal-Programming/main_1.html](http://yuma.ohgami.jp/UEFI-Bare-Metal-Programming/main_1.html)    
　このページによるとプロトコルとつくのは構造体である。それぞれの構造体はメンバに関数ポインタを持っている。その関数ポインタを呼び出すことで、構造体を経由してUEFIファームウェアを利用できるようになるそうです。
　構造体の関数ポインタでファームウェアにアクセスするのでプロトコルということだろうか？

　spec.p.513にはEFI_FILE_PROTOCOLの定義（構造体の宣言）が書かれている。構造体の内容について厳密に書いているので非常に重要だと思う。

```
typedef struct _EFI_FILE_PROTOCOL {  
    UINT64  Revision;  
    EFI_FILE_OPEN Open;  
    EFI_FILE_CLOSE Close;  
    EFI_FILE_DELETE Delete;  
    EFI_FILE_READ Read;  
    EFI_FILE_WRITE Write;  
    EFI_FILE_GET_POSITION GetPosition;  
    EFI_FILE_SET_POSITION SetPosition;  
    EFI_FILE_GET_INFO GetInfo;  
    EFI_FILE_SET_INFO SetInfo;  
    EFI_FILE_OPEN_EX OpenEx;  
    EFI_FILE_READ_EX ReadEx;  
    EFI_FILE_WRITE_EX WriteEx;  
    EFI_FILE_FLUSH_EX FlushEx;  
} EFI_FILE_PROTOCOL;  
```

構造体の中に関数ポインタを持つ構造体。  
例えば、EFI_FILE_OPEN Openの文。  
関数の型名はEFI_FILE_OPEN、これはspec.p.514に書かれている。  
プロトタイプ宣言は以下の通りになる。  

```
typedef EFI_STATUS (EFIAPI *EFI_FILE_OPEN)(  
    IN EFI_FILE_PROTOCOL *This,  
    OUT EFI_FILE_PROTOCOL **NewHandle,  
    IN CHAR16 *FileName,  
    IN UINT64 OpenMode,  
    IN UINT64 Attributes  
);  
```

すべてが関数ポインタではないと思うが、少なくとも関数ポインタを含む構造体であるという認識。

　また、プロトコルについては、spec.p.46の図2-4に図として説明がある。プロトコルの構造をイメージするのに役立つかもしれない。  
　
### OpenProtocol()はどこ？
　spec.p.191に定義が書かれている。
　また、余談であるがこれを探す過程でEFI_BOOT_SERVICES構造体も発見した。spec.p.97の周辺（数ページにわたり）に定義されている。今回調べているOpenProtocol関数もこのEFI_BOOT_SERVICES構造体のメンバであることも確認できる。  
　関数の機能は、ハンドルが指定されたプロトコルをサポートしているか問い合わせる。

　関数の定義は以下の通り。  
```
typedef
EFI_STATUS
(EFIAPI *EFI_OPEN_PROTOCOL) (
    IN EFI_HANDLE   Handle,
    IN EFI_GUID    *Protocol,
    OUT VOID      **Interface OPTIONAL,
    IN EFI_HANDLE   AgentHandle,
    IN EFI_HANDLE   ControllerHandle,
    IN UINT32       Attributes
);
```

また、EFI_HANDLEとはなんだろうと思い仕様書を探した。spec.p.20の表2－3にあった。  
```
EFI_HANDLE : A Collection of related interfaces. Type VOID *.
```
また、edk2にも定義されていた。[edk2/MdePkg/Include/Uefi/UefiBaseType.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Uefi/UefiBaseType.h)  
edk2/MdePkg/Include/Uefi/UefiBaseType.h の　32行目。  
```
typedef VOID  *EFI_HANDLE;
```
となっている。

EFI_GUIDも調べる
仕様書の同じページで定義されている。仕様書では、  
```
128-bit buffer containing a unique identifier value. Unless otherwise specified, aligned on a 64-bit boundary.
```
おおよその意味としては、128bitのバッファであり、バッファ内には唯一の特定可能な値(ID)が入っている。という認識。  

edk2では[edk2/MdePkg/Include/Base.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Base.h)  の213～218行目に定義されており、  
```
typedef struct {
    UINT32  Data1;
    UINT16  Data2;
    UINT16  Data3;
    UINT8   Data4[8];
} GUID;
```
そしてこれが、[edk2/MdePkg/Include/Uefi/UefiBaseType.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Uefi/UefiBaseType.h)の24行目でEFI_GUIDとして宣言される。  
```
typedef GUID EFI_GUID;
```
みかん本ではこの項目は深く触れられていない。そのためここではあまり時間を使わないようにして次に進もうと思う。この関数で重要なのは、出力が3つ目の引数だけだということ、他は入力であるためこれ以上ここでは深入りしない。しっかりと写経する。

### OpenVolume()はどこ？
　spec.p.511に書かれている。これはEFI_SIMPLE_FILE_SYSTEM_PROTOCOLのメンバである。サマリのOpens the root directory on a volume　がしっくりこないのでvolumeを調べる。
　調べようと思ったら下のDescriptionに書いていた。どうやらボリュームを開いて、そのボリューム内のルートディレクトリを返すだけの関数らしい。英語の意味を取り違えていた。確かに読み直すとあくまでもOpens the root directoryと書いているのでルートを開くだけだった。

OpenVolumeの定義。  
```
typedef
EFI_STATUS
(EFIAPI *EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
    IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *This,
    OUT EFI_FILE_PROTOCOL               **Root
);

引数はそれぞれ次の通り。
This : A pointer to the volume to open the root directory of.
Root : A pointer to the location to return the opend file handle for the root directory. 
```
なお、EFI_FILE_PROTOCOLはspec.p.512に定義されている。ここまで書くと深入りしすぎになるので書かないことにする。  
　
### エントリポイント第一引数。　
　エントリポイントは以下のように書かれる。
    ```
    EFI_STATUS EFIAPI UefiMain(
        EFI_HANDLE image_handle,
        EFI_SYSTEM_TABLE* system_table
    )
    ```

エントリポイントには2つの引数があることは前回わかっているが、内容については一切触れていない。  
このサンプルではじめて第一引数について触れていく。

前回のところからエントリポイントのメモを取り出すと以下の通り。
```
spec.p.91、pdf.p.166。
4.1. UEFI Image Entry Point
    EFI_IMAGE_ENTRY_POINT
        Summary
            This is the main entry point for a UEFI image. This entry point is the same for UEFI applications and UEFI drivers.
        
        Prototype
            typedef
            EFI_STATUS
            (EFIAPI *EFI_IMAGE_ENTRY_POINT)(
                IN EFI_HANDLE ImageHandle,
                IN EFI_SYSTEM_TABLE *SystemTable
            );

        Parameters
            ImageHandle     The firmware allocated handle for the UEFI image. 
            SystemTable     A pointer to the EFI System Table.
```
ここで出てくるのは、UEFI imageというもの。これについては、spec.p.15に書かれている。  
全文を書くと長いので初めのほうを要約すると、UEFI imageは実行可能であることを示した情報が記述された、実行可能なコードを含むもの。（的確ではないかもしれないが、そこまで深く考えなくてもいい？）  
ちなみに、イメージファイル自体の説明ができなかったので調べたところ、ディスクの先頭から末尾までデータをそのままファイルにしたものということらしい。  

参考は以下の通り。
> - [イメージファイルとは何か](https://pctrouble.net/knowledge/file_diskimage.html)  
> - [ディスクイメージ 【disk image】 イメージファイル / image file](https://e-words.jp/w/%E3%83%87%E3%82%A3%E3%82%B9%E3%82%AF%E3%82%A4%E3%83%A1%E3%83%BC%E3%82%B8.html)  

感覚でいうと、OSを含めディスク上のすべてのデータを記録したファイル。つまり、別のパソコン本体のコピーであると思えばいいだろうか？

意味があいまいなので意味を調べた。  
The firmware allocated handle for the UEFI image.   
-> UEFIイメージに割り当てられたファームフェアのハンドル。
ちなみにハンドルがイメージしか持っていないので調べると。ポインタのポインタのこと、または対象を特定する唯一の番号のことらしい。  
参考は以下の通り。
> - [ハンドル 【handle】](https://e-words.jp/w/%E3%83%8F%E3%83%B3%E3%83%89%E3%83%AB.html#:~:text=%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0%E3%81%AE%E5%88%86%E9%87%8E%E3%81%A7%E3%81%AF%E3%80%81%E3%83%9D%E3%82%A4%E3%83%B3%E3%82%BF,%E3%83%8F%E3%83%B3%E3%83%89%E3%83%AB%E3%81%A8%E3%81%84%E3%81%86%E3%81%93%E3%81%A8%E3%81%8C%E3%81%82%E3%82%8B%E3%80%82)

または、ハンドルはC言語においては疑似的にオブジェクト指向を実現する方法であるそうだ。参考は以下の通り。
> - [C言語 オブジェクト指向【ハンドルから学ぶオブジェクトの概念】](https://monozukuri-c.com/langc-oop/)  
このサイトによれば、C言語にとってハンドルとはオブジェクト指向言語のオブジェクトである、ということらしい。  
(オブジェクト指向言語にとってハンドルとは何？となるがとりあえずおいておく)  

そのため仕様書の文章は、  
「UEFIイメージに割り当てられたファームフェアのポインタのポインタ」  
「UEFIイメージに割り当てられたファームフェアの擬オブジェクト」　　　ということになる。

すなわち、エントリポイントの第一引数 EFI_HANDLE ImageHandle とは、  
「UEFIイメージ（おそらくここでは作成するOS本体のことだと推測される）が各種ハードウェアをファームウェアを通して制御するための、ファームウェアのハンドル、すなわちファームウェアの擬オブジェクトを割り当てたもの。」  
という理解が現状読み取ることができる最大のものだと思う。しかし、現状でもいまいちファームフェアのハンドルというのがあまりつかめない。とりあえず、ハンドルが擬オブジェクトであるということであればハードウェアがオブジェクトとして抽象化され、プログラム側から制御しやすくしたものと思うか、ファームウェアの関数に対するポインタであるという理解をしている。実際にそれを確かめていないのではっきりわからない。しかし、現時点では上記の理解で次に進むのがいいと思うので今後調べていきたい。  
というよりこの認識で正しいかわからないので進めながら確かめるべき。

現状の結論：ハンドル　== 擬オブジェクト(C言語なので)  
現状の結論：第一引数　== 自作OSがハードウェアを制御するために、ファームウェアを擬オブジェクトとして認識するためのもの。  

### エントリポイント第二引数
　第一引数を調べたなら同時に第二引数も調べようと思った(正確にはosbook_day02aで調べたが、エントリポイントの引数をまとめて書いておいたほうがいいと思ったのでわかっている範囲で再度記す)。第二引数は上記より以下のように説明されている。

A pointer to the EFI System Table.

ここで、EFI_SYSTEM_TABLEはUEFIの仕様書に定義されている。
- [spec.p.62 Table 2-11 Required UEFI Implementation Elements](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_9_2021_03_18.pdf)
- spec.p.94 に構造体の定義。  
- サマリは「Contains pointers to the runtime and boot services tables」、と書かれている。意味的には「ランタイムサービスとブートサービスのテーブルへのポインタを含むもの」。
```
typedef struct {
    EFI_TABLE_HEADER                  Hdr;
    CHAR16                           *FirmwareVendor;
    UINT32                            FirmwareRevision;
    EFI_HANDLE                        ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *ConIn;
    EFI_HANDLE                        ConsoleOutHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL   *ConOut;
    EFI_HANDLE                        StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROCOTOL  *StdErr;
    EFI_RUNTIME_SERVICES             *RuntimeServices;
    EFI_BOOT_SERVICES                *BootServices;
    UINTN                             NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE          *ConfigurationTable;
} EFI_SYSTEM_TABLE;
```

この構造体（中身が入っているかどうかわからないが）に対するポインタを引数として持っている。  
しかし、現状これをどう使うかはわからない。進めていくと出てくると思うのでそれまで待ったほうが効率がいいと思うので放置する。  

## UEFIの仕様書の関数はなぜ関数ポインタで定義されている？
　いろいろと仕様書を読んでいく内に感じたが、なぜか関数の定義がだいたい次の通り。  
```
typedef EFI_STATUS (EFIAPI *関数名)(引数リスト)  
```
　なぜだろうか？

追記-> 関数ポインタで宣言されているは間違い。関数ポインタ型を宣言しているが正しい。  
あくまで宣言しているのは型名であるということに注意したい。  