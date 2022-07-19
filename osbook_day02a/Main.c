#include<Uefi.h>
#include<Library/UefiLib.h>

EFI_STATUS EFIAPI UefiMain(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTabel
    )
{
    Print(L"Hello,Mikan world!\n");
    while(1);
    return EFI_SUCCESS;
}

/*以下メモ
情報源はUEFIの仕様書。リンクは以下の通り。以降、仕様書と略す。
https://uefi.org/sites/default/files/resources/UEFI_Spec_2_9_2021_03_18.pdf 
また、以降仕様書のページ数とpdfのページ数を書くが、それぞれspec.p.とpdf.p.と略すことにする。
英語になれるためにちょっとだけ英語で書く。

・EFI_STATUS : 
spec.p.20、pdf.p.95。表2-3より、
データ型の一種でStatus code。型としてはUINTN。

・EFIAPI :
spec.p.19、pdf.p.94。2.3 Calling Conventions
Calling Conventionsは呼び出し規約のこと。EFIAPIが仕様書ではじめて出てきたところの文章は以下のようになっている。
以下仕様書より引用。
 In all cases, all pointers to UEFI functions are cast with the word EFIAPI. This allows the compiler for each architecture to supply the proper compiler keywords to achieve the needed calling conventions. 

意味的には、「すべての、UEFI関数へのポインタは EFIAPIでキャストされる。これによって、各アーキテクチャのコンパイラは必要な呼び出し規約を実現するために適切なコンパイラキーワードを提供できる」という感じ。
呼び出し規約は以前WinAPIを学習しているときに出てきた。関数を呼び出すときに引数などをスタック領域に積んでいく必要があるが、その時のルールを表していた。この文章的にはハードウェアの種類によって適切な積み方が違うからとりあえずC言語のコードではEFIAPIでキャストしておいて、内容は各コンパイラが環境に合わせておいてねという感じだろうか？費用対効果的にこれ以上調べる価値がなさそうなのでここまでにする。
とりあえずEFIAPIを見たら、WinAPIの __stdcallの仲間だと思うことにする。

たまたま見つけたので以下もメモ。
spec.p.21、pdf.p.96。表2-4より。
EFIAPI : Define the calling convention for UEFI interfaces.

・UefiMain :　Loader.infで定義したエントリポイント。

・EFI_HANDLE :
spec.p.20、pdf.p.95。表2-3より、引用。
A collection of realated interfaces. Type VOID*.

ちなみにVOIDは以下の通り。同様に引用。
Undecleared type.

多分、普通のc言語のvoid型と同じだと思われる。なので、EFI_HANDLEはいわゆるvoidポインタというやつだと推測される。

EFI_SYSTEM_TABLE :
spec.p.62、pdf.p.137。表2-11より。 EFI_SYSTEM_TABLEは
Provide acsess to UEFI Boot Services, UEFI Runtime Services, consoles, firmware vendor infomation, and the system congigration tabeles.
となっている。
spec.p.93、pdf.p.168。 4.3. EFI System Table
ここでは、具体的な内容、EFI_SYSTEM_TABLE構造体の中身について言及している。
初期の段階であるのであまり踏み込んで調べないが、読んでいるとシステムで使われる様々なサービス、例えばコンソールやブートサービスなど、のハンドルやサービスに関する入出力のポインタ（ここは少しあいまい、誤りの可能性大）が集められているといった印象。
書いた後で気づいたが、そのことは上記の引用文と言っていることはほどんど同じの気がする。なぜなら、Provide acsess to ~であるから。ブートサービスやランタイムサービス、コンソールなどへのアクセスを提供するという文言的には正しく見える。
そのため、EFI_SYSTEM_TABLEで宣言された変数は、UEFIが提供するいろんなサービスへアクセスできるものだと考えて進める。
（間違っていたら戻ってくる）

・EFI_STATUS EFIAPI UefiMain( EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) : 
なぜ2つの変数を引数として持っているのか？
spec.p.24、pdf.p.99。図2-2の説明文に以下のことが書かれている。
Figure 2-2 shows the stack after AddressOfEntryPoint in the image's PE32+ header has been called on supported 32-bit systems. All UEFI image entry points take two parameters. These are the image handle to the UEFI image, and a pointer to the EFI System Table.

この図はあくまでIA-32の説明だが、文中にはすべてのアーキテクチャに共通することを述べている。
すべての UEFI image entry pointsは2つのパラメータをとる、と書かれている。
そして、それがimage handle to the UEFI image　と　a pointer to the EFI System Tableということである。

同様のことが以下にも記述されている。
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

つまり、UEFI application や　UEFI imageのエントリポイントはEFI_IMAGE_ENTRY_POINTでイメージハンドルとEFI System Tableへのポインタを引数として持つようにプロトタイプ宣言されているということだと思う。
また推測であるが、プロトタイプ宣言で型名のない（具体的にエントリポイント名が決まっていないということ）エントリポイント関数に名前をつけるという操作をしている、といった認識。これは別のc言語の本、独習Cのp.299に書かれている。この本によればtypedefを使うと関数ポインタのように型名をを持たない型に対して名前を付けるととができる。書き方は以下の通り。

typedef 関数の戻り値の型　(*適当な関数名)(適当な関数名の関数が持つ引数リスト)

といったように書く。こうすることで適当な関数名といった型名を作ることができる。（具体的には関数ポインタを返り値として持つ関数の型名などに使える）
これを今回の内容に当てはめると以下の通りになる。

typedef EFI_STATUS (EFIAPI *EFI_IMAGE_ENTRY_POINT)(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
こう書くことでEFI_IMAGE_ENTRY_POINTという型名をつけて使うことができる。
なので、これも推測に過ぎないが、エントリポイントをLoader.infで定めるのはこういったようにエントリポイント名が関数名で決まっているのではなく、あくまで型名として決まっていて実体を定義していない（コンソールアプリケーションでは具体的にmain()、WinAPIではMinMain()であると決まっている）からではないかと思う。

読み返すと結構滅茶苦茶な文章を書いているがなんとなく趣旨は書けているので良いものとする。
以上がエントリポイントに関する認識である。間違っているところもあると思うが、とりあえず進めていく。

・Print : 
仕様書にPrintの説明がない。仕様書には一応Print文が使われているところはある。例えば以下のページ。
spec.p.425、pdf.p.500。しかし、Printに関する説明はない。
疑問に思って、このファイルのヘッダファイルのインクルードなしにコンパイルしたが、コンパイルに失敗した。
しかし、上記のコードでは特にヘッダファイルなしに記述されている。コードのすべてが表示されているわけではないのかもしれないが不思議である。
edk2のリポジトリを確認したら発見できた。場所は次。
https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Library/UefiLib.h
上記URLにあるファイルの1089行目からにあった。定義は以下の通り。

UINTN EFIAPI Print (IN CONST CHAR16 *Format, ... );

ここでUINTNはデータ型の一種。spec.p.20、pdf.p.95。表2-3に以下の文面が書かれている。
Unsigned value of native width. (4 bytes on suppoeted 32-bit processor instruction, 8 bytes on supported 64-bit processor instructions, 16 bytes on supported 128-bit processor instructions)

native widht の意味のとり方があいまいだが、括弧書きでおおよその意味が取れると思う。ここでのnativeとは使用しているコンピュータのCPUのbit数によって変わるということを表しているという意味でそこまで深読みする必要はないと思う。
単純に、私の環境だとCPUはx86_64アーキテクチャなので UINTNは8 bytes　の符号なしの値である。しかし、整数なのか実数なのか明記されていない。推測に過ぎないが整数だと思う。その理由は、 UINTN なので次の2つに分解できると思う。
U - INT - N
で内訳は、
U   : Unsigned 
INT : INTeger
N   : Number (CPUのビット数に依存して変わる値の意。)
という認識で読んでいる。

・EFI_SUCCESS : 
仕様書には説明はない。しかし大量に使用されている。文字通り処理が成功したときに返される。
edk2のGithubリポジトリにあった。以下にURLを記載する。
EFI_SUCCESS 自体のマクロは以下のURLであるが、マクロの定義がマクロを介している。
書き方的には、　#define EFI_SUCCESS  RETURN_SUCCESS　といった状態。
https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Uefi/UefiBaseType.h
RETURN_SUCCESS　のマクロ定義は以下のURL。値は 0 として定義している。
https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Base.h

結論としては値は0。

・疑問
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

・ひとまず終了。
Hello world　だけであったがかなり知識がないと読み解けない内容だった。
とりあえず初めだったのである程度しっかり調べたが、進み具合によっては調整しなければならないと思った。
*/