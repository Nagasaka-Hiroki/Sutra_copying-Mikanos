#include<cstdint>

extern "C" void KernelMain(uint64_t frame_buffer_base,/*frame_buffer_base自体にアドレスが格納されている*/
                           uint64_t frame_buffer_size) {
    //ポインタ変数にドレスを代入する。
    uint8_t* frame_buffer = reinterpret_cast<uint8_t*>(frame_buffer_base);
    //色を塗る。
    for(uint64_t i = 0; i < frame_buffer_size; ++i){
        frame_buffer[i] = i % 256;
    }
    while(1) __asm__("hlt");
}