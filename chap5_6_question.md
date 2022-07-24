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