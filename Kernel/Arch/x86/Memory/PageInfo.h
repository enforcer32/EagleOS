#pragma once

#define PAGE_DIRECTORY_TABLE_COUNT 1024
#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_SIZE 4096

#define PAGE_DIRECTORY_TABLE_INDEX(vaddr)	((uint32_t)vaddr / (PAGE_DIRECTORY_TABLE_COUNT * PAGE_SIZE))
#define PAGE_TABLE_ENTRY_INDEX(vaddr)		((uint32_t)vaddr % (PAGE_TABLE_ENTRY_COUNT * PAGE_SIZE) / PAGE_SIZE)
#define PAGE_TABLE_PHYSICAL_ADDRESS(dirent) ((*dirent) & ~0xFFF)
