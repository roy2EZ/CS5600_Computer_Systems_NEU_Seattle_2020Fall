#ifndef FILESYSTEM_DIRTESTS_H
#define FILESYSTEM_DIRTESTS_H

void testFindRelativeDirInRoot();
void testFindAbsoluteDirInRoot();
void testFindRelativeDir();
void testFindAbsoluteDir();
void testMakeDirRelativeDir();
void testMakeDirAbsoluteDir();
void testEveryonePermissionMakeDirSuccess();
void testMakeDirFailure();
void testUserPermissionMakeDirSuccess();
void testGroupPermissionMakeDirSuccess();

#endif
