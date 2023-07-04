#include <stddef.h>

#include <lv2/lv2.h>
#include <debug.h>
#include <lv2/memory.h>
#include <lv2/io.h>
#include <lv2/libc.h>
#include <lv2/thread.h>
#include <lv2/patch.h>

#include <lv1/lv1.h>


#ifdef DEBUG
#define DPRINTF		_debug_printf
#define DPRINT_HEX	debug_print_hex
#else
#define DPRINTF(...)
#define DPRINT_HEX(a, b)
#endif

#define STAGE2_FILE	"/dev_flash/rebug/cobra/stage2.dex.debug"
//#define STAGE2_FILE	"/dev_flash/stage2.dex.debug"
//#define STAGE2_FILE	"/dev_hdd0/stage2.dex"
//#define STAGE2_FILE	"/dev_usb001/stage2.dex"

extern int flash_mount_clone(void) __asm__(".flash_mount_clone");
//int flash_mount_clone(void);

int main(void)
{
	void *stage2 = NULL;

	f_desc_t f;
	//f_desc_t *fp;
	int (* func)(void);
	int ret;

#ifdef DEBUG
	debug_init();
	DPRINTF("Stage 1 hello.\n");
#endif
	f.addr = &flash_mount_clone;
	f.toc = (void *)MKA(TOC);
	func = (void *)&f;
	ret = func();

  //fp = (f_desc_t *)&flash_mount_clone;
  //fp->toc = (void *)MKA(TOC);
  //ret = flash_mount_clone();

	if (ret != 0 && ret != 1)
	{
		DPRINTF("Flash mount failed!\n");
	}
	else
	{
		CellFsStat stat;

		DPRINTF("Flash mounted\n");

		if (cellFsStat(STAGE2_FILE, &stat) == 0)
		{
			int fd;

			if (cellFsOpen(STAGE2_FILE, CELL_FS_O_RDONLY, &fd, 0, NULL, 0) == 0)
			{
				uint64_t psize = stat.st_size;

				DPRINTF("stage2 location: %s\nPayload size = %d\n", STAGE2_FILE, psize);

				stage2 = (void *)alloc(psize, 0x27);
				if (stage2)
				{
					DPRINTF("Allocated for stage2\n");
					uint64_t rs;

					cellFsRead(fd, stage2, psize, &rs);
					//if (cellFsRead(fd, stage2, psize, &rs) != 0)
					if (rs != psize)
					{
						DPRINTF("Stage 2 read fail.\n");
						dealloc(stage2, 0x27);
						stage2 = NULL;
					}
				  DPRINTF("stage2: 0x%02llX, *stage2: 0x%02llX, **stage2: 0x%02llX\n", (uint64_t)(&stage2), *(uint64_t *)(&stage2), **(uint64_t **)(&stage2));
				}
				else
				{
					DPRINTF("Cannot allocate stage2\n");
				}

				cellFsClose(fd);
			}
		}
		else
		{
			DPRINTF("There is no stage 2, booting system.\n");
		}
	}

	if (stage2)
	{
    //f_desc_t *fp;
    //fp = stage2;
		//DPRINTF("fp->addr: 0x%02llX, fp->addr[0]: 0x%02llX\n", (uint64_t)(fp->addr) *(uint64_t)(fp->addr));
		f.addr = (void *)(*(uint64_t *)&stage2);
	  f.toc = (void *)MKA(TOC);
		func = (void *)&f;
    //fp = (f_desc_t *)
		//DPRINTF("Calling stage 2 at 0x%02llX, func[0]: 0x%02llX\n", (uint64_t)(f.addr), **(uint64_t **)(f.addr));
		DPRINTF("Calling stage 2: f.addr: 0x%02llX, *(f.addr): 0x%02llX\n", (uint64_t)(f.addr), *(uint64_t *)(f.addr));
		ret = func();
	  DPRINTF("Returned from stage 2: %d\n", ret);
	}

	DPRINTF("Returning from stage 1: %d\n", ret);
	return ret;
}
