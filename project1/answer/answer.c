// Standard C Headers
#include <malloc.h>
#include <stdio.h>
#include <string.h>
// UNIX system calls
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
// Project Header
#include "answer.h"

ElfData getELFData(const char *executable) {
    ElfData elfData;
    memset(&elfData, 0, sizeof(ElfData));
    struct stat sb;  // file stat
    char *p;         // memory-mapped address
    int fd;          // file descriptor

    fd = open(executable, O_RDONLY);
    if (fd == -1) {
        return elfData;
    }
    if (fstat(fd, &sb) == -1) {
        return elfData;
    }
    p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        return elfData;
    }
    // check magic number
    if (p[0] != 0x7f || p[1] != 'E' || p[2] != 'L' || p[3] != 'F') {
        return elfData;
    }
    // load file header
    memcpy(&elfData.elfHeader, p, sizeof(Elf64_Ehdr));

    // load program header items
    Elf64_Phdr *programHeaders =
        malloc(sizeof(Elf64_Phdr) * elfData.elfHeader.e_phnum);
    for (size_t i = 0; i < elfData.elfHeader.e_phnum; ++i) {
        off_t offset =
            elfData.elfHeader.e_phoff + i * elfData.elfHeader.e_phentsize;
        memcpy(&programHeaders[i], p + offset, elfData.elfHeader.e_phentsize);
    }
    elfData.programHeader = programHeaders;

    // load section header items
    ElfSection *sections =
        malloc(sizeof(ElfSection) * elfData.elfHeader.e_shnum);
    size_t shstrtabIdx = -1, dynsymtabIdx = -1, symtabIdx = -1,
           dynstrtabIdx = -1, strtabIdx = -1;

    for (size_t i = 0; i < elfData.elfHeader.e_shnum; ++i) {
        off_t offset =
            elfData.elfHeader.e_shoff + i * elfData.elfHeader.e_shentsize;
        memcpy(&(sections[i].sectionHeader), p + offset,
               elfData.elfHeader.e_shentsize);
        // find .shstrtab
        if (sections[i].sectionHeader.sh_type == SHT_STRTAB &&
            sections[i].sectionHeader.sh_flags == 0 && shstrtabIdx == -1) {
            shstrtabIdx = i;
        }
    }
    elfData.sections = sections;

    // load name of each sections
    for (size_t i = 0; i < elfData.elfHeader.e_shnum; ++i) {
        off_t shstrtabOffset = sections[shstrtabIdx].sectionHeader.sh_offset;
        char *nameAddr = p + shstrtabOffset + sections[i].sectionHeader.sh_name;
        size_t nameLen = strlen(nameAddr);
        // string in C language is char array like "abcd\0acdde\0"
        // set \0 at the end of the char array
        // for marking the end of the string
        sections[i].sectionName = malloc(nameLen + 1);
        sections[i].sectionName[nameLen] = '\0';
        memcpy(sections[i].sectionName, nameAddr, nameLen);

        // string compare, return 0 if two strings are equal
        if (strncmp(sections[i].sectionName, ".dynsym", 7) == 0) {
            dynsymtabIdx = i;
        }
        if (strncmp(sections[i].sectionName, ".dynstr", 7) == 0) {
            dynstrtabIdx = i;
        }
        if (strncmp(sections[i].sectionName, ".symtab", 7) == 0) {
            symtabIdx = i;
        }
        if (strncmp(sections[i].sectionName, ".strtab", 7) == 0) {
            strtabIdx = i;
        }
    }

    // dynamic symbol
    elfData.dynSymbols.size =
        sections[dynsymtabIdx].sectionHeader.sh_size / sizeof(Elf64_Sym);
    elfData.dynSymbols.list =
        malloc(sizeof(ElfSymbol) * elfData.dynSymbols.size);
    for (size_t i = 0; i < elfData.dynSymbols.size; ++i) {
        off_t offset = sections[dynsymtabIdx].sectionHeader.sh_offset +
                       i * sections[dynsymtabIdx].sectionHeader.sh_entsize;
        memcpy(&(elfData.dynSymbols.list[i].symbol), p + offset,
               sizeof(Elf64_Sym));
        off_t shstrtabOffset = sections[dynstrtabIdx].sectionHeader.sh_offset;
        char *name =
            p + shstrtabOffset + elfData.dynSymbols.list[i].symbol.st_name;
        size_t nameLen = strlen(name);
        if (nameLen > 0) {
            elfData.dynSymbols.list[i].name = malloc(nameLen + 1);
            elfData.dynSymbols.list[i].name[nameLen] = '\0';
            memcpy(elfData.dynSymbols.list[i].name, name, nameLen);
        } else {
            elfData.dynSymbols.list[i].name = NULL;
        }
    }

    // other symbol
    if (symtabIdx == -1) {
        elfData.otherSymbols.size = 0;
        elfData.otherSymbols.list = NULL;
    } else {
        elfData.otherSymbols.size =
            sections[symtabIdx].sectionHeader.sh_size / sizeof(Elf64_Sym);
        elfData.otherSymbols.list =
            malloc(sizeof(ElfSymbol) * elfData.otherSymbols.size);
        for (size_t i = 0; i < elfData.otherSymbols.size; ++i) {
            off_t offset = sections[symtabIdx].sectionHeader.sh_offset +
                           i * sections[symtabIdx].sectionHeader.sh_entsize;
            memcpy(&(elfData.otherSymbols.list[i].symbol), p + offset,
                   sizeof(Elf64_Sym));
            off_t shstrtabOffset = sections[strtabIdx].sectionHeader.sh_offset;
            char *name = p + shstrtabOffset +
                         elfData.otherSymbols.list[i].symbol.st_name;
            size_t nameLen = strlen(name);
            if (nameLen > 0) {
                elfData.otherSymbols.list[i].name = malloc(nameLen + 1);
                elfData.otherSymbols.list[i].name[nameLen] = '\0';
                memcpy(elfData.otherSymbols.list[i].name, name, nameLen);
            } else {
                elfData.otherSymbols.list[i].name = NULL;
            }
        }
    }
    munmap(p, sb.st_size);
    close(fd);
    return elfData;
}

void destroyELFData(ElfData elfData) {
    // This demonstrates valgrind leak detection.
    for (int i = 0; i < elfData.elfHeader.e_shnum; ++i) {
        free(elfData.sections[i].sectionName);
    }
    for (int i = 0; i < elfData.dynSymbols.size; ++i) {
        if (elfData.dynSymbols.list[i].name != NULL) {
            free(elfData.dynSymbols.list[i].name);
        }
    }
    free(elfData.dynSymbols.list);
    if (elfData.otherSymbols.list != NULL) {
        for (int i = 0; i < elfData.otherSymbols.size; ++i) {
            if (elfData.otherSymbols.list[i].name != NULL) {
                free(elfData.otherSymbols.list[i].name);
            }
        }
        free(elfData.otherSymbols.list);
    }
    free(elfData.sections);
    free(elfData.programHeader);
}
