# project_3

CS 551 Project 3: File System Recovery

Requirements:

Develop the following tools for the MINIX file system:

A DirectoryWalker that will traverse the directory tree of a given directory and produce all the subdirectories and 
files reachable from that directory as well as the inodes and zones allocated to those subdirectories and files.

An iNodeBitMapWalker that reads the inode bit map and returns all the inodes that have been allocated.

A ZoneBitMapWalker that reads the zone bit map and returns all the zones that have been allocated.

Develop a set of utilities that will repair the file system when the following happens:

The inode bit map is damaged.

The zone bit map is damaged.

A directory file is completely corrupted.

The inode of a directory file is completely corrupted.

Also develop the tools that will allow you to test the above.

Also recover from the following situations:

Part of the superblock is damaged.

The inode of a file is damaged.

A block (or blocks) allocated to a file is damaged
