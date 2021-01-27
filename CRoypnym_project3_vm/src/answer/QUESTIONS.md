1. Describe your design of the page table and frame tables. How and why did you store your data the way you did?

   -A Page table is an array of int, which is defined as int PFN[NUM_PAGES] and it maps each virtual page of the thread to a physical page. It is defined as a member of Thread structures, since each thread needs its own page table. 

   -A frame table is an array of pointers, which each pointer points to a frame with size PAGE_SIZE. There is only one frame table so it is defined as a global variable.

2. Describe your algorithm for finding the frame associated with a page.

   For each thread, to find the frame of a virtual page, it only needs to look up the `PFN` array with the page number as the index. So `PFN[vpn]` is the frame number in the frame table. If `PFN[vpn]` is -1, then there is no frame associated with the virtual page.

3. When two user processes both need a new frame at the same time, how are races avoided?

   A mutex is used when a thread tries to allocate a new frame, so it will avoid races.

4. Why did you choose the data structure(s) that you did for representing virtual-to-physical mappings?

   An int array is used to map virtual page number to physical page number, since the page number is successive, and the frame table is unique. To find a frame that has been allocated to which thread, I use an inverted page table, which is an array of `IPTE` with size `NUM_PAGES`. Each entry is a structure containing the `Thread` object and the virtual page that associated.

5. When a frame is required but none is free, some frame must be evicted.  Describe your code for choosing a frame to evict.

   To evict a frame from the frame table, I use the [clock] page replacement algorithm. For each frame page there is an `accessed` and `dirty` flag. When a memory is referenced, the `accessed` flag is set to true. If the reference is a write, the `dirty` flag is set to true. 
   The clock hand points to the oldest page. When a page fault occurs, the frame being pointed to by the hand is inspected. 
   If its `accessed` flag is false, the frame is evicted, and the hand is advanced one position.
   If its `accessed` flag is true,  it is cleared and the hand is advanced to the next page.
   This process is repeated until a frame is found with `accessed` is false.

6. When a process P obtains a frame that was previously used by a process Q, how do you adjust the page table (and any other data structures) to reflect the frame Q no longer has?

   From the inverted page table we can know which Q's virtual page used the frame, so, we clear the PFN of Q at that page by setting it to -1. Then we set process P's virtual page to use that frame, and clear the flags, and update the inverted page table to reflect process P used that frame.

7. A page fault in process P can cause another process Q's frame to be evicted.  How do you ensure that Q cannot access or modify the page during the eviction process?  How do you avoid a race between P evicting Q's frame and Q faulting the page back in?

   Since page fault occured when P allocated a frame, we use a mutex to avoid the races between P and Q.
If P evicts Q's frame, when Q fails the page back in, the clock algorithm will not evict the frame agine, since it has forward to another frame.

8. Suppose a page fault in process P causes a page to be read from the file system or swap.  How do you ensure that a second process Q cannot interfere by e.g. attempting to evict the frame while it is still being read in?

   Since page fault occurred when process P allocated a new frame, and there is a mutex to ensure only one process can access the frame table.

9. A single lock for the whole VM system would make synchronization easy, but limit parallelism. On the other hand, using many locks complicates synchronization and raises the possibility for deadlock but allows for high parallelism. Explain where your design falls along this continuum and why you chose to design it this way.

   I choose to use one single lock since it is simple and easy to implement. Premature optimization is not a good idea.
