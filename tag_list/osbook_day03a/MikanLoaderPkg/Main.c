#include<Uefi.h>
#include<Library/UefiLib.h>
#include<Library/UefiBootServicesTableLib.h>
#include<Library/PrintLib.h>
#include<Protocol/LoadedImage.h>
#include<Protocol/SimpleFileSystem.h>
#include<Protocol/DiskIo2.h>
#include<Protocol/BlockIo.h>
#include<Guid/FileInfo.h>

struct MemoryMap {
    UINTN buffer_size;
    VOID* buffer;
    UINTN map_size;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
};

EFI_STATUS GetMemoryMap(struct MemoryMap *map);
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root);
EFI_STATUS SaveMemoryMap(struct MemoryMap* map, EFI_FILE_PROTOCOL *file);
const CHAR16* GetMemoryTypeUnicode(EFI_MEMORY_TYPE type);

EFI_STATUS EFIAPI UefiMain(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table
)
{
//  過去に記述
    Print(L"Hello,Mikan world!\n");
    CHAR8 memmap_buff[4096 * 4];
    struct MemoryMap memmap = {sizeof(memmap_buff),memmap_buff,0,0,0,0};
    GetMemoryMap(&memmap);
    EFI_FILE_PROTOCOL *root_dir;
    OpenRootDir(image_handle,&root_dir);
    EFI_FILE_PROTOCOL *memmap_file;
    root_dir->Open(
        root_dir,&memmap_file,L"\\memmap",
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,0
    );
    SaveMemoryMap(&memmap, memmap_file);
    memmap_file->Close(memmap_file);
//  ここまで
//  ここから新しく記述。
//  カーネルを読み込む。
    EFI_FILE_PROTOCOL* kernel_file;
    root_dir->Open(
        root_dir,&kernel_file,L"\\kernel.elf",
        EFI_FILE_MODE_READ,0
    );
    //EFI_FILE_INFOは構造体。構造体を代入できる大きさの配列をVLAとして宣言する。
    UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 file_info_buffer[file_info_size];
    //配列を容易で来たら、ファイル情報を取得する。
    kernel_file->GetInfo(
        kernel_file, &gEfiFileInfoGuid,
        &file_info_size, file_info_buffer
    );//これでファイル情報が file_info_buffer に保存される。
    //file_info_bufferはファイル情報が入っているが、UINT8型なのでデータが変な見え方になる。
    //UINT8　から　EFI_FILE_INFO　にキャストする。
    EFI_FILE_INFO* file_info = (EFI_FILE_INFO*)file_info_buffer;
    //これで kernel.elf のファイル情報をメモリに書き込み、データの先頭アドレスを持った、 file_info ができた。
    //file_info は EFI_FILE_INFO構造体へのポインタなので、当然アロー演算子でメンバにアクセスできる。
    // kernel.elf をメモリにロードするために、ロードする大きさを取得する。
    UINTN kernel_file_size = file_info->FileSize;
    //カーネルをロードする位置はリンカーオプションで指定しているのでその位置を書く。
    EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;
    //指定したアドレスにロードする。
    gBS->AllocatePages(
        AllocateAddress,/*AllocateAddressを指定すれば指定アドレスにロードする。*/
        EfiLoaderData,/*ブートローダが使うときはEfiLoaderDataでいいらしい。そういうものと思っておく*/
        (kernel_file_size + 0xfff) / 0x1000, /*0xfffを足すのは、kernel_file_size==0x1000のとき+0xfffしても計算結果は同じになるから。*/
        &kernel_base_addr/*指定のアドレス。*/
    );//メモリ確保完了。早速カーネルを読み込む。
    //カーネルの読み込み。
    kernel_file->Read(kernel_file,&kernel_file_size,(VOID*)kernel_base_addr);
    
    Print(L"Kerneel: 0x%0lx (%lu bytes)\n",kernel_base_addr,kernel_file_size);
//  カーネル読み込み完了。
//  カーネルを起動する前にブートサービスを停止する。
    EFI_STATUS status;
    //ExitBootServicesは現状そういうものと思うしかない。
    //メモリマップキーが変化していると失敗する。ゆえに、実行する直前にメモリマップキーを取得するのがいい？
    //メモリマップキーの変化はブートサービスの利用で変化するらしい。
    status = gBS->ExitBootServices(image_handle,memmap.map_key);//ほとんど失敗するなのでif文を使う。
    if(EFI_ERROR(status)){/*戻り値 statusで失敗かどうかわかる。*/
        //メモリマップを再取得してマップキーを更新する。
        status = GetMemoryMap(&memmap);
        //取得に失敗するとエラーを出力する。
        if(EFI_ERROR(status)){
            Print(L"failed to get memory map: %r\n",status);
            while(1);
        }
        //再度ブートサービスを停止する。
        status = gBS->ExitBootServices(image_handle,memmap.map_key);
        if(EFI_ERROR(status)){
            Print(L"Could not exit boot service: %r\n",status);
            while(1);
        }
    }
//  ブートサービス停止完了。
//  カーネルを呼び出す。
    //アドレスは1つあたり1バイト==8bit。エントリポイントのアドレスは8バイトの整数、オフセットは24バイトなので
    UINT64 entry_addr = *(UINT64*)(kernel_base_addr + 24);
    //EntryPointTypeという返り値がvoidで引数がvoidの関数型を宣言する。
    typedef void EntryPointType(void);//EntryPointTypeは関数型のプロトタイプ宣言。
    //これで関数の型（プロトタイプ宣言）と引数と返り値の型を明らかにしたのでEntryPointTypeにキャストすれば関数が使えるようになった。
    //->Main.cがC言語のファイルなのでC言語の関数として呼び出すことができるようにしなければならない。
    //kernel.elfの中からエントリポイント関数のアドレスを格納したentry_addrをEntryPointType型の関数ポインタにキャストする。
    EntryPointType* entry_point = (EntryPointType*) entry_addr;
    entry_point();
//  カーネルの呼び出し完了。
    Print(L"All done\n");
    while(1);
    return EFI_SUCCESS;
}

EFI_STATUS GetMemoryMap(struct MemoryMap *map){
    if(map->buffer == NULL) {
        return EFI_BUFFER_TOO_SMALL;
    }
    
    map->map_size = map->buffer_size;
    
    return gBS->GetMemoryMap(
        &map->map_size,
        (EFI_MEMORY_DESCRIPTOR*)map->buffer,
        &map->map_key,
        &map->descriptor_size,
        &map->descriptor_version
    );
}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle,EFI_FILE_PROTOCOL **root){
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

    gBS->OpenProtocol(
        image_handle,
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&loaded_image,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );

    gBS->OpenProtocol(
        loaded_image->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID**)&fs,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
    );
    
    fs->OpenVolume(fs,root);
    return EFI_SUCCESS;
}

EFI_STATUS SaveMemoryMap(struct MemoryMap *map, EFI_FILE_PROTOCOL *file) {
    CHAR8 buf[256];
    UINTN len;
    
    CHAR8* header = "Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute\n";
    len = AsciiStrLen(header);
    file->Write(file,&len,header);

    Print(L"map->buffer = %08lx, map->map_size = %08lx\n",map->buffer,map->map_size);
    
    EFI_PHYSICAL_ADDRESS iter;
    int i;
    for(iter = (EFI_PHYSICAL_ADDRESS)map->buffer,i=0;
        iter < (EFI_PHYSICAL_ADDRESS)map->buffer + map->map_size;
        iter += map->descriptor_size, i++) {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR*)iter;
        len = AsciiSPrint(
            buf,sizeof(buf),
            "%u, %x, %-ls, %08lx, %lx, %lx\n",
            i, desc->Type, GetMemoryTypeUnicode(desc->Type),
            desc->PhysicalStart, desc->NumberOfPages,
            desc->Attribute & 0xffffflu
        );
        file->Write(file,&len,buf);
    }
    return EFI_SUCCESS;
}

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