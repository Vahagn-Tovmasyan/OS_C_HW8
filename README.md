# Homework 8 - Working with Files in C

## About the implementation

I solved the seven exercises using file descriptors and standard Linux system calls. A file descriptor is the number returned by `open()`; the OS uses it to identify the opened file. Most file operations also have a **current offset**, which tells the OS where the next `read()` or `write()` will operate. The examples below show how changing that offset affects the result.


## Task 1 - Copying a file

I open the source with `O_RDONLY` and the destination with `O_WRONLY | O_CREAT | O_TRUNC`. This means the original file is only read, while the destination is created if necessary and cleared before the new data is written. The mode `0644` sets the requested permissions for a newly created file (subject to `umask`).

The loop reads up to **512 bytes** at a time. `read()` returns the actual number of bytes obtained, which can be smaller than the buffer size. I pass that number to `write()` instead of writing the whole buffer. A second loop checks whether `write()` wrote only part of the data and continues until the whole block is copied. When `read()` returns zero, the source is finished. Summing the sizes of the blocks gives the copied byte count.

## Task 2 - Reducing file length

First, the program writes the alphabet to `data.txt`. Each ASCII letter occupies one byte, so the file initially has **26 bytes**. After reopening with `O_RDWR`, `lseek(file, 0, SEEK_END)` moves the position to the end. The returned offset gives the file size in this case.

Then `ftruncate(file, 10)` changes the **length of the file itself** to 10 bytes, removing everything after `J`. It is not enough to change the current offset: `lseek()` only moves the position, whereas `ftruncate()` removes data. After measuring the new length, I seek to the beginning and read the remaining letters one by one. The result is `ABCDEFGHIJ`.

## Task 3 - Reading backwards

A normal `read()` moves forward, so to reverse a file I have to move the position before every read. I first find its size using `SEEK_END`. Then I seek **one byte before the end**, read it, seek two bytes before the end, and continue until the first byte is reached. In this version the positions are calculated using negative offsets relative to `SEEK_END`.

Every byte is sent straight to standard output. For an empty file the loop runs zero times, but the program still prints a newline. If the source ends with `\n`, that newline appears *first* when the bytes are reversed; this can look like a blank line in the terminal.

## Task 4 - Adding a log message

I open `log.txt` with `O_APPEND`, which makes writes go to the end of the existing file rather than replacing previous entries. The message is read from standard input using `read(0, ...)`. I stop at the first newline, obtain the process ID with `getpid()`, and use `snprintf()` to build one entry in the format `PID=<number>: message\n`.

After the entry is written, `lseek(file, 0, SEEK_CUR)` reports the descriptor's current position. Even with `O_APPEND`, a successful write advances that position to just after the new data. This is why the reported offset increases as more messages are appended. The PID can be different each time the program runs.

## Task 5 - Making a sparse file

The program writes `START`, which puts the offset at byte **5**. Next, `lseek(file, 1024 * 1024, SEEK_CUR)` moves the offset forward by **1,048,576 bytes** without writing anything. When `END` is written, the file extends to that location and the untouched region becomes a *hole*.

This is called a sparse file because the filesystem can represent the hole without allocating blocks for all those zero bytes. Reading the hole behaves as though it contains zeros, but the program never stored them individually. The apparent size is `5 + 1,048,576 + 3 = 1,048,584` bytes; `du -h sparse.bin` will normally report much less physical disk usage on a filesystem that supports sparse files.

## Task 6 - Replacing line 4 without losing data

The file begins with numbers 1 through 10, each followed by `\n`. I cannot safely write `100\n` directly over `4\n`: the replacement uses **four bytes instead of two**, so it would damage the beginning of line 5.

To find the exact location, I read one character at a time and count the first **three newline characters**. The position immediately after them is the start of line 4, which I save with `lseek(..., SEEK_CUR)`. I skip the old fourth line, read lines 5-10 into a buffer, and seek back to the saved position. Writing `100\n` followed by the saved remainder shifts those later lines forward correctly. Finally, `ftruncate()` sets the ending size, and I return to offset zero to display the final file. Only line 4 changes.

## Task 7 - Finding the first different byte

Both files are opened read-only and read into separate **256-byte buffers**. Comparing individual bytes is important because files are not necessarily C strings and may contain zero bytes. A counter tracks how many bytes have already matched in earlier blocks.

Within each block, I compare bytes at the same index. If a pair differs, I report the running count plus that index: this is the first different **zero-based** byte position. If the bytes so far match but one read returns fewer bytes, the shorter file ends first, so that end position is the first difference. If both reads reach EOF together without a mismatch, the files are identical. The program exits with `0` for identical files and a nonzero code for different files.

## What these exercises demonstrate

The same small set of system calls can perform several different file operations. `open()` selects how a file can be accessed, `read()` and `write()` transfer its bytes, `lseek()` controls the next position, and `ftruncate()` changes the actual length. The exercises also show why we must check return values: the number of bytes processed is not always the number requested.

## Tests on Server

<img width="544" height="137" alt="Screenshot 2026-11-11 at 23 03 51" src="https://github.com/user-attachments/assets/5be262e2-cbd9-4419-a060-5ecf36dd520e" />

<img width="438" height="118" alt="Screenshot 2026-11-11 at 23 04 50" src="https://github.com/user-attachments/assets/01fcf51e-625d-4b73-a6a3-4856b7c4e7bf" />

<img width="367" height="102" alt="Screenshot 2026-11-11 at 23 06 04" src="https://github.com/user-attachments/assets/212a2b99-5a4b-475a-81ac-548d1606fa2d" />

<img width="391" height="99" alt="Screenshot 2026-11-11 at 23 07 10" src="https://github.com/user-attachments/assets/09ec727d-26d3-451b-a644-2f218931bce5" />

<img width="391" height="38" alt="Screenshot 2026-11-11 at 23 08 08" src="https://github.com/user-attachments/assets/7615813e-410b-4ec5-999c-94163ad35f21" />

<img width="391" height="228" alt="Screenshot 2026-11-11 at 23 13 15" src="https://github.com/user-attachments/assets/163822d0-da40-48e8-8d23-b390229902e6" />

<img width="391" height="152" alt="Screenshot 2026-11-11 at 23 14 47" src="https://github.com/user-attachments/assets/c90cfcc3-2ad6-48d1-889b-60dd891f887c" />
