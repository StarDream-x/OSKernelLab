#ifndef _ELF_H_
#define _ELF_H_

#include "util/types.h"
#include "process.h"
#include "spike_interface/spike_utils.h"

#define MAX_CMDLINE_ARGS 64

// elf header structure
typedef struct elf_header_t {
  uint32 magic;
  uint8 elf[12];
  uint16 type;      /* Object file type */
  uint16 machine;   /* Architecture */
  uint32 version;   /* Object file version */
  uint64 entry;     /* Entry point virtual address */
  uint64 phoff;     /* Program header table file offset */
  uint64 shoff;     /* Section header table file offset */
  uint32 flags;     /* Processor-specific flags */
  uint16 ehsize;    /* ELF header size in bytes */
  uint16 phentsize; /* Program header table entry size */
  uint16 phnum;     /* Program header table entry count */
  uint16 shentsize; /* Section header table entry size */
  uint16 shnum;     /* Section header table entry count */
  uint16 shstrndx;  /* Section header string table index */
} elf_header;

// Program segment header.
typedef struct elf_prog_header_t {
  uint32 type;   /* Segment type */
  uint32 flags;  /* Segment flags */
  uint64 off;    /* Segment file offset */
  uint64 vaddr;  /* Segment virtual address */
  uint64 paddr;  /* Segment physical address */
  uint64 filesz; /* Segment size in file */
  uint64 memsz;  /* Segment size in memory */
  uint64 align;  /* Segment alignment */
} elf_prog_header;

// elf section header
typedef struct elf_sect_header_t{
    uint32 name;
    uint32 type;
    uint64 flags;
    uint64 addr;
    uint64 offset;
    uint64 size;
    uint32 link;
    uint32 info;
    uint64 addralign;
    uint64 entsize;
} elf_sect_header;

// compilation units header (in debug line section)
typedef struct __attribute__((packed)) {
    uint32 length;
    uint16 version;
    uint32 header_length;
    uint8 min_instruction_length;
    uint8 default_is_stmt;
    int8 line_base;
    uint8 line_range;
    uint8 opcode_base;
    uint8 std_opcode_lengths[12];
} debug_header;

typedef struct elf_sym_tab_t{
  uint32 name;
  uint8 info;
  uint8 other;
  uint16 shndx;
  uint64 value;
  uint64 size;
} elf_sym_tab;

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian
#define ELF_PROG_LOAD 1

#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define STT_FUNC 2
#define ELF64_ST_TYPE(info) ((info) & 0xf)
#define STRTAB_MAX 1000
#define FUNCNUM_MAX 100

typedef enum elf_status_t {
  EL_OK = 0,

  EL_EIO,
  EL_ENOMEM,
  EL_NOTELF,
  EL_ERR,

} elf_status;

typedef struct elf_ctx_t {
  void *info;
  elf_header ehdr;
} elf_ctx;

elf_status elf_init(elf_ctx *ctx, void *info);
elf_status elf_load(elf_ctx *ctx);
elf_status elf_load_sect(elf_ctx *ctx);

void load_bincode_from_host_elf(process *p);

//lab1 challenge1
void backtrace(uint64 dep, uint64 addr);

typedef struct elf_info_t{
    spike_file_t *f;
    process *p;
}elf_info;
uint64 elf_fpread(elf_ctx *ctx, void *dest, uint64 nb, uint64 offset);
void make_addr_line(elf_ctx *ctx, char *debug_line, uint64 length);
#endif
