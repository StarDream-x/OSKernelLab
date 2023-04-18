#include <string.h>
#include "kernel/riscv.h"
#include "kernel/process.h"
#include "spike_interface/spike_utils.h"
#include "kernel/elf.h"

static void handle_instruction_access_fault() { panic("Instruction access fault!"); }

static void handle_load_access_fault() { panic("Load access fault!"); }

static void handle_store_access_fault() { panic("Store/AMO access fault!"); }

static void handle_illegal_instruction() { panic("Illegal instruction!"); }

static void handle_misaligned_load() { panic("Misaligned Load!"); }

static void handle_misaligned_store() { panic("Misaligned AMO!"); }

// added @lab1_3
static void handle_timer() {
  int cpuid = 0;
  // setup the timer fired at next time (TIMER_INTERVAL from now)
  *(uint64*)CLINT_MTIMECMP(cpuid) = *(uint64*)CLINT_MTIMECMP(cpuid) + TIMER_INTERVAL;

  // setup a soft interrupt in sip (S-mode Interrupt Pending) to be handled in S-mode
  write_csr(sip, SIP_SSIP);
}

void print_errorline(uint64 mepc){
    char *debugline=current->debugline;
    char **dir=current->dir;
    code_file *file=current->file;
    addr_line *line=current->line;
    int line_ind=current->line_ind;
    char* file_name=NULL;
    char* dir_name=NULL;
    uint64 row_num=0;
    for(int i=0;i<line_ind;++i){
        if(mepc==line[i].addr){
            row_num=line[i].line;
            file_name=file[line[i].file].file;
            dir_name=dir[file[line[i].file].dir];
            break;
        }
    }
    sprint("Runtime error at %s/%s:%lld\n",dir_name,file_name,row_num);
    int c_size= strlen(file_name)+ strlen(dir_name)+1;
    char  c_name[c_size];
    strcpy(c_name,dir_name);
    c_name[strlen(dir_name)]='/';
    strcpy(c_name+strlen(dir_name)+1,file_name);
    c_name[c_size+1]='\0';

    elf_info info;
    spike_file_t *f= spike_file_open(c_name,O_RDONLY,0);
    int cur_row=1;
    char cur_char;
    int cur_idx=0;
    for(cur_idx=0;;cur_idx++){
        spike_file_pread(f,(void*)&cur_char,1,cur_idx);
        if(cur_char=='\n') ++cur_row;
        if(cur_row==row_num) break;
    }
    for(int i=0;;++i){
        spike_file_pread(f,(void*)&cur_char,1,cur_idx+i+1);
        sprint("%c",cur_char);
        if(cur_char=='\n') break;
    }
}

//
// handle_mtrap calls a handling function according to the type of a machine mode interrupt (trap).
//
void handle_mtrap() {
  uint64 mcause = read_csr(mcause);
  uint64 mepc= read_csr(mepc);
  print_errorline(mepc);
  switch (mcause) {
    case CAUSE_MTIMER:
      handle_timer();
      break;
    case CAUSE_FETCH_ACCESS:
      handle_instruction_access_fault();
      break;
    case CAUSE_LOAD_ACCESS:
      handle_load_access_fault();
    case CAUSE_STORE_ACCESS:
      handle_store_access_fault();
      break;
    case CAUSE_ILLEGAL_INSTRUCTION:
      handle_illegal_instruction();
      break;
    case CAUSE_MISALIGNED_LOAD:
      handle_misaligned_load();
      break;
    case CAUSE_MISALIGNED_STORE:
      handle_misaligned_store();
      break;

    default:
      sprint("machine trap(): unexpected mscause %p\n", mcause);
      sprint("            mepc=%p mtval=%p\n", read_csr(mepc), read_csr(mtval));
      panic( "unexpected exception happened in M-mode.\n" );
      break;
  }
}
