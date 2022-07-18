#include<Uefi.h>
#include<Library/UefiLib.h>
#include<Library/UefiBootServicesTableLib.h>
#include<Library/PrintLib.h>
#include<Protocol/LoadedImage.h>
#include<Protocol/SimpleFileSystem.h>
#include<Protocol/DiskIo2.h>
#include<Protocol/BlockIo.h>

//メモリーマップ構造体の定義
struct MemoryMap {
    UINTN buffer_size;
    VOID* buffer;//voidポインタで指すのは後々変更があるのだろうか？
    UINTN map_size;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
};
//typedef struct MemoryMap MemoryMap　としたほうがいいだろうか？

//プロトタイプ宣言
//メイン関数をある程度書かないとどういう呼び出し方をして、関数内部の処理の内容が想像しにくいのでメインを先に書く。
EFI_STATUS GetMemoryMap(struct MemoryMap *map);//メモリマップを取得して、引数に渡す。
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root);//ルートディレクトリを開く。
EFI_STATUS SaveMemoryMap(struct MemoryMap* map, EFI_FILE_PROTOCOL *file);//メモリーマップをファイルに書き込む。
const CHAR16* GetMemoryTypeUnicode(EFI_MEMORY_TYPE type);//メモリディスクリプタのタイプ値をタイプ名に変換する。
//エントリポイント
EFI_STATUS EFIAPI UefiMain(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table
)
{
    Print(L"Hello,Mikan world!\n");
    //メモリマップを取得する。
    CHAR8 memmap_buff[4096 * 4];
    struct MemoryMap memmap = {sizeof(memmap_buff),memmap_buff,0,0,0,0};
    //プロトタイプ宣言の通り。
    //memmapのアドレスを引数に渡して、GetMemoryMapに渡す。
    GetMemoryMap(&memmap);
    //ルートディレクトリを開く。
    EFI_FILE_PROTOCOL *root_dir;//EFI_FILE_PROTOCOL構造体へのポインタ。構造体の詳細はspec.p.513にある。下にメモをする。
    OpenRootDir(image_handle,&root_dir);//はじめてImageHandle(エントリポイントの第一引数)を使った。
    //ここから調べる量を減らす。関数一つあたりに調べる量が多すぎるので、そういうものと思い込むラインを緩める。
    //そういうものというものに対しては明確に書く。（認識やそういうものとかそういう書き方をする。）
    //ルートディレクトリが開けた。ここからファイルを作って書き込んでいく。
    EFI_FILE_PROTOCOL *memmap_file;//EFI_FILE_PROTOCOLを使ってファイル構造体を宣言するという認識。
    root_dir->Open(
        root_dir,&memmap_file,L"\\memmap",
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,0
    );//これで書き込みモードでルートディレクトリにmemmapが開く。
    //書き込むデータはすでに取得済み、書き込み先も開いた。->早速書き込んでいく。
    SaveMemoryMap(&memmap, memmap_file);
    //いつも通り、ファイル構造体は最後は閉じる。
    memmap_file->Close(memmap_file);
    //終了をしたことを画面に出力する。
    Print(L"All done\n");
    while(1);
    return EFI_SUCCESS;
}

//プロトタイプの中身を書いていく。
//メモリパップを取得する関数を定義して、その中でgBS->GetMemoryMapを呼び出す。
EFI_STATUS GetMemoryMap(struct MemoryMap *map){
    //* mapの配列の中身は、UefiMainで定めた通り。
    //     = {sizeof(memmap_buff),memmap_buff,0,0,0,0};
    //大きい配列なので確保できているか確認。動的確保と同じやり方という認識だが大丈夫か？
    if(map->buffer == NULL) {
        return EFI_BUFFER_TOO_SMALL;
    }
    //if文でリターンされなければ、配列は確保されている。以下でメモリマップを確保していく。
    //map_sizeは入力時は書き込み用メモリ領域の大きさなので、buffer_size == sizeof(memmap_buff)を代入。
    map->map_size = map->buffer_size;
    //return文でgBS->MemoryMapを実行して、EFI_STATUSを返す。
    return gBS->GetMemoryMap(
        &map->map_size,/*入力時は書き込み用メモリ領域の大きさ、実行後は実際のメモリマップの大きさが入る*/
        (EFI_MEMORY_DESCRIPTOR*)map->buffer,/*bufferはvoidポインタなのでEFI_MEMORY_DESCRIPTOR型のポインタにキャスト*/
        &map->map_key,/*gBS->ExtitBootServices()を呼び出す時に使うらしい*/
        &map->descriptor_size,/*メモリディスクリプタの大きさが返ってくる*/
        &map->descriptor_version/*これはみかん本では使わないらしい*/
    );
}
//ルートディレクトリを開く。
//引数はエントリポイントで受け取ったimage_handleと出力用にルートディレクトリを返す**root
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle,EFI_FILE_PROTOCOL **root){
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;//spec.p.47,288。構造体の定義はspec.p.288
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;//spec.p.510に構造体の定義がある。
    //処理の流れは、ブートサービスからOpenProtocolを使って連鎖的に開いていく。
    //まず、すでに開いたハンドル image_handleを使ってloaded_imageを開く。
    gBS->OpenProtocol(
        image_handle,
        &gEfiLoadedImageProtocolGuid,/*Loader.infに追加したものに近いが何か関係があるのだろうか*/
        (VOID**)&loaded_image,/*ここが重要。OpenPrtocolの出力に相当する。*/
        image_handle,/*AgentHandleとは一体なんだろう。肝ではなさそうなので飛ばす*/
        NULL,/*ControllerHandle、ここはAgentHandleがUEFI Dirver ModelじゃないならNULLにするらしい。よくわからん*/
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL/*開き方の指定*/
    );
    //loaded_imageが開いた？と思うのでfsを開く。
    gBS->OpenProtocol(
        loaded_image->DeviceHandle,/*イメージの場所を表す。*/
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID**)&fs,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );
    //これでfsが使えるようになったのでルートディレクトリを開く。
    fs->OpenVolume(fs,root);
    return EFI_SUCCESS;
    //この関数SUCCESSしか返さないが大丈夫だろうか？OpenProtocolが必ず成功すると定められているならいいのだが
}
//メモリーマップを保存していく。
EFI_STATUS SaveMemoryMap(struct MemoryMap *map, EFI_FILE_PROTOCOL *file) {
    CHAR8 buf[256];
    UINTN len;
    //csvファイルの先頭行を見出しとして使う。
    CHAR8* header = "Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute\n";
    //文字列の長さを計算。
    len = AsciiStrLen(header);
    //書き込む。
    file->Write(file,&len,header);
    //画面に出力。
    Print(L"map->buffer = %08lx, map->map_size = %08lx\n",map->buffer,map->map_size);
    //イテレータを宣言。
    EFI_PHYSICAL_ADDRESS iter;
    int i;
    for(iter = (EFI_PHYSICAL_ADDRESS)map->buffer,i=0;/*iterに配列の先頭アドレスを代入、*/
        iter < (EFI_PHYSICAL_ADDRESS)map->buffer + map->map_size;/*iterがマップサイズの範囲を越えなければ繰り返し。*/
        iter += map->descriptor_size, i++) {/*メモリディスクリプタの大きさだけ移動する。*/
        //メモリディスクリプタ型のポインタに再度キャストする。
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR*)iter;
        len = AsciiSPrint(/*write関数の説明文中に返り値が文字列のバイト数になるらしい。*/
            buf,sizeof(buf),/*AsciiPrintは引数に指定した配列（buf)にフォーマットで指定した形に成形された文字列が入る。*/
            "%u, %x, %-ls, %08lx, %lx, %lx\n",/*フォーマット*/
            i, desc->Type, GetMemoryTypeUnicode(desc->Type),/*GetMemoryTypeUnicodeはこのファイル内で定義*/
            desc->PhysicalStart, desc->NumberOfPages,
            desc->Attribute & 0xffffflu
        );//これでbufに文字列、lenに文字列のバイト数が格納される。
        file->Write(file,&len,buf);
    }
    return EFI_SUCCESS;
}
//メモリディスクリプタのタイプ値とタイプ名の対応をとる。
const CHAR16* GetMemoryTypeUnicode(EFI_MEMORY_TYPE type){
    switch (type){
        case EfiReservedMemoryType      : return L"EfiReservedMemoryType";
        case EfiLoaderCode              : return L"EfiLoaderCode";
        case EfiLoaderData              : return L"EfiLoaderData";
        case EfiBootServicesCode        : return L"EfiBootServicesCode";
        case EfiBootServicesData        : return L"EfiBootServicesData";
        case EfiRuntimeServicesCode     : return L"EfiRuntimeServicesCode";
        case EfiRuntimeServicesData     : return L"EfiRuntimeServicesData";
        case EfiConventionalMemory      : return L"EfiConventionalMemory";
        case EfiUnusableMemory          : return L"EfiUnusableMemory";
        case EfiACPIReclaimMemory       : return L"EfiACPIReclaimMemory";
        case EfiACPIMemoryNVS           : return L"EfiACPIMemoryNVS";
        case EfiMemoryMappedIO          : return L"EfiMemoryMappedIO";
        case EfiMemoryMappedIOPortSpace : return L"EfiMemoryMappedIOPortSpace";
        case EfiPalCode                 : return L"EfiPalCode";
        case EfiPersistentMemory        : return L"EfiPersistentMemory";
        case EfiMaxMemoryType           : return L"EfiMaxMemoryType";
        default                         : return L"InvalidMemoryType";
    }
}
/*
---
みかん本 p.55~
tag: osbook_day02b
---
edk2の仕様書（いろいろ種類があったけど）を見て、グローバル変数がどこで定義されるか探してみた。
結果は、どのようにファイルが作成されて最終的に.efiができるかの概要は出てきたがどこでどのように定義されるかはわからなかった。
より時間を書けて調べたら出てくるかもしれないが、1周目なのでここではもう「gで始まったらどこかで定義されたグローバル変数」と思っておこう。でないと進まない。

・EFI_FILE_PROTOCOL
spec.p.513より引用。

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

構造体の中に関数ポインタを持つ構造体。
例えば、EFI_FILE_OPEN Openの文。
関数の型名はEFI_FILE_OPEN、これはspec.p.514に書かれている。
プロトタイプ宣言は以下の通りになる。

typedef EFI_STATUS (EFIAPI *EFI_FILE_OPEN)(
IN EFI_FILE_PROTOCOL *This,
OUT EFI_FILE_PROTOCOL **NewHandle,
IN CHAR16 *FileName,
IN UINT64 OpenMode,
IN UINT64 Attributes
);

すべてが関数ポインタではないと思うが、少なくとも関数ポインタを含む構造体であるという認識。
*/