/* �R���\�[���֌W */

#include "bootpack.h"
#include <stdio.h>
#include <string.h>

unsigned int addrlist[100];
unsigned int sizelist[100];
int num = 0;


void console_task(struct SHEET *sheet, int memtotal)
{
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	struct FILEHANDLE fhandle[8];
	char cmdline[30];
	unsigned char *nihongo = (char *) *((int *) 0x0fe8);

	cons.sht = sheet;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	task->cons = &cons;
	task->cmdline = cmdline;

	if (cons.sht != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	for (i = 0; i < 8; i++) {
		fhandle[i].buf = 0;	/* ���g�p�}�[�N */
	}
	task->fhandle = fhandle;
	task->fat = fat;
	if (nihongo[4096] != 0xff) {	/* ���{���t�H���g�t�@�C�����ǂݍ��߂����H */
		task->langmode = 1;
	} else {
		task->langmode = 0;
	}
	task->langbyte1 = 0;

	/* �v�����v�g�\�� */
	cons_putchar(&cons, '>', 1);

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) { /* �J�[�\���p�^�C�} */
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0); /* ����0�� */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_FFFFFF;
					}
				} else {
					timer_init(cons.timer, &task->fifo, 1); /* ����1�� */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_000000;
					}
				}
				timer_settime(cons.timer, 50);
			}
			if (i == 2) {	/* �J�[�\��ON */
				cons.cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* �J�[�\��OFF */
				if (cons.sht != 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL8_000000,
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				cons.cur_c = -1;
			}
			if (i == 4) {	/* �R���\�[���́u�~�v�{�^���N���b�N */
				cmd_exit(&cons, fat);
			}
			if (256 <= i && i <= 511) { /* �L�[�{�[�h�f�[�^�i�^�X�NA�o�R�j */
				if (i == 8 + 256) {
					/* �o�b�N�X�y�[�X */
					if (cons.cur_x > 16) {
						/* �J�[�\�����X�y�[�X�ŏ����Ă����A�J�[�\����1�߂� */
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) {
					/* Enter */
					/* �J�[�\�����X�y�[�X�ŏ����Ă������s���� */
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);	/* �R�}���h���s */
					if (cons.sht == 0) {
						cmd_exit(&cons, fat);
					}
					/* �v�����v�g�\�� */
					cons_putchar(&cons, '>', 1);
				} else {
					/* ���ʕ��� */
					if (cons.cur_x < 240) {
						/* �ꕶ���\�����Ă����A�J�[�\����1�i�߂� */
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			/* �J�[�\���ĕ\�� */
			if (cons.sht != 0) {
				if (cons.cur_c >= 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, cons.cur_c,
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				sheet_refresh(cons.sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	/* �^�u */
		for (;;) {
			if (cons->sht != 0) {
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			}
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 496) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				break;	/* 32�Ŋ����؂ꂽ��break */
			}
		}
	} else if (s[0] == 0x0a) {	/* ���s */
		cons_newline(cons);
	} else if (s[0] == 0x0d) {	/* ���A */
		/* �Ƃ肠�����Ȃɂ����Ȃ� */
	} else {	/* ���ʂ̕��� */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		}
		if (move != 0) {
			/* move��0�̂Ƃ��̓J�[�\�����i�߂Ȃ� */
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 496) {
				cons_newline(cons);
			}
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	struct TASK *task = task_now();
	if (cons->cur_y < 28 + 272) {
		cons->cur_y += 16; /* ���̍s�� */
	} else {
		/* �X�N���[�� */
		if (sheet != 0) {
			for (y = 28; y < 28 + 272; y++) {
				for (x = 8; x < 8 + 496; x++) {
					sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
				}
			}
			for (y = 28 + 272; y < 28 + 288; y++) {
				for (x = 8; x < 8 + 496; x++) {
					sheet->buf[x + y * sheet->bxsize] = COL8_000000;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 496, 28 + 288);
		}
	}
	cons->cur_x = 8;
	if (task->langmode == 1 && task->langbyte1 != 0) {
		cons->cur_x = 16;
	}
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s)
{
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l)
{
	int i;
	for (i = 0; i < l; i++) {
		cons_putchar(cons, s[i], 1);
	}
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, int memtotal)
{
	if (strcmp(cmdline, "mem") == 0 && cons->sht != 0) {
		cmd_mem(cons, memtotal);
	} else if (strcmp(cmdline, "cls") == 0 && cons->sht != 0) {
		cmd_cls(cons);
	} else if (strcmp(cmdline, "dir") == 0 && cons->sht != 0) {
		cmd_dir(cons);
	} else if (strcmp(cmdline, "exit") == 0) {
		cmd_exit(cons, fat);
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		cmd_start(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "ncst ", 5) == 0) {
		cmd_ncst(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "langmode ", 9) == 0) {
		cmd_langmode(cons, cmdline);
	}else if (strcmp(cmdline, "reader")== 0) {
		cmd_reader();
	} else if (strcmp(cmdline, "writer")== 0) {
		cmd_writer();
	}else if (strncmp(cmdline, "alloc", 5)== 0) {
		cmd_mymem(cmdline);
	}
	else if(strcmp(cmdline,"free")==0){
		cmd_free();
	}
	else if (strcmp(cmdline, "1") == 0 && cons->sht != 0) {
		produce(cons);
	}else if (strcmp(cmdline, "2") == 0 && cons->sht != 0) {
		consume(cons);
	}else if (strcmp(cmdline, "add") == 0 && cons->sht != 0) {
		shareadd(cons);
	}
	else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* �R�}���h�ł͂Ȃ��A�A�v���ł��Ȃ��A�����ɋ��s�ł��Ȃ� */
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}

void cmd_mem(struct CONSOLE *cons, int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[60];
	sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 288; y++) {
		for (x = 8; x < 8 + 496; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 496, 28 + 288);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[30];
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				sprintf(s, "filename.ext   %7d\n", finfo[i].size);
				for (j = 0; j < 8; j++) {
					s[j] = finfo[i].name[j];
				}
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0x0fec);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0) {
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);	/* 768�`1023 */
	} else {
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);	/* 1024�`2023 */
	}
	io_sti();
	for (;;) {
		task_sleep(task);
	}
}

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* �R�}���h���C���ɓ��͂��ꂽ���������A�ꕶ�����V�����R���\�[���ɓ��� */
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	/* �R�}���h���C���ɓ��͂��ꂽ���������A�ꕶ�����V�����R���\�[���ɓ��� */
	for (i = 5; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_langmode(struct CONSOLE *cons, char *cmdline)
{
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 2) {
		task->langmode = mode;
	} else {
		cons_putstr0(cons, "mode number error.\n");
	}
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	char name[18], *p, *q;
	struct TASK *task = task_now();
	int i, segsiz, datsiz, esp, dathrb, appsiz;
	struct SHTCTL *shtctl;
	struct SHEET *sht;

	/* �R�}���h���C�������t�@�C�����𐶐� */
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0; /* �Ƃ肠�����t�@�C�����̌�����0�ɂ��� */

	/* �t�@�C�����T�� */
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		/* �������Ȃ������̂Ō�����".HRB"�����Ă������x�T���Ă݂� */
		name[i    ] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo != 0) {
		/* �t�@�C�������������ꍇ */
		appsiz = finfo->size;
		p = file_loadfile2(finfo->clustno, &appsiz, fat);
		if (appsiz >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {
			segsiz = *((int *) (p + 0x0000));
			esp    = *((int *) (p + 0x000c));
			datsiz = *((int *) (p + 0x0010));
			dathrb = *((int *) (p + 0x0014));
			q = (char *) memman_alloc_4k(memman, segsiz);
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, appsiz - 1, (int) p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsiz - 1, (int) q, AR_DATA32_RW + 0x60);
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[dathrb + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & 0x11) == 0x11 && sht->task == task) {
					/* �A�v�����J�����ςȂ��ɂ����������𔭌� */
					sheet_free(sht);	/* ���� */
				}
			}
			for (i = 0; i < 8; i++) {	/* �N���[�Y���ĂȂ��t�@�C�����N���[�Y */
				if (task->fhandle[i].buf != 0) {
					memman_free_4k(memman, (int) task->fhandle[i].buf, task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}
			timer_cancelall(&task->fifo);
			memman_free_4k(memman, (int) q, segsiz);
			task->langbyte1 = 0;
		} else {
			cons_putstr0(cons, ".hrb file format error.\n");
		}
		memman_free_4k(memman, (int) p, appsiz);
		cons_newline(cons);
		return 1;
	}
	/* �t�@�C�����������Ȃ������ꍇ */
	return 0;
}

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_now();
	int ds_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) 0x0fec);
	int *reg = &eax + 1;	/* eax�̎��̔Ԓn */
		/* �ۑ��̂��߂�PUSHAD�������ɏ��������� */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */
	int i;
	struct FILEINFO *finfo;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

	if (edx == 1) {
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		cons_putstr0(cons, (char *) ebx + ds_base);
	} else if (edx == 3) {
		cons_putstr1(cons, (char *) ebx + ds_base, ecx);
	} else if (edx == 4) {
		return &(task->tss.esp0);
	} else if (edx == 5) {
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= 0x10;
		sheet_setbuf(sht, (char *) ebx + ds_base, esi, edi, eax);
		make_window8((char *) ebx + ds_base, esi, edi, (char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* ���̃}�E�X�Ɠ��������ɂȂ��悤�Ɏw���F �}�E�X�͂��̏��ɂȂ� */
		reg[7] = (int) sht;
	} else if (edx == 6) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0;	/* 16�o�C�g�P�ʂ� */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 16�o�C�g�P�ʂɐ؂��グ */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx,0);
	} else if (edx == 10) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 16�o�C�g�P�ʂɐ؂��グ */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		hrb_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0) {
			if (eax > esi) {
				i = eax;
				eax = esi;
				esi = i;
			}
			if (ecx > edi) {
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) {
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					task_sleep(task);	/* FIFO�����Ȃ̂ŐQ�đ҂� */
				} else {
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons->sht != 0) { /* �J�[�\���p�^�C�} */
				/* �A�v�����s���̓J�[�\�����o�Ȃ��̂ŁA�������͕\���p��1�𒍕����Ă��� */
				timer_init(cons->timer, &task->fifo, 1); /* ����1�� */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* �J�[�\��ON */
				cons->cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* �J�[�\��OFF */
				cons->cur_c = -1;
			}
			if (i == 4) {	/* �R���\�[������������ */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);	/* 2024�`2279 */
				cons->sht = 0;
				io_sti();
			}
			if (i >= 256) { /* �L�[�{�[�h�f�[�^�i�^�X�NA�o�R�j�Ȃ� */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1;	/* �����L�����Z���L�� */
	} else if (edx == 17) {
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0) {
				break;
			}
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char *) ebx + ds_base,
					(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
			if (finfo != 0) {
				reg[7] = (int) fh;
				fh->size = finfo->size;
				fh->pos = 0;
				fh->buf = file_loadfile2(finfo->clustno, &fh->size, task->fat);
			}
		}
	} else if (edx == 22) {
		fh = (struct FILEHANDLE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			fh->pos = ebx;
		} else if (ecx == 1) {
			fh->pos += ebx;
		} else if (ecx == 2) {
			fh->pos = fh->size + ebx;
		}
		if (fh->pos < 0) {
			fh->pos = 0;
		}
		if (fh->pos > fh->size) {
			fh->pos = fh->size;
		}
	} else if (edx == 24) {
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		fh = (struct FILEHANDLE *) eax;
		for (i = 0; i < ecx; i++) {
			if (fh->pos == fh->size) {
				break;
			}
			*((char *) ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) {
		i = 0;
		for (;;) {
			*((char *) ebx + ds_base + i) =  task->cmdline[i];
			if (task->cmdline[i] == 0) {
				break;
			}
			if (i >= ecx) {
				break;
			}
			i++;
		}
		reg[7] = i;
	} else if (edx == 27) {
		reg[7] = task->langmode;
	}
	else if(edx == 28){
	    char s[100];
	    int gdt_addr = ADR_GDT;
	    int ldt_addr_offset = task->tss.ldtr;
	    struct SEGMENT_DESCRIPTOR temp =  *(struct SEGMENT_DESCRIPTOR *)(ADR_GDT + ldt_addr_offset);
	    int ldt_addr = temp.base_low +(temp.base_mid << 16) + (temp.base_high<<24);
	    temp = *(struct SEGMENT_DESCRIPTOR *)(ldt_addr + 1*8);
	    int ds_base = temp.base_low +(temp.base_mid << 16) + (temp.base_high<<24);
	    int phy_addr = ds_base + eax;
	    sprintf(s,"gdt_base: %x\nldt_base: %x\nds_base: %x\nlog_addr: %x\nphy_addr: %x\nvalue: %d\n",gdt_addr,ldt_addr,ds_base,eax,phy_addr,*(int *)(phy_addr));
	    //int addr = task->ldt[1].base_low + (task->ldt[1].base_mid<<16) +(task->ldt[1].base_high<<24);
	    //sprintf(s,"%x %x %x %x %d\n",addr,task->ds_base,eax,addr+eax,*(int *)(addr+eax));
	    cons_putstr0(cons, s);
		//int i = ADR_GDT +task->tss.ldtr;
		//struct SEGMENT_DESCRIPTOR* des = *(ADR_GDT +task->tss.ldtr);
		//i = des->base_low + des->base_mid <<8 + des->base_high<<12;
	  	reg[7] = phy_addr;
	}
	else if(edx == 29){
	     char s[100];
        int gdt = *(int *)(ds_base+eax+2);
        int ldt_index = ecx >>3;
        int ldt_des = gdt + ldt_index*8;
        struct SEGMENT_DESCRIPTOR temp =  *(struct SEGMENT_DESCRIPTOR *)(ldt_des);
        int ldt_addr = temp.base_low +(temp.base_mid << 16) + (temp.base_high<<24);
        int ds_index = ebx>>3;
        int ds_des = ldt_addr + ds_index*8;
        temp = *(struct SEGMENT_DESCRIPTOR *)(ds_des);
        int ds_addr = temp.base_low +(temp.base_mid << 16) + (temp.base_high<<24);
        int phy_addr = ds_addr + ebp;
        sprintf(s,"gdt_base: %x  ldt_index: %d\nldt_des: %x  ldt_addr: %x\nds_index: %d  ds_des: %x\nds_addr: %x  log_addr: %x\nphy_addr:%x  value: %d\n",gdt,ldt_index,ldt_des,ldt_addr,ds_index,ds_des,ds_addr,ebp,phy_addr,*(int *)phy_addr);
        cons_putstr0(cons, s);
	}
	else if(edx == 30){
		cmd_reader();
	}
	else if(edx == 31){
		cmd_writer();
	}
	else if(edx==32){
    shareadd(cons);
	}else if(edx==33){
		consume(cons);
	}else if(edx==34){
		produce(cons);
	}else if(edx==35){
		entrance(eax);
	}else if(edx==36){
		exiting(eax);
	}else if(edx==37){
		reg[7]=var_create((char*)ds_base+ebx,ecx);
	}else if(edx==38){
		reg[7]=var_read((char*)ds_base+ebx,ecx);
	}else if(edx==39){
		reg[7]=var_wrt((char*)ds_base+ebx,ecx,eax);
	}else if(edx==40){
		reg[7]=var_free((char*)ds_base+ebx);
	}else if(edx==41){
		avoid_sleep();
	}else if(edx==42){
		Tlock();
	}else if(edx==43){
		unTlock();
	}


	return 0;
}

int *inthandler0c(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0C :\n Stack Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* �ُ��I�������� */
}

int *inthandler0d(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0D :\n General Protected Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* �ُ��I�������� */
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}

	return;
}


void wait(struct S *s,struct process *this_process,char * which_s){
	char buf[100];
	int eflags= io_load_eflags();
	io_cli();
	s->value--;
	if(s->value<0){
		//��ʾ˭�ڵȴ�
		sprintf(buf, "%s is waiting %s!\n", this_process->name, which_s);
	    cons_putstr0(this_process->task->cons, buf);

		//�����ȴ��б�
		if(s->list_last==NULL){//�б���
			s->list_first=this_process;
			s->list_last=this_process;
			s->list_last->next=NULL;
		}
		else{//�б�����
			s->list_last->next=this_process;
			s->list_last=s->list_last->next;
			s->list_last->next=NULL;
		}
		//����
		task_sleep(this_process->task);
	}
	io_store_eflags(eflags);
}

void signal(struct S *s,char * which_s){
	struct process *temp;
	char buf[100];
	int eflags= io_load_eflags();
	io_cli();
	s->value++;
	if(s->value<=0){
		//�б�
		temp=s->list_first;

		if(s->list_first==s->list_last){//ֻ��һ�������ڵȴ������Ѻ��ȴ��б���
			s->list_first=s->list_last=NULL;
		}
		else
			s->list_first=s->list_first->next;
		//
		task_run(temp->task,-1,-1);//����ԭ����Ĭ�����ȼ�

		sprintf(buf, "%s already get %s.\n", temp->name, which_s);
	    cons_putstr0(temp->task->cons, buf);

	}
	io_store_eflags(eflags);
}

void init_S(){
	mutex.value=1;
	mutex.list_first=NULL;
	mutex.list_last=NULL;
	wrt.value=1;
	wrt.list_first=NULL;
	wrt.list_last=NULL;
	readcount=0;
	share_bupt=0;
}
#define END 1000
void cmd_reader(){
	char readbuf[100];
	int if_end=0;
	struct process this_process;
	this_process.next=NULL;
	this_process.task=task_now();
	while(1){
		sprintf(this_process.name,"reader %d",readcount+1);
		wait(&mutex,&this_process,"mutex");
		readcount++;
		if(readcount==1){
			wait(&wrt,&this_process,"wrt");
		}
		signal(&mutex,"mutex");
		//�������������ٽ���
		sprintf(readbuf,"%s get share=%d|| %d\n",this_process.name,share_bupt,if_end+1);
		cons_putstr0(this_process.task->cons, readbuf);

		wait(&mutex,&this_process,"mutex");
		readcount--;
		if(readcount==0){
			signal(&wrt,"wrt");
		}
		signal(&mutex,"mutex");

		if_end++;
		if(if_end==END)
			break;
	}
}
void cmd_writer(){
	char writebuf[100];
	int if_end=0;
	struct process this_process;
	this_process.next=NULL;
	this_process.task=task_now();
	sprintf(this_process.name,"writer");

	while(1){
		wait(&wrt,&this_process,"wrt");

		share_bupt++;
		sprintf(writebuf,"%s have written share=%d|| %d\n",this_process.name,share_bupt,if_end+1);
		cons_putstr0(this_process.task->cons, writebuf);

		signal(&wrt,"wrt");

		if_end++;
		if(if_end==END)
			break;
	}
}

unsigned int char2int(char *cSize){//��char����ת��Ϊint������
	unsigned int iSize=0;
	char c;
	int i=0;
	while((c=cSize[i])!='\0'){
		iSize=iSize*10+c-'0';
		i++;
	}
	return iSize;
}
void cmd_mymem(char *cmdline){//������alloc�ڴ棺alloc mode size
	char cSize[10];
	int mode;//�����ڴ���ģʽ����0����1����

	unsigned int iSize;
	int i;
	unsigned int addr;

	char memsizebuf[100];

	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task=task_now();
	//
	for(i=0;cmdline[i]!=' ';i++);

	mode=cmdline[i+1]-'0';//ȡ��ģʽ

	strcpy(cSize,&cmdline[i+3]);//ȡ����С
	iSize=char2int(cSize);
	//allocǰ��ʾ������С
	sprintf(memsizebuf,"before alloc %d:\n",iSize);
	cons_putstr0(task->cons,memsizebuf);
	for (i = 0; i < memman->frees; i++) {
		sprintf(memsizebuf,"NO.%d-size=%d  ",i,memman->free[i].size);
		cons_putstr0(task->cons,memsizebuf);
	}
	//alloc
	addr=memman_alloc((struct MEMMAN *) MEMMAN_ADDR,iSize,mode);
	//��alloc�Ŀ鶼�ǵ������freeʹ��
	addrlist[num]=addr;
	sizelist[num]=iSize;
	num++;
	//alloc֮����ʾ������С
	cons_putstr0(task->cons,"\nafter alloc:\n");
	for (i = 0; i < memman->frees; i++) {
		sprintf(memsizebuf,"NO.%d-size=%d  ",i,memman->free[i].size);
		cons_putstr0(task->cons,memsizebuf);
	}
	cons_putstr0(task->cons,"\n");
	//memman_free((struct MEMMAN *) MEMMAN_ADDR, addr,iSize);
}

void cmd_free(){//free֮ǰ��������alloc���ڴ�
	int i;
	for(i=0;i<num;i++){
		memman_free((struct MEMMAN *) MEMMAN_ADDR, addrlist[i],sizelist[i]);
	}
}

int share=0;
int sharenum=0;//ʹ�ù��������Ľ�������
void shareadd(struct CONSOLE *cons)
{
	int i,j,x,temp,e;
	char s[60];
	struct TASK *now_task;

	if(sharenum==0)
	   share=0;
	sharenum+=1;
	while(sharenum<2)
	{
		now_task=task_now();
		now_task->flags=2;
	}
	for(;;)
	{
		temp=share;
		e=io_load_eflags();
		io_cli();
		share+=1;
		io_store_eflags(e);
		x=share;
		if((x-temp)>1)
		{
		sprintf(s,"share=%d,but share+1=%d\n",temp,x);
		cons_putstr0(cons,s);
		sharenum-=1;
		break;
		}
	}
	return ;
}

//�����ߺ������߳���������������peterson�㷨
#define BUFFER_SIZE 100
int producer=0,consumer=1;
int flag[2]={0,0};//��ʾ�ĸ������������ٽ���
int turn;//��ʾ�ĸ����̿��Խ����ٽ���
int in=0,out=0,counter=0;
int buffer[BUFFER_SIZE];//������
int xnum=0;

void produce(struct CONSOLE *cons)
{
	char *s;
	int temp,outcome,e;
	struct TASK *now_task;
	now_task=task_now();
	while(1){
		while (counter == BUFFER_SIZE)
		{
			now_task=task_now();//����ѭ��������ֹ��������
			now_task->flags=2;
		}
	    // xnum++;
		   flag[producer]=1;
		   turn=consumer;
	   	 while(flag[consumer]==1&&turn==consumer)
		   xnum++;
		  //�ٽ���
			 temp=in;
			 outcome=rand();
		   buffer[in]=outcome;
		   in = (in + 1)%BUFFER_SIZE;
			 counter++;
		   flag[producer]=0;
			 //ʣ����
			 sprintf(s,"in buffer %d,produce %d\n",temp+1,outcome);
			 cons_putstr0(cons,s);
	}
	return ;
}

void consume(struct CONSOLE *cons)
{
	char *s;
	int temp,outcome,e;
	struct TASK *now_task;
	while(1){
		while (counter==0)
		{
			now_task=task_now();
			now_task->flags=2;
		}
		  flag[consumer]=1;
		  turn=producer;
		  while(flag[producer]==1&&turn==producer)
		  xnum++;
		  //�ٽ���
			temp=out;
			outcome=buffer[out];
			out=(out+1)%BUFFER_SIZE;
			counter--;
			flag[consumer]=0;
			//ʣ����
			sprintf(s,"in buffer %d,consume %d\n",temp+1,outcome);
			cons_putstr0(cons,s);
	}
	return ;
}

//peterson算法的进入区和退出区，这2个可以当成互斥锁来使用，但仅能用于2个进程之间
void entrance(int x)
{
	if(x==1)
	{
		flag[consumer]=1;
		turn=producer;
		while(flag[producer]==1&&turn==producer)
		xnum++;
	}
	if(x==0)
	{
		flag[producer]=1;
		turn=consumer;
		while(flag[consumer]==1&&turn==consumer)
		xnum++;
	}
}

void exiting(int x)
{
   if(x==1)
	 {
		flag[consumer]=0;
	 }
	 if(x==0)
	 {
		 flag[producer]=0;
	 }
}

/*
int metux=1;

void entrance()
{
	while(metux<=0)
	;
	metux--;
}

void exit()
{
	metux++;
}
*/
