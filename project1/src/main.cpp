#include <elf.h>
#include <unistd.h>
#include "elf_common_cpp.h"
#include "gtest/gtest.h"

void compareSections(int numSections, ElfSection *expected, ElfSection *actual);
void compareSectionHeaders(Elf64_Shdr expected, Elf64_Shdr actual);
void compareElfSymbol(ElfSymbol expected, ElfSymbol actual);
void compareElfSymbolList(ElfSymbolList expected, ElfSymbolList actual);
bool elfDataEmpty(ElfData data);
bool fileReadable(const char *path);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

void compareSections(int numSections, ElfSection *expected,
                     ElfSection *actual) {
    for (int x = 0; x < numSections; x++) {
        compareSectionHeaders(expected[x].sectionHeader,
                              actual[x].sectionHeader);
        EXPECT_STREQ(expected[x].sectionName, actual[x].sectionName);
    }
}

void compareSectionHeaders(Elf64_Shdr expected, Elf64_Shdr actual) {
    EXPECT_EQ(expected.sh_name, actual.sh_name);
    EXPECT_EQ(expected.sh_type, actual.sh_type);
    EXPECT_EQ(expected.sh_flags, actual.sh_flags);
    EXPECT_EQ(expected.sh_addr, actual.sh_addr);
    EXPECT_EQ(expected.sh_offset, actual.sh_offset);
    EXPECT_EQ(expected.sh_size, actual.sh_size);
    EXPECT_EQ(expected.sh_link, actual.sh_link);
    EXPECT_EQ(expected.sh_info, actual.sh_info);
    EXPECT_EQ(expected.sh_addralign, actual.sh_addralign);
    EXPECT_EQ(expected.sh_entsize, actual.sh_entsize);
}

void compareElfSymbol(ElfSymbol expected, ElfSymbol actual) {
    EXPECT_EQ(expected.symbol.st_name, actual.symbol.st_name);
    EXPECT_EQ(expected.symbol.st_info, actual.symbol.st_info);
    EXPECT_EQ(expected.symbol.st_other, actual.symbol.st_other);
    EXPECT_EQ(expected.symbol.st_shndx, actual.symbol.st_shndx);
    EXPECT_EQ(expected.symbol.st_value, actual.symbol.st_value);
    EXPECT_EQ(expected.symbol.st_size, actual.symbol.st_size);
    if (expected.name) {
        EXPECT_STREQ(expected.name, actual.name);
    } else {
        EXPECT_EQ(expected.name, actual.name);
    }
}

void compareElfSymbolList(ElfSymbolList expected, ElfSymbolList actual) {
    EXPECT_EQ(expected.size, actual.size);
    for (int x = 0; x < actual.size; x++) {
        compareElfSymbol(expected.list[x], actual.list[x]);
    }
}

bool elfDataEmpty(ElfData data) {
    ElfData empty;
    bzero(&empty, sizeof(ElfData));
    return 0 == memcmp(&empty, &data, sizeof(ElfData));
}

bool fileReadable(const char *path) {
    if (access(path, R_OK) != -1) {
        return true;
    }
    return false;
}

const char *ELF_BINARY = "/bin/ls";
const char *ELF_SHARED_OBJECT = "lib/libgcc_s.so.1";
const char *ELF_OTHER_SYMBOLS = "lib/ELFTest";
const char *NON_EXISTENT_FILE = "/bin/__THIS_FILE_DOES_NOT_EXIST__";
const char *NON_ELF_FILE = "src/main.cpp";

TEST(System, ElfBinaryExists) { EXPECT_EQ(true, fileReadable(ELF_BINARY)); }

TEST(System, ElfSharedObjectExists) {
    EXPECT_EQ(true, fileReadable(ELF_SHARED_OBJECT));
}

TEST(System, ElfOtherSymbolsExists) {
    EXPECT_EQ(true, fileReadable(ELF_OTHER_SYMBOLS));
}

TEST(System, NonExistantFile) {
    EXPECT_EQ(false, fileReadable(NON_EXISTENT_FILE));
}

TEST(System, NonElfFileExists) { EXPECT_EQ(true, fileReadable(NON_ELF_FILE)); }

TEST(Header, Executable) {
    const char *testPath = ELF_BINARY;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    Elf64_Ehdr answerHeader = answer.elfHeader;
    Elf64_Ehdr testHeader = test.elfHeader;
    EXPECT_EQ(0, memcmp(testHeader.e_ident, answerHeader.e_ident, EI_NIDENT));
    EXPECT_EQ(answerHeader.e_type, testHeader.e_type);
    EXPECT_EQ(answerHeader.e_machine, testHeader.e_machine);
    EXPECT_EQ(answerHeader.e_version, testHeader.e_version);
    EXPECT_EQ(answerHeader.e_entry, testHeader.e_entry);
    EXPECT_EQ(answerHeader.e_phoff, testHeader.e_phoff);
    EXPECT_EQ(answerHeader.e_shoff, testHeader.e_shoff);
    EXPECT_EQ(answerHeader.e_flags, testHeader.e_flags);
    EXPECT_EQ(answerHeader.e_ehsize, testHeader.e_ehsize);
    EXPECT_EQ(answerHeader.e_phentsize, testHeader.e_phentsize);
    EXPECT_EQ(answerHeader.e_phnum, testHeader.e_phnum);
    EXPECT_EQ(answerHeader.e_shentsize, testHeader.e_shentsize);
    EXPECT_EQ(answerHeader.e_shnum, testHeader.e_shnum);
    EXPECT_EQ(answerHeader.e_shstrndx, testHeader.e_shstrndx);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(Header, SharedObject) {
    const char *testPath = ELF_SHARED_OBJECT;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    Elf64_Ehdr answerHeader = answer.elfHeader;
    Elf64_Ehdr testHeader = test.elfHeader;
    EXPECT_EQ(0, memcmp(testHeader.e_ident, answerHeader.e_ident, EI_NIDENT));
    EXPECT_EQ(answerHeader.e_type, testHeader.e_type);
    EXPECT_EQ(answerHeader.e_machine, testHeader.e_machine);
    EXPECT_EQ(answerHeader.e_version, testHeader.e_version);
    EXPECT_EQ(answerHeader.e_entry, testHeader.e_entry);
    EXPECT_EQ(answerHeader.e_phoff, testHeader.e_phoff);
    EXPECT_EQ(answerHeader.e_shoff, testHeader.e_shoff);
    EXPECT_EQ(answerHeader.e_flags, testHeader.e_flags);
    EXPECT_EQ(answerHeader.e_ehsize, testHeader.e_ehsize);
    EXPECT_EQ(answerHeader.e_phentsize, testHeader.e_phentsize);
    EXPECT_EQ(answerHeader.e_phnum, testHeader.e_phnum);
    EXPECT_EQ(answerHeader.e_shentsize, testHeader.e_shentsize);
    EXPECT_EQ(answerHeader.e_shnum, testHeader.e_shnum);
    EXPECT_EQ(answerHeader.e_shstrndx, testHeader.e_shstrndx);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(ProgramHeader, Executable) {
    const char *testPath = ELF_BINARY;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    Elf64_Phdr *answerHeader = answer.programHeader;
    Elf64_Phdr *testHeader = test.programHeader;

    for (int x = 0; x < answer.elfHeader.e_phnum; x++) {
        EXPECT_EQ(answerHeader[x].p_type, testHeader[x].p_type);
        EXPECT_EQ(answerHeader[x].p_flags, testHeader[x].p_flags);
        EXPECT_EQ(answerHeader[x].p_offset, testHeader[x].p_offset);
        EXPECT_EQ(answerHeader[x].p_vaddr, testHeader[x].p_vaddr);
        EXPECT_EQ(answerHeader[x].p_paddr, testHeader[x].p_paddr);
        EXPECT_EQ(answerHeader[x].p_filesz, testHeader[x].p_filesz);
        EXPECT_EQ(answerHeader[x].p_memsz, testHeader[x].p_memsz);
        EXPECT_EQ(answerHeader[x].p_align, testHeader[x].p_align);
    }

    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(ProgramHeader, SharedObject) {
    const char *testPath = ELF_SHARED_OBJECT;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    Elf64_Phdr *answerHeader = answer.programHeader;
    Elf64_Phdr *testHeader = test.programHeader;
    for (int x = 0; x < answer.elfHeader.e_phnum; x++) {
        EXPECT_EQ(answerHeader[x].p_type, testHeader[x].p_type);
        EXPECT_EQ(answerHeader[x].p_flags, testHeader[x].p_flags);
        EXPECT_EQ(answerHeader[x].p_offset, testHeader[x].p_offset);
        EXPECT_EQ(answerHeader[x].p_vaddr, testHeader[x].p_vaddr);
        EXPECT_EQ(answerHeader[x].p_paddr, testHeader[x].p_paddr);
        EXPECT_EQ(answerHeader[x].p_filesz, testHeader[x].p_filesz);
        EXPECT_EQ(answerHeader[x].p_memsz, testHeader[x].p_memsz);
        EXPECT_EQ(answerHeader[x].p_align, testHeader[x].p_align);
    }
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(Sections, Executable) {
    const char *testPath = ELF_BINARY;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    EXPECT_EQ(answer.elfHeader.e_shnum, test.elfHeader.e_shnum);
    compareSections(answer.elfHeader.e_shnum, answer.sections, test.sections);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(Sections, SharedObject) {
    const char *testPath = ELF_SHARED_OBJECT;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    EXPECT_EQ(answer.elfHeader.e_shnum, test.elfHeader.e_shnum);
    compareSections(answer.elfHeader.e_shnum, answer.sections, test.sections);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(DynamicSymbols, Executable) {
    const char *testPath = ELF_BINARY;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    ElfSymbolList answerSymbols = answer.dynSymbols;
    ElfSymbolList testSymbols = test.dynSymbols;
    compareElfSymbolList(answerSymbols, testSymbols);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(DynamicSymbols, SharedObject) {
    const char *testPath = ELF_SHARED_OBJECT;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    ElfSymbolList answerSymbols = answer.dynSymbols;
    ElfSymbolList testSymbols = test.dynSymbols;
    compareElfSymbolList(answerSymbols, testSymbols);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(OtherSymbols, ExecutableEmpty) {
    const char *testPath = ELF_BINARY;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    ElfSymbolList answerSymbols = answer.otherSymbols;
    ElfSymbolList testSymbols = test.otherSymbols;
    compareElfSymbolList(answerSymbols, testSymbols);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(OtherSymbols, SharedObjectEmpty) {
    const char *testPath = ELF_SHARED_OBJECT;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    ElfSymbolList answerSymbols = answer.otherSymbols;
    ElfSymbolList testSymbols = test.otherSymbols;
    compareElfSymbolList(answerSymbols, testSymbols);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(OtherSymbols, Executable) {
    const char *testPath = ELF_OTHER_SYMBOLS;
    ElfData answer = answerGetELFData(testPath);
    ElfData test = getELFData(testPath);
    EXPECT_FALSE(elfDataEmpty(test));
    ElfSymbolList answerSymbols = answer.otherSymbols;
    ElfSymbolList testSymbols = test.otherSymbols;
    compareElfSymbolList(answerSymbols, testSymbols);
    answerDestroyELFData(answer);
    destroyELFData(test);
}

TEST(Error, NonExistentFile) {
    ElfData test = getELFData(NON_EXISTENT_FILE);
    EXPECT_TRUE(elfDataEmpty(test));
}

TEST(Error, NonELFFile) {
    ElfData test = getELFData(NON_ELF_FILE);
    EXPECT_TRUE(elfDataEmpty(test));
}
