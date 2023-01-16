# Check OS
ifeq ($(OS), Windows_NT)
FOUND_OS := Windows
else
FOUND_OS := $(shell uname)
endif

BUILD_DIR ?= build/
DIST_DIR ?= dist/
ISO_NAME ?= kernel.iso

ASM ?= nasm
x86_64_CC ?= x86_64-elf-gcc
CFLAGS := $(CFLAGS) -c -I src/intf -ffreestanding -Wall -Wextra -pedantic -nostdlib -mabi=sysv -mcmodel=large #-g
x86_64_LD ?= x86_64-elf-ld
LFLAGS := $(LFLAGS) -n

ifdef RELEASE
CFLAGS := $(CFLAGS) -O3 -fno-tree-loop-distribute-patterns -fno-tree-vectorize -DNDEBUG
endif

# Targets that don't depend on files
.PHONY: build-x86_64 build-all clean run-x86_64 docker-build docker-run

# Linux only targets (compile & build)
ifeq ($(FOUND_OS), Linux)
# source & object files
x86_64_asm_sources := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_objects := $(patsubst src/impl/x86_64/%.asm, $(BUILD_DIR)/x86_64/%.o, $(x86_64_asm_sources))

x86_64_c_sources := $(shell find src/impl/x86_64 -name *.c)
x86_64_c_objects := $(patsubst src/impl/x86_64/%.c, $(BUILD_DIR)/x86_64/%.o, $(x86_64_c_sources))

kernel_sources := $(shell find src/impl/kernel -name *.c)
kernel_objects := $(patsubst src/impl/kernel/%.c, $(BUILD_DIR)/kernel/%.o, $(kernel_sources))

x86_64_objects := $(x86_64_asm_objects) $(x86_64_c_objects)

# Compile x86_64 asm code
$(x86_64_asm_objects): $(BUILD_DIR)/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	$(ASM) -f elf64 $(patsubst $(BUILD_DIR)/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@

# Compile x86_64 C code using cross-compiler
$(x86_64_c_objects): $(BUILD_DIR)/x86_64/%.o : src/impl/x86_64/%.c
	mkdir -p $(dir $@) && \
	$(x86_64_CC) $(CFLAGS) $(patsubst $(BUILD_DIR)/x86_64/%.o, src/impl/x86_64/%.c, $@) -o $@

# Compile kernel code using cross-compiler
$(kernel_objects): $(BUILD_DIR)/kernel/%.o : src/impl/kernel/%.c
	mkdir -p $(dir $@) && \
	$(x86_64_CC) $(CFLAGS) $(patsubst $(BUILD_DIR)/kernel/%.o, src/impl/kernel/%.c, $@) -o $@

# Build x86_64 iso using cross-linker & grub
build-x86_64: $(x86_64_objects) $(kernel_objects)
	mkdir -p $(DIST_DIR)/x86_64 && .
	$(x86_64_LD) $(LFLAGS) -o $(DIST_DIR)/x86_64/kernel.bin -T targets/x86_64/linker.ld $(x86_64_objects) $(kernel_objects) && \
	cp $(DIST_DIR)/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o $(DIST_DIR)/x86_64/$(ISO_NAME) targets/x86_64/iso

# Build all supported architectures
build-all: build-x86_64

# Clean up object files and disk images
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DIST_DIR)
	rm targets/*/iso/boot/kernel.bin

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
	qemu-system-x86_64 -cdrom $(DIST_DIR)/x86_64/kernel.iso

# Build docker env from Dockerfile
docker-build: buildenv/Dockerfile
	docker build buildenv -t osdev-buildenv

# Start docker container
docker-run: docker-build
	docker run --rm -it -v $(WD):/root/env osdev-buildenv
