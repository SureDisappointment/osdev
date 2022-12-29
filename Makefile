ifeq ($(OS), Windows_NT)
UNAME := Windows
else
UNAME := $(shell uname)
endif

ifeq ($(UNAME), Linux) # Only on Linux
x86_64_asm_source_files := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o, $(x86_64_asm_source_files))

$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@

.PHONY: build-x86_64
build-x86_64: $(x86_64_asm_object_files)
	mkdir -p dist/x86_64 && .
	x86_64-elf-ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(x86_64_asm_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso

build-all: build-x86_64

clean:
	rm -rf dist
	rm targets/*/iso/boot/kernel.bin
	rm -rf build
endif


ifeq ($(UNAME), Windows) # Only on Windows
run-x86_64:
	qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso

run-all: run-x86_64

docker-build: buildenv/Dockerfile
	docker build buildenv -t osdev-buildenv

docker-run: docker-build
	docker run --rm -it -v $(shell echo %cd%):/root/env osdev-buildenv
endif