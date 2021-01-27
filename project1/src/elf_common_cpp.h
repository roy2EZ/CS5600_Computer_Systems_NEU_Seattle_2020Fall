#ifndef ELFTESTER_ELF_COMMON_CPP_H
#define ELFTESTER_ELF_COMMON_CPP_H

#include <elf.h>

extern "C" {
typedef struct ElfSection {
    Elf64_Shdr sectionHeader;
    char* sectionName;
} ElfSection;

typedef struct ElfSymbol {
    Elf64_Sym symbol;
    char* name;
} ElfSymbol;

typedef struct ElfSymbolList {
    ElfSymbol* list;
    int size;
} ElfSymbolList;

typedef struct ElfData {
    Elf64_Ehdr elfHeader;
    ElfSection* sections;
    Elf64_Phdr* programHeader;
    ElfSymbolList dynSymbols;
    ElfSymbolList otherSymbols;
} ElfData;

ElfData answerGetELFData(const char* executable);
ElfData getELFData(const char* executable);
void answerDestroyELFData(ElfData elfData);
void destroyELFData(ElfData elfData);
};
#endif  // ELFTESTER_ELF_COMMON_CPP_H
