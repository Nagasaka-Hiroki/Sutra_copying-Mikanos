target = kernel.elf
objs = main.o

cxxflags += -O2 -Wall -g --target=x86_64-elf --ffreestanding -mno-red-zon \
			-fno-exceptions -fno-rtti -std=c++17

ldflags  += --entry KernelMain -z norelro --image-base 0x100000 --static

.phony: all
	all: $(target)

.phony: clean
clean:
	rm -rf *.o

kernel.elf: $(objs) Makefile
	ld.lld $(ldflags) -o kernel.elf $(objs)

%.o: %.cpp Makefile
	clang++ $(cppflags) $(cxxflags) -c $<