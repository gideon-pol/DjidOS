SRCDIR := src
BUILDDIR := build
CFLAGS = -c -mcmodel=large -fno-rtti -ffreestanding

x86_64_asm_source_files := $(shell find $(SRCDIR) -name *.asm)
x86_64_asm_object_files := $(patsubst $(SRCDIR)/%.asm, $(BUILDDIR)/x86_64/%.o, $(x86_64_asm_source_files))

kernel_source_files := $(shell find $(SRCDIR)/kernel -name *.cpp | grep -v $(SRCDIR)/kernel/interrupt_stubs.cpp)
kernel_object_files := $(patsubst $(SRCDIR)/kernel/%.cpp, $(BUILDDIR)/kernel/%.o, $(kernel_source_files))

x86_64_cpp_source_files := $(shell find $(SRCDIR) -name *.cpp | grep -v $(SRCDIR)/kernel/interrupt_stubs.cpp)
x86_64_cpp_object_files := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/x86_64/%.o, $(x86_64_cpp_source_files))

header_files :=  $(shell find include -name *.h)

# this doesn't work, fix later
# font_object_files := $(shell find $(BUILDDIR)/x86_64/interface/graphics -name *.font)

font_source_files = $(shell find $(SRCDIR) -name *.psf)
font_object_files = $(patsubst $(SRCDIR)/%.psf, $(BUILDDIR)/x86_64/%.font, $(font_source_files))

x86_64_object_files := $(x86_64_cpp_object_files) $(x86_64_asm_object_files) $(BUILDDIR)/x86_64/kernel/interrupt_stubs.o $(font_object_files)
	
$(font_object_files): $(BUILDDIR)/x86_64/%.font : $(SRCDIR)/%.psf
	objcopy -O elf64-x86-64 -B i386 -I binary $(patsubst $(BUILDDIR)/x86_64/%.font, $(SRCDIR)/%.psf, $@) $@

$(BUILDDIR)/x86_64/kernel/interrupt_stubs.o: $(SRCDIR)/kernel/interrupt_stubs.cpp include/kernel/interrupt_stubs.h
	mkdir -p $(dir $@) && \
	g++ $(CFLAGS) -fpermissive -mno-red-zone -mgeneral-regs-only -I include $(SRCDIR)/kernel/interrupt_stubs.cpp -o $@

$(x86_64_cpp_object_files): $(BUILDDIR)/x86_64/%.o : $(SRCDIR)/%.cpp $(header_files)
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