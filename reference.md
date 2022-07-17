# Reference
　[UEFIの仕様書](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_9_2021_03_18.pdf)
や[edk2](https://github.com/tianocore/edk2)に関するメモ。おおよそはquestion.mdに書いているかもしれない。ここでは「これってこんなところに定義されてたんだ」とか「頻繁に参照しそうだから短くメモしよう」といったように使おうと思う。  

## [edk2](https://github.com/tianocore/edk2)
- エントリポイント(EFIAPI *EFI_IMAGE_ENTRYPOINT)　[edk2/MdePkg/Include/Uefi/UefiSpec.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Uefi/UefiSpec.h)
    - 2039行目から2055行目。edk2でもちゃんと定義が見つかって良かった。
    - しかし、このヘッダファイルの意味と関数の実体はどこにあるのだろうか？
- GetMemoryMap (EFIAPI *EFI_GET_MEMORY_MAP) [edk2/MdePkg/Include/Uefi/UefiSpec.h](https://github.com/tianocore/edk2/blob/master/MdePkg/Include/Uefi/UefiSpec.h)
    - メモリマップを取得する関数(osbook_day02b)の元の関数？。少し見え方が違うがグローバル変数の定義と同時にこの関数も定義しているのだろうか？
- EFI_SYSTEM_TABLE
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
    サマリの通り、ランタイムサービステーブルとブートサービステーブルへのポインタが含まれている。しかしどう使うかは現状不明。  

## [UEFIの仕様書](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_9_2021_03_18.pdf)
