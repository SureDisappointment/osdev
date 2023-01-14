#include "boot/multiboot2.h"
#include "panic.h"
#include "cgascr.h"
#include "stdlib/stdio.h"
#include "io_port.h"
#include "device/ps2_keyboard.h"
#include "guard.h"
#include "cpu.h"
#include "user/app.h"
#include "thread/scheduler.h"
#include "device/watch.h"
#include <stdint.h>

extern void set_kernel_stack(uint64_t stackptr);
extern void jump_usermode(uint64_t start_addr);

void test_user_function()
{
    CGA_show(0, 0, 'U');
    for(;;);
}

void kmain(unsigned long magic, unsigned long addr)
{
    CGA_clear();

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panicf("Invalid magic number: 0x%x\n", (unsigned) magic);

    if(addr & 7)
        panicf("Unaligned mbi: 0x%x\n", addr);

    struct multiboot_tag *tag;
    unsigned int size = *(unsigned int *) addr;
    printf("Announced mbi size 0x%x\n", size);
    for(tag = (struct multiboot_tag *)(addr + 8);
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        printf("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
        switch(tag->type)
        {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
            printf("Command line = %s\n", ((struct multiboot_tag_string *)tag)->string);
            break;
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            printf("Boot loader name = %s\n", ((struct multiboot_tag_string *)tag)->string);
            break;
        case MULTIBOOT_TAG_TYPE_MODULE:
            printf("Module at 0x%x-0x%x. Command line %s\n",
                    ((struct multiboot_tag_module *)tag)->mod_start,
                    ((struct multiboot_tag_module *)tag)->mod_end,
                    ((struct multiboot_tag_module *)tag)->cmdline);
            break;
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            printf("mem_lower = %uKB, mem_upper = %uKB\n",
                    ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
                    ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
            break;
        case MULTIBOOT_TAG_TYPE_BOOTDEV:
            printf("Boot device 0x%x,%u,%u\n",
                    ((struct multiboot_tag_bootdev *)tag)->biosdev,
                    ((struct multiboot_tag_bootdev *)tag)->slice,
                    ((struct multiboot_tag_bootdev *)tag)->part);
            break;
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
            multiboot_memory_map_t *mmap;
            printf("mmap\n");
    
            for(mmap = ((struct multiboot_tag_mmap *)tag)->entries;
                (multiboot_uint8_t *)mmap 
                < (multiboot_uint8_t *)tag + tag->size;
                mmap = (multiboot_memory_map_t *) 
                ((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
            printf(" base_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
                    (unsigned)(mmap->addr >> 32),
                    (unsigned)(mmap->addr & 0xffffffff),
                    (unsigned)(mmap->len >> 32),
                    (unsigned)(mmap->len & 0xffffffff),
                    (unsigned)mmap->type);
        }
        break;
        }
    }
    tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
    printf("Total mbi size 0x%x\n", (unsigned) tag - addr);

    // for(;;)
    //     asm("hlt");

    uint64_t rsp;
    asm volatile("mov %%rsp, %0" : "=r"(rsp));
    set_kernel_stack(rsp);
    guard_enter();

    CGA_clear();
    CGA_setpos(0,2);

    ps2kbd_plugin();
    int_enable();

    asm volatile ("xchg %bx, %bx"); // Bochs breakpoint

    jump_usermode((uint64_t)&test_user_function);

    // Die sind temporäre stacks fürs testen der Kontextwechsel
    unsigned char stack1[(1 << 10)];
    void* stack1_bp = stack1 + sizeof(stack1);

    unsigned char stack2[(1 << 10)];
    void* stack2_bp = stack2 + sizeof(stack2);

    Coroutine *c1 = app(stack1_bp);
    Coroutine *c2 = app2(stack2_bp);
    scheduler_ready(c1);
    scheduler_ready(c2);
    watch_set(60000, 20);
    watch_plugin();
    scheduler_schedule();

    /*CGA_clear();
    CGA_set_color(CGA_F_YELLOW | CGA_B_BLACK);
    CGA_puts("Goodbye, World!\n");
    printf("--- %i %i %o %x %i %i %i %i %u ---\n", 3, 6, 9, 10, 12, 13, 14, 15, -16);
    printf("--- %c %s %s %s ---\n", 'x', "ABC", "DEFG", "Z");*/

    for(;;)
        asm("hlt");
    guard_leave();
}
