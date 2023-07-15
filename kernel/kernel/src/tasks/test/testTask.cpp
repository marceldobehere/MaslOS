#include "testTask.h"
#include "../../devices/pit/pit.h"
#include "../../memory/heap.h"
#include "../../osData/osData.h"
#include "../../osData/MStack/MStackM.h"
#include "../../paging/paging.h"








// BIG SHOUTOUTS TO https://github.com/cloudflare/cloudflare-blog/tree/master/2021-03-obj-file/3


/* parsing ELF files */
#include <elf.h>

/* from https://elixir.bootlin.com/linux/v5.11.6/source/arch/x86/include/asm/elf.h#L51 */
#define R_X86_64_PC32 2
#define R_X86_64_PLT32 4

typedef union {
    const Elf64_Ehdr *hdr;
    const uint8_t *base;
} objhdr;

/* obj.o memory address */
static objhdr obj;

/* sections table */
static const Elf64_Shdr *sections;
static const char *shstrtab = NULL;

/* symbols table */
static const Elf64_Sym *symbols;
/* number of entries in the symbols table */
static int num_symbols;
static const char *strtab = NULL;

static uint64_t page_size;

/* runtime base address of the imported code */
static uint8_t *text_runtime_base;
/* runtime base of the .data section */
static uint8_t *data_runtime_base;
/* runtime base of the .rodata section */
static uint8_t *rodata_runtime_base;
/* runtime base of the .bss section */
static uint8_t *bss_runtime_base;

/* number of external symbols in the symbol table */
static int num_ext_symbols = 0;

struct ext_jump {
    /* address to jump to */
    uint8_t *addr;
    /* unconditional x64 JMP instruction */
    /* should always be {0xff, 0x25, 0xf2, 0xff, 0xff, 0xff} */
    /* so it would jump to an address stored at addr above */
    uint8_t instr[6];
};

struct ext_jump *jumptable;

/* external dependencies for obj.o */
static int my_puts(const char *s)
{
    GlobalRenderer->Println(s);
    //puts("my_puts executed");
    return 1;//puts(s);
}

void perror(const char* s)
{
    GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->Println(s, Colors.bred);
    PIT::Sleep(5000);
}

static inline uint64_t page_align(uint64_t n)
{
    return (n + (page_size - 1)) & ~(page_size - 1);
}

void TaskTest::load_obj(void* data, uint64_t len)
{
    AddToStack();
    uint64_t AMT = (len + 0x0FFF) / 0x1000;

    // AddToStack();
    // GlobalRenderer->Println("LEN 0: {}", to_string(len), Colors.yellow);
    // GlobalRenderer->Println("AMT 0: {}", to_string(AMT), Colors.yellow);
    // RemoveFromStack();

    

    AddToStack();
    void* posTest = (void*) 0x60000000000;
    void* oldPosTest = posTest;
    for (int i = 0; i < AMT; i++)
    {
        GlobalPageTableManager.MapMemory(posTest, GlobalAllocator->RequestPage(), false);
        posTest = (void*)((uint64_t)posTest + 0x1000);
    }
    RemoveFromStack();

    // GlobalRenderer->Println("ADDR 0: {}", ConvertHexToString((uint64_t)posTest), Colors.yellow);
    // GlobalRenderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)oldPosTest), Colors.yellow);

    //PIT::Sleep(4000);


    AddToStack();
    _memcpy(data, oldPosTest, len);
    RemoveFromStack();

    obj.base = (const uint8_t*)oldPosTest;//mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (obj.base == NULL) {
        perror("Maping obj.o failed");
        //exit(errno);
    }
    RemoveFromStack();
    //_memcpy(data, (void*)obj.base, len);
    // struct stat sb;

    // int fd = open("objects/merge.o", O_RDONLY);
    // if (fd <= 0) {
    //     perror("Cannot open merge.o");
    //     //exit(errno);
    // }

    // /* we need obj.o size for mmap(2) */
    // if (fstat(fd, &sb)) {
    //     perror("Failed to get obj.o info");
    //     //exit(errno);
    // }

    // /* mmap obj.o into memory */
    // obj.base = (const uint8_t*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    // if (obj.base == NULL) {
    //     perror("Maping obj.o failed");
    //     //exit(errno);
    // }
    // close(fd);
}

static const Elf64_Shdr *lookup_section(const char *name)
{
    size_t name_len = StrLen(name);

    /* number of entries in the sections table is encoded in the ELF header */
    for (Elf64_Half i = 0; i < obj.hdr->e_shnum; i++) {
        /* sections table entry does not contain the string name of the section
         * instead, the `sh_name` parameter is an offset in the `.shstrtab`
         * section, which points to a string name
         */
        const char *section_name = shstrtab + sections[i].sh_name;
        size_t section_name_len = StrLen(section_name);

        if (name_len == section_name_len && StrEquals(name, section_name)) {
            /* we ignore sections with 0 size */
            if (sections[i].sh_size)
                return sections + i;
        }
    }

    return NULL;
}

static void *lookup_function(const char *name)
{
    size_t name_len = StrLen(name);

    /* loop through all the symbols in the symbol table */
    for (int i = 0; i < num_symbols; i++) {
        /* consider only function symbols */
        if (ELF64_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
            /* symbol table entry does not contain the string name of the symbol
             * instead, the `st_name` parameter is an offset in the `.strtab`
             * section, which points to a string name
             */
            const char *function_name = strtab + symbols[i].st_name;
            size_t function_name_len = StrLen(function_name);

            if (name_len == function_name_len && StrEquals(name, function_name)) {
                /* st_value is an offset in bytes of the function from the
                 * beginning of the `.text` section
                 */
                return text_runtime_base + symbols[i].st_value;
            }
        }
    }

    return NULL;
}















void FN_OS_Window_Log(OS_Window* window, const char* msg)
{
    // puts("Testo: ");
    // puts(msg);
    // printf("ADDR: %llx\n", (uint64_t)window);
    // printf("VAL: %d\n", window->value);
    ((Window*)window->window)->Log(msg);
}

// #include <cmath>

// double _Floor(double val)
// {
//     return floor(val);
// }

// double _Pow(double x, double y)
// {
//     return pow(x, y);
// }

// double _Sqrt(double val)
// {
//     return sqrt(val);
// }



static void *lookup_ext_function(const char *name)
{
    size_t name_len = StrLen(name);

    if (name_len == StrLen("puts") && StrEquals(name, "puts"))
        return (void*)my_puts;

    if (name_len == StrLen("_ZN9OS_Window3LogEPKc") && StrEquals(name, "_ZN9OS_Window3LogEPKc"))
        return (void*)FN_OS_Window_Log;

    // if (name_len == StrLen("floor") && StrEquals(name, "floor"))
    //     return (void*)_Floor;
    // if (name_len == StrLen("sqrt") && StrEquals(name, "sqrt"))
    //     return (void*)_Sqrt;
    // if (name_len == StrLen("pow") && StrEquals(name, "pow"))
    //     return (void*)_Pow;
   
    perror("No address for function %s\n");
    //exit(ENOENT);
    return NULL;
}

static void count_external_symbols(void)
{
    const Elf64_Shdr *rela_text_hdr = lookup_section(".rela.text");
    if (!rela_text_hdr) {
        perror("Failed to find .rela.text\n");
        //return;
        //exit(ENOEXEC);
    }

    int num_relocations = rela_text_hdr->sh_size / rela_text_hdr->sh_entsize;
    const Elf64_Rela *relocations = (Elf64_Rela *)(obj.base + rela_text_hdr->sh_offset);

    for (int i = 0; i < num_relocations; i++) {
        int symbol_idx = ELF64_R_SYM(relocations[i].r_info);

        /* if there is no section associated with a symbol, it is probably
         * an external reference */
        if (symbols[symbol_idx].st_shndx == SHN_UNDEF)
            num_ext_symbols++;
    }
}

static uint8_t *section_runtime_base(const Elf64_Shdr *section)
{
    const char *section_name = shstrtab + section->sh_name;
    size_t section_name_len = StrLen(section_name);

    if (StrLen(".text") == section_name_len && StrEquals(".text", section_name))
        return text_runtime_base;

    if (StrLen(".data") == section_name_len && StrEquals(".data", section_name))
        return data_runtime_base;

    if (StrLen(".rodata") == section_name_len && StrEquals(".rodata", section_name))
        return rodata_runtime_base;

    if (StrLen(".bss") == section_name_len && StrEquals(".bss", section_name))
        return bss_runtime_base;

    perror("No runtime base address for section %s\n");
    //exit(ENOENT);
    return NULL;
}

static void do_text_relocations(void)
{
    /* we actually cheat here - the name .rela.text is a convention, but not a
     * rule: to figure out which section should be patched by these relocations
     * we would need to examine the rela_text_hdr, but we skip it for simplicity
     */
    const Elf64_Shdr *rela_text_hdr = lookup_section(".rela.text");
    if (!rela_text_hdr) {
        perror("Failed to find .rela.text\n");
        //return;
        //exit(ENOEXEC);
    }

    int num_relocations = rela_text_hdr->sh_size / rela_text_hdr->sh_entsize;
    const Elf64_Rela *relocations = (Elf64_Rela *)(obj.base + rela_text_hdr->sh_offset);

    for (int i = 0; i < num_relocations; i++) {
        int symbol_idx = ELF64_R_SYM(relocations[i].r_info);
        int type = ELF64_R_TYPE(relocations[i].r_info);

        /* where to patch .text */
        uint8_t *patch_offset = text_runtime_base + relocations[i].r_offset;
        /* symbol, with respect to which the relocation is performed */
        uint8_t *symbol_address;
        
        /* if this is an external symbol */
        if (symbols[symbol_idx].st_shndx == SHN_UNDEF) {
            static int curr_jmp_idx = 0;

            /* get external symbol/function address by name */
            jumptable[curr_jmp_idx].addr = (uint8_t*)lookup_ext_function(strtab +  symbols[symbol_idx].st_name);

            /* x64 unconditional JMP with address stored at -14 bytes offset */
            /* will use the address stored in addr above */
            jumptable[curr_jmp_idx].instr[0] = 0xff;
            jumptable[curr_jmp_idx].instr[1] = 0x25;
            jumptable[curr_jmp_idx].instr[2] = 0xf2;
            jumptable[curr_jmp_idx].instr[3] = 0xff;
            jumptable[curr_jmp_idx].instr[4] = 0xff;
            jumptable[curr_jmp_idx].instr[5] = 0xff;

            /* resolve the relocation with respect to this unconditional JMP */
            symbol_address = (uint8_t *)(&jumptable[curr_jmp_idx].instr);

            curr_jmp_idx++;
        } else {
            symbol_address = section_runtime_base(&sections[symbols[symbol_idx].st_shndx]) + symbols[symbol_idx].st_value;
        }

        switch (type)
        {
        case R_X86_64_PC32:
            /* S + A - P, 32 bit output, S == L here */
        case R_X86_64_PLT32:
            /* L + A - P, 32 bit output */
            *((uint32_t *)patch_offset) = symbol_address + relocations[i].r_addend - patch_offset;
            break;
        }
    }
}

static void parse_obj(void)
{
    AddToStack();
    /* the sections table offset is encoded in the ELF header */
    sections = (const Elf64_Shdr *)(obj.base + obj.hdr->e_shoff);
    /* the index of `.shstrtab` in the sections table is encoded in the ELF header
     * so we can find it without actually using a name lookup
     */
    // GlobalRenderer->Println("BASE: {}", ConvertHexToString((uint64_t)obj.base), Colors.yellow);
    // GlobalRenderer->Println("HDR: {}", ConvertHexToString((uint64_t)obj.hdr), Colors.yellow);
    // GlobalRenderer->Println("OFFSET: {}", ConvertHexToString(obj.hdr->e_shoff), Colors.yellow);
    RemoveFromStack();

    AddToStack();
    shstrtab = (const char *)(obj.base + sections[obj.hdr->e_shstrndx].sh_offset);
    RemoveFromStack();


    AddToStack();
    /* find the `.symtab` entry in the sections table */
    const Elf64_Shdr *symtab_hdr = lookup_section(".symtab");
    if (symtab_hdr == NULL) {
        perror("Failed to find .symtab\n");
        //exit(ENOEXEC);
    }
    RemoveFromStack();

    

    AddToStack();
    /* the symbols table */
    symbols = (const Elf64_Sym *)(obj.base + symtab_hdr->sh_offset);
    /* number of entries in the symbols table = table size / entry size */
    // GlobalRenderer->Println("SYMBOLS: {}", ConvertHexToString((uint64_t)symbols), Colors.yellow);
    // GlobalRenderer->Println("SIZE: {}", to_string(symtab_hdr->sh_entsize), Colors.yellow);

    num_symbols = symtab_hdr->sh_size / symtab_hdr->sh_entsize;
    RemoveFromStack();

    AddToStack();
    const Elf64_Shdr *strtab_hdr = lookup_section(".strtab");
    if (!strtab_hdr) {
        perror("Failed to find .strtab\n");
        //exit(ENOEXEC);
    }
    RemoveFromStack();

    AddToStack();
    strtab = (const char *)(obj.base + strtab_hdr->sh_offset);
    RemoveFromStack();

    /* get system page size */
    page_size = 0x1000;//sysconf(_SC_PAGESIZE);

    AddToStack();
    /* find the `.text` entry in the sections table */
    const Elf64_Shdr *text_hdr = lookup_section(".text");
    if (!text_hdr) {
        perror("Failed to find .text\n");
        //exit(ENOEXEC);
    }
    RemoveFromStack();

    AddToStack();
    /* find the `.data` entry in the sections table */
    const Elf64_Shdr *data_hdr = lookup_section(".data");
    if (!data_hdr) {
        perror("Failed to find .data\n");
        //exit(ENOEXEC);
    }
    RemoveFromStack();

    AddToStack();
    /* find the `.rodata` entry in the sections table */
    const Elf64_Shdr *rodata_hdr = lookup_section(".rodata");
    if (!rodata_hdr) {
        perror("Failed to find .rodata\n");
        //exit(ENOEXEC);
    }
    RemoveFromStack();

    AddToStack();
    /* find the `.rodata` entry in the sections table */
    const Elf64_Shdr *bss_hdr = lookup_section(".bss");
    if (!rodata_hdr) {
        perror("Failed to find .bss\n");
        //exit(ENOEXEC);
    }
    RemoveFromStack();

    AddToStack();
    count_external_symbols();
    RemoveFromStack();

    AddToStack();


    uint64_t AMT = page_align(text_hdr->sh_size) + 
                        page_align(data_hdr->sh_size) + 
                        page_align(rodata_hdr->sh_size) + 
                        page_align(bss_hdr->sh_size) + 
                        page_align(sizeof(struct ext_jump) * num_ext_symbols);

    //GlobalRenderer->Println("AMT: {}", ConvertHexToString(AMT), Colors.yellow);

    void* posTest = (void*) 0x50000000000;
    void* oldPosTest = posTest;
    for (int i = 0; i < AMT / 0x1000; i++)
    {
        GlobalPageTableManager.MapMemory(posTest, GlobalAllocator->RequestPage(), false);
        posTest = (void*)((uint64_t)posTest + 0x1000);
    }

    text_runtime_base = (uint8_t*)oldPosTest;
    // GlobalRenderer->Println("RUNTIME BASE: {}", ConvertHexToString((uint64_t)text_runtime_base), Colors.yellow);
    // GlobalRenderer->Println("RUNTIME END: {}", ConvertHexToString((uint64_t)posTest), Colors.yellow);

    RemoveFromStack();

    AddToStack();
    if (text_runtime_base == NULL) {
        perror("Failed to allocate memory");
        //exit(errno);
    }
    RemoveFromStack();

    AddToStack();
    /* .data will come right after .text */
    data_runtime_base = text_runtime_base + page_align(text_hdr->sh_size);
    /* .rodata will come after .data */
    rodata_runtime_base = data_runtime_base + page_align(data_hdr->sh_size);

    bss_runtime_base = rodata_runtime_base + page_align(rodata_hdr->sh_size);
    /* jumptable will come after .rodata */
    jumptable = (struct ext_jump *)(bss_runtime_base + page_align(bss_hdr->sh_size));
    RemoveFromStack();

    AddToStack();
    /* copy the contents of `.text` section from the ELF file */
    _memcpy((void*)(obj.base + text_hdr->sh_offset), text_runtime_base, text_hdr->sh_size);
    /* copy .data */
    _memcpy((void*)(obj.base + data_hdr->sh_offset), data_runtime_base, data_hdr->sh_size);
    /* copy .rodata */
    _memcpy((void*)(obj.base + rodata_hdr->sh_offset), rodata_runtime_base, rodata_hdr->sh_size);
    _memcpy((void*)(obj.base + bss_hdr->sh_offset), bss_runtime_base, bss_hdr->sh_size);
    RemoveFromStack();

    AddToStack();
    do_text_relocations();
    RemoveFromStack();

    /* make the `.text` copy readonly and executable */
    // if (mprotect(text_runtime_base, page_align(text_hdr->sh_size), PROT_READ | PROT_EXEC)) {
    //     perror("Failed to make .text executable");
    //     //exit(errno);
    // }

    /* we don't need to do anything with .data - it should remain read/write */

    /* make the `.rodata` copy readonly */
    // if (mprotect(rodata_runtime_base, page_align(rodata_hdr->sh_size), PROT_READ)) {
    //     perror("Failed to make .rodata readonly");
    //     //exit(errno);
    // }

    // /* make the jumptable readonly and executable */
    // if (mprotect(jumptable, page_align(sizeof(struct ext_jump) * num_ext_symbols), PROT_READ | PROT_EXEC)) {
    //     perror("Failed to make the jumptable executable");
    //     //exit(errno);
    // }
}


void (*_init)(KernelAppData);
bool (*_do)();
void (*_free)();

void TaskTest::execute_funcs()
{
    // /* pointers to imported functions */
    // int (*add5)(int);
    // void (*hello)(KernelAppData);

    // AddToStack();
    // add5 = (int (*)(int))lookup_function("add5");
    // hello = (void (*)(KernelAppData))lookup_function("HELLO");
    // if (!add5) {
    //     perror("Failed to find add5 function\n");
    //     //exit(ENOENT);
    // }
    // RemoveFromStack();

    // AddToStack();
    // my_puts("Executing add5...");
    //printf("add5(%d) = %d\n", 42, add5(42));
    
    // AddToStack();
    // // GlobalRenderer->Println("ADDR ADD: {}", ConvertHexToString((uint64_t)add5), Colors.bgreen);
    // // GlobalRenderer->Println("ADDR HELLO: {}", ConvertHexToString((uint64_t)hello), Colors.bgreen);
    // RemoveFromStack();



    // void (*test2)();
    // test2 = &Test;
    // test2();

    // AddToStack();
    // void* test3 = GlobalAllocator->RequestPage();
    // RemoveFromStack();

    // AddToStack();
    // _memcpy((void*)((uint64_t)test2 - 100), test3, 0x1000);
    // RemoveFromStack();
    
    // AddToStack();
    // GlobalRenderer->Println("ADDR 1: {}", ConvertHexToString((uint64_t)test2), Colors.bgreen);
    // RemoveFromStack();
    // AddToStack();
    // GlobalRenderer->Println("ADDR 2: {}", ConvertHexToString((uint64_t)test3), Colors.bgreen);
    // RemoveFromStack();







    // AddToStack();
    // test2 = (void (*)())((uint64_t)test3 + 100);
    // RemoveFromStack();

    // AddToStack();
    // //test2();
    // RemoveFromStack();

    // RemoveFromStack();
    // return;


    // AddToStack();
    // int res = add5(42);
    // RemoveFromStack();
    
    // GlobalRenderer->Println("add5(42) = {}", to_string(res), Colors.yellow);
    // RemoveFromStack();

    // AddToStack();
    
    // if (!hello) {
    //     perror("Failed to find hello function\n");
    //     //exit(ENOENT);
    // }
    // RemoveFromStack();

    // AddToStack();
    // my_puts("Executing hello...");
    // KernelAppData x;
    // x.test = false;
    // hello(x);
    // x.test = true;
    // hello(x);
    // RemoveFromStack();


    _init = (void (*)(KernelAppData))lookup_function("Module_Init");
    if (!_init) {
        perror("Failed to find Module_Init function\n");
    }
    
    _do = (bool (*)())lookup_function("Module_Do");
    if (!_do) {
        perror("Failed to find Module_Do function\n");
    }
   
    _free = (void (*)())lookup_function("Module_Free");
    if (!_free) {
        perror("Failed to find Module_Free function\n");
    }

    AddToStack();

    AddToStack();
    _init(kernelAppData);
    RemoveFromStack();




    RemoveFromStack();

}

int TaskTest::InitTheStuff(void* data, uint16_t len)
{
    AddToStack();
    load_obj(data, len);
    parse_obj();
    execute_funcs();

    RemoveFromStack();
    return 0;
}



void* tMalloc(uint64_t size)
{
    return _Malloc(size);
}

void tFree(void* addr)
{
    _Free(addr);
}



TaskTest::TaskTest(void* data, uint64_t len, Window* window)
{
    done = false;
    type = TaskType::TESTO;
    dataCopy = _Malloc(len);
    dataLen = len;
    _memcpy(data, dataCopy, len);
    this->window = window;

    kernelAppData.test = false;
    kernelAppData.window = (OS_Window*)_Malloc(sizeof(OS_Window));
    kernelAppData.window->window = (void*)window;
    kernelAppData.window->windowFramebuffer.height = window->framebuffer->Height;
    kernelAppData.window->windowFramebuffer.width = window->framebuffer->Width;
    kernelAppData.window->windowFramebuffer.size = window->framebuffer->BufferSize;
    kernelAppData.window->windowFramebuffer.pps = window->framebuffer->PixelsPerScanLine;
    kernelAppData.window->windowFramebuffer.start = window->framebuffer->BaseAddress;
    kernelAppData.OS_Malloc = tMalloc;
    kernelAppData.OS_Free = tFree;
    oldIcon = window->icon;
    window->icon = WindowManager::internalWindowIcons[WindowManager::windowIconEnum.TESTO_PGM];



    InitTheStuff(dataCopy, dataLen);
}


void TaskTest::Do()
{
    AddToStack();
    done = _do();
    RemoveFromStack();
}

TaskTest* NewTestTask(void* data, uint64_t len, Window* window)
{
    TaskTest* task = (TaskTest*)_Malloc(sizeof(TaskTest), "New Test Task");
    *task = TaskTest(data, len, window);
    return task;
}

void TaskTest::Free()
{
    window->icon = oldIcon;
    AddToStack();
    _free();
    RemoveFromStack();

    _Free(dataCopy);
}


