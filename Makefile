# Check OS
ifeq ($(OS), Windows_NT)
FOUND_OS := Windows
else
FOUND_OS := $(shell uname)
endif

# Targets that don't depend on files
.PHONY: build-x86_64 build-all clean run-x86_64 run-all docker-build docker-run

# Linux only targets (compile & build)
ifeq ($(FOUND_OS), Linux)
# source & object files
x86_64_asm_sources := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_objects := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o, $(x86_64_asm_sources))

x86_64_c_sources := $(shell find src/impl/x86_64 -name *.c)
x86_64_c_objects := $(patsubst src/impl/x86_64/%.c, build/x86_64/%.o, $(x86_64_c_sources))

kernel_sources := $(shell find src/impl/kernel -name *.c)
kernel_objects := $(patsubst src/impl/kernel/%.c, build/kernel/%.o, $(kernel_sources))

x86_64_objects := $(x86_64_asm_objects) $(x86_64_c_objects)

# Compile x86_64 asm code using nasm
$(x86_64_asm_objects): build/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@

# Compile x86_64 C code using cross-compiler
$(x86_64_c_objects): build/x86_64/%.o : src/impl/x86_64/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I src/intf -ffreestanding $(patsubst build/x86_64/%.o, src/impl/x86_64/%.c, $@) -o $@

# Compile kernel code using cross-compiler
$(kernel_objects): build/kernel/%.o : src/impl/kernel/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I src/intf -ffreestanding $(patsubst build/kernel/%.o, src/impl/kernel/%.c, $@) -o $@

# Build x86_64 iso using cross-linker & grub
build-x86_64: $(x86_64_objects) $(kernel_objects)
	mkdir -p dist/x86_64 && .
	x86_64-elf-ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(x86_64_objects) $(kernel_objects) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso

# Build all supported architectures
build-all: build-x86_64

# Clean up object files and disk images
clean:
	rm -rf dist
	rm targets/*/iso/boot/kernel.bin
	rm -rf build

WD := $(shell pwd)
endif

# Windows targets
ifeq ($(FOUND_OS), Windows)
# Find working directory
WD := $(shell echo %cd%)
ifeq ($(WD), %cd%)
WD := $(shell (pwd).path)
endif
endif


# Emulate x86_64 iso using qemu
run-x86_64:
	qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso

# Emulate all supported architectures
run-all: run-x86_64

# Build docker env from Dockerfile
docker-build: buildenv/Dockerfile
	docker build buildenv -t osdev-buildenv

# Start docker container
docker-run: docker-build
	docker run --rm -it -v $(WD):/root/env osdev-buildenv
