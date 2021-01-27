#ifndef VIRTUALMEMFRAMEWORKC_STACK_NONTHREADEDTESTS_H
#define VIRTUALMEMFRAMEWORKC_STACK_NONTHREADEDTESTS_H

void testWriteStackFullUserPage();
void testWriteStackAcrossTwoPages();
void testWriteStackMiddleOfPage();
void testWriteStackPartialPage();
void testWriteStackBeyondEndOfPage();
void testReadStackFullPage();
void testReadStackAcrossTwoPages();
void testReadStackMiddleOfPage();
void testReadStackPartialPage();
void testReadAllStackMem();
void testWriteIntoKernelFails();

#endif //VIRTUALMEMFRAMEWORKC_NONTHREADEDTESTS_H
