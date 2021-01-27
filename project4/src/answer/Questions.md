1) What fields did you have to add

sector size and block size

2) What is the maximum size of a file supported by your inode structure?  Show your work.

My inode structure max size is 512x123+512x128 for 123 direct inode and 128 not direct inode,512 is block size

3) Is your inode structure a multilevel index?  If so, why did you choose this particular combination of direct, indirect, and doubly
   indirect blocks?  If not, why did you choose an alternative inode structure, and what advantages and disadvantages does your structure have, compared to a multilevel index?

My inode structure is a multilevel index. I choose this combination for read file faster and based on my ability of understanding this part of class

4) Describe your code for traversing a user-specified path.  How do traversals of absolute and relative paths differ?

(1)Load all files. (2) get child file inode num if there is one we search for. (3) load content recursively.


