SRCDIR := src
BUILDDIR := build
CFLAGS = -c -mcmodel=large -fno-rtti -ffreestanding

x86_64_asm_source_files := $(shell find $(SRCDIR) -name *.asm)
x86_64_asm_object_files := $(patsubst $(SRCDIR)/%.asm, $(BUILDDIR)/x86_64/%.o, $(x86_64_asm_source_files))

kernel_source_files := $(shell find $(SRCDIR)/kernel -name *.cpp | grep -v $(SRCDIR)/kernel/interrupt_stubs.cpp)
kernel_object_files := $(patsubst $(SRCDIR)/kernel/%.cpp, $(BUILDDIR)/kernel/%.o, $(kernel_source_files))

x86_64_cpp_source_files := $(shell find src -name *.cpp | grep -v $(SRCDIR)/kernel/interrupt_stubs.cpp)
x86_64_cpp_object_files := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/x86_64/%.o, $(x86_64_cpp_source_files))

font_object_files := $(shell find $(BUILDDIR)/x86_64/interface/graphics -name *.font)

x86_64_object_files := $(x86_64_cpp_object_files) $(x86_64_asm_object_files) $(BUILDDIR)/x86_64/kernel/interrupt_stubs.o $(font_object_files)

$(BUILDDIR)/x86_64/kernel/interrupt_stubs.o: $(SRCDIR)/kernel/interrupt_stubs.cpp include/kernel/interrupt_stubs.h
	mkdir -p $(dir $@) && \
	g++ $(CFLAGS) -fpermissive -mno-red-zone -mgeneral-regs-only -I include $(SRCDIR)/kernel/interrupt_stubs.cpp -o $@

$(kernel_object_files): $(BUILDDIR)/kernel/%.o : $(SRCDIR)/kernel/%.cpp include/*.h
	mkdir -p $(dir $@) && \
	g++ $(CFLAGS) -I include $(patsubst $(BUILDDIR)/kernel/%.o, $(SRCDIR)/kernel/%.cpp, $@) -o $@

$(x86_64_cpp_object_files): $(BUILDDIR)/x86_64/%.o : $(SRCDIR)/%.cpp include/*.h
	mkdir -p $(dir $@) && \
	g++ $(CFLAGS) -I include $(patsubst $(BUILDDIR)/x86_64/%.o, $(SRCDIR)/%.cpp, $@) -o $@

$(x86_64_asm_object_files): $(BUILDDIR)/x86_64/%.o : $(SRCDIR)/%.asm
	mkdir -p $(dir $@) && \
	nasm  -f elf64 $(patsubst $(BUILDDIR)/x86_64/%.o, $(SRCDIR)/%.asm, $@) -o $@

.PHONY: build-x86_64
build-x86_64: $(x86_64_object_files)
	mkdir -p dist/x86_64 && \
	ld -n --no-relax -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(x86_64_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso