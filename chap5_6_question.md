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