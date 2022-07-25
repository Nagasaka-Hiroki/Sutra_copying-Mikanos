# 疑問(第5章、第6章)
# 第5章
## osbook_day05a
### x80u
　末尾のuはサフィックスという。uがつくとunsigned int の意味になる。  
参考は以下の通り。  
> - [データ型](https://programming.pc-note.net/c/type.html)  
> - [「ゼロからのOS自作入門」の リベンジ Part5](https://zenn.dev/omiso/articles/5f58cc80748ad0#%E6%96%87%E5%AD%97%E3%82%92%E6%9B%B8%E3%81%8F%EF%BC%88%E7%AC%AC5%E7%AB%A0%EF%BC%89)  

また、x80u　を2進数に換算すると、　1000 0000 になる。換算は以下のサイトでした。  
> - [https://hogehoge.tk/tool/number.html](https://hogehoge.tk/tool/number.html)  

## osbook_day05b
　特に難しいところはない。ファイルを分割するだけなので。念のためコードを書きながらこれまでのコードの復習をしておく。

### 新しい.gitignore
　kernelのディレクトリに.gitignoreが追加された。現状どういったファイルができるかわからないので念ためそのまま写経する。  
```
    git rm kernel.elf
    git rm *.o
    git rm .*.d
```
を実行して.gitignoreが有効になるようにする。

### Makefile
　Makefileの内容が本格的にわからなくなってきた。本文中の内容的に本質的ではないので今は放置する。  
しかし、この段階でわからないということだけメモしておく。

### osbook_day05b終了
　特に見た目は変わらないがMakefileのファイルの変更が大きい。また、ファイルを分割する工程が入っているのでOS作りの本質的なところではないがプログラムを作るうえでは重要な操作がこの節にはあると思う。Makefileの書き方については付録があるそうなのでそちらもしっかり確認したい。  
追記：Makefileの付録は読み間違い。githubリポジトリからコードを読んでねという意味だった。こちらはおいおい調べていく。

## osbook_day05
### 東雲フォント
　ライセンスはPublic Domain ライセンス。これは著作権を放棄している状態らしい。参考は以下。  
> - [「パブリック・ドメイン」とは何か](https://mag.osdn.jp/03/06/15/1044258)  

なので特に著作権は気にしなくてもいいと思う。
とりあえずフォントのデータhankaku.txtとフォントデータをバイナリデータに変換するツールはそのままリポジトリからコピーする。  
本質的なところではないはずなので、ここは時短する。

しかし、ツールでどういった操作をしているのだろうか？コードを読んだがあまりよくわからない。
今の認識だととりあえずフォントデータ（テキストデータ）をバイナリデータに変換している程度。現状はこれでいいが自分でツールを作るとなるとここを理解しなくてはいけない。

### objcopyのつける変数名
　どうやって決まるのだろうか？それぞれの変数名の中身は何を指しているのだろうか？  
とりあえず調べたところ以下のURLがヒットした。  
> - [普通のやつらの下を行け: objcopy で実行ファイルにデータを埋め込む](http://0xcc.net/blog/archives/000076.html)  

　この記事では、今回と似たことをしている。そのためどういったことをしているかの感じはわかる。  
この記事では、画像やCSVファイルなどをバイナリ形式で扱う内容が書かれている。  

> -[ゼロからのOS自作入門 5章 osbook_day05c の objcopy のオプションメモ](https://zenn.dev/miwarin/articles/a05fddf6c1b42e)  

この記事では、ゼロからのOS自作入門の内容で私と同じくobjcopyについて調べてくれている。どうやら変数名の指定は-Bと関係があるみたい。

```
    man objcopy
```
で調べてみたら、上記URLと同じ文章が出てきた。とりあえず引用する。引用元は```man objcopy```または、[Chapter 4. objcopy](http://web.mit.edu/rhel-doc/3/rhel-binutils-en-3/objcopy.html)に似た内容？同じ内容？かわからないが重要なワードが見えたので一応リンクを貼っておく。

> - man objcopy の -B の説明文。  
-B bfdarch  
       --binary-architecture=bfdarch  
           Useful when transforming a architecture-less input file into an object file.  In
           this case the output architecture can be set to bfdarch.  This option will be
           ignored if the input file has a known bfdarch.  You can access this binary data
           inside a program by referencing the special symbols that are created by the
           conversion process.  These symbols are called _binary_objfile_start,
           _binary_objfile_end and _binary_objfile_size.  e.g. you can transform a picture file
           into an object file and then access it in your code using these symbols.

この文章によると3つの変数があるらしい。
1. _binary_objfile_start
2. _binary_objfile_end
3. _binary_objfile_size

それぞれがどういった変数なのかといった説明はすぐに見つからなかったが、とりあえずここを見たら変数名がわかるということだけメモ。

### osbook_day05c終了
　この節では、ざっくりと(1)データをバイナリに変換して指定実行ファイル形式とリンクする方法、(2) リンクされたデータを使う方法 の2つの内容が書かれていた。  
ページ数的には大したことはない。しかし内容は結構重要だと思うのでしっかり把握する必要がある。

また、今更だがmakeを実行するたびに拡張子が```.d```のファイルが作成される。中身を見てもプログラムというよりは設定内容を書いているだけのような感じ。このファイルはMakefileが作っているのでレシピを見ればいいのだが、いかんせん現状どういった効果があるかわからない。ひとまずわからないということだけメモする。わからなくても進めていく。

## osbook_day05d
### caddr_t?
以下のリンクに定義されていた。  
> [http://www.sde.cs.titech.ac.jp/~gondow/dwarf2-xml/HTML-rxref/app/gcc-3.3.2/lib/gcc-lib/sparc-sun-solaris2.8/3.3.2/include/sys/types.h.html](http://www.sde.cs.titech.ac.jp/~gondow/dwarf2-xml/HTML-rxref/app/gcc-3.3.2/lib/gcc-lib/sparc-sun-solaris2.8/3.3.2/include/sys/types.h.html)  

これによれば、  
```
    typedef char *caddr_t;
```
となっているので、```char*```だと思えばいいと思う。

### NewLib?
　標準Cライブラリの一種。参考は以下の通り。  
> - [https://www.weblio.jp/content/Newlib](https://www.weblio.jp/content/Newlib)  

標準Cライブラリの種類は以下のURLに書かれている。  
> [標準Cライブラリ](https://www.wdic.org/w/TECH/%E6%A8%99%E6%BA%96C%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9%E3%83%AA)  

上記URLの中を読むと、種類ごとにライセンスが違う。たとえばNewLibならBSDライクなライセンス、glibならLGPLライセンスといった感じらしい。しかし、読み進めると種類は違えどヘッダファイルの名前は規格で決まっているみたい。なので、それぞれの種類はライセンスによる違いや環境による違いで使い分けるといった感じだと思って今は解釈する。本文中にもNewLibは組み込み機器でも利用できるといった具合になっているので使い分けが重要？

->もし複数の種類の標準ライブラリを用いているとするならば、どこを見ればどの種類の標準ライブラリを使っているとわかるのだろうか？

### フォーマットの指定についてメモ
　今回は標準ライブラリの関数でフォーマットを守った文字列を生成している。なのでこちらが準備するのは標準ライブラリを使えるようにする関数群の準備と文字列を描画する関数である。

## osbook_day05e
### while(*s)
　確かにこういった書き方をするときれいにかけるなと思ったが、具体的にどういった動作をするか知らなかったので実際に書いてみた。  
```
#include<stdio.h>
void loop(const char* s);

int main(){
        const char arr[]="now_looping\n";
        printf("const char arr[%d]=%s",(int)(sizeof(arr)/sizeof(char)),arr);
        loop(arr);
        return 0;
}

void loop( const char* s){
        int i=0;
        while(*s){
                printf("arr[%d]=%c\n",i,*s);
                s++;
                i++;
        }
        return;
}
```
こういったコードを書いて```main.c```で保存し、```clang main.c```でコンパイルして実行する。実行結果は以下の通りで、  
```
const char arr[13]=now_looping
arr[0]=n
arr[1]=o
arr[2]=w
arr[3]=_
arr[4]=l
arr[5]=o
arr[6]=o
arr[7]=p
arr[8]=i
arr[9]=n
arr[10]=g
arr[11]=

```
配列の最後arr[11]は改行文字で改行が2つされているため最後は空白行になっている。また、while文をちゃんと抜けることができているためこの書き方は問題なく動作することが確認できる。  
正常に動作する要因は、配列の末尾arr[12]が'\0'であるからだと思われる。'\0'はAsciiコードで0(==0x00)なのでwhile文を正常に抜けることができたのだと思われる。

###  memcpyについて
　あまり詳しくないので調べた。参考は以下  
> - [https://bituse.info/c_func/56](https://bituse.info/c_func/56)  

メモリにデータをコピーする単純？な関数。これもNewLibの関数の一種だろうか？
追記：みかん本に書いてた。標準Cライブラリ関数の一種。またmemsetも同様であるそうだ。

### osbook_day05e終了
　改行文字を含む文字列の描画についての内容だった。エミュレータが起動した段階ですでに改行済みの内容が描画されているのでどういった感じで改行されるか見えないのが残念だが結果だけ見るとうまく言っているように見える。  
　今更だが、NewLibを使うために定義しなければいけない関数群はどのようにすればわかるのだろうか？今はここを理解していなくても本の内容を理解できるが自分で作るとなると困るので追々調べていきたい。

## osbook_day05f
### va_list ?
　参考は以下  
> - [va_list、可変長引数の仕組みを理解してvprintf関数を使う](https://qiita.com/kurasho/items/1f6e04ab98d70b582ab7)  

　読んでみると、処理の内容を気にしなければそれほど難しくないといった印象。（フォーマットの指定の型の大きさごとにメモリ移動量を変えるといった感じ？）

また、可変長引数の受け取りに関する処理の関数は以下に説明があった。  
> - [https://www.k-cube.co.jp/wakaba/server/func/stdarg_h.html](https://www.k-cube.co.jp/wakaba/server/func/stdarg_h.html)  

### ファイル読み込み機能？
　OSにデータをobjcopyを使ってのりしろを作ってリンクした。その理由としてみかん本ではファイル読み込み機能が現時点では足りないからとあった。しかし、メモリマップを出力する段階でテキストファイルの出力をしている。そのためUEFIの機能を使えば読み込むことはできるはずだ。どうしてその方法をしなかったのだろう？単純にobjcopyを使ってバイナリデータをリンクする方法を紹介するためなのだろうか？