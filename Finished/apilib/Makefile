OBJS_API =	api001.obj api002.obj api003.obj api004.obj api005.obj api006.obj \
			api007.obj api008.obj api009.obj api010.obj api011.obj api012.obj \
			api013.obj api014.obj api015.obj api016.obj api017.obj api018.obj \
			api019.obj api020.obj api021.obj api022.obj api023.obj api024.obj \
			api025.obj api026.obj api027.obj api028.obj api029.obj api030.obj \
			api031.obj api032.obj api033.obj api034.obj api035.obj api036.obj \
			api037.obj api038.obj api039.obj api040.obj api041.obj api042.obj \
			api043.obj alloca.obj

TOOLPATH = ../../z_tools/
INCPATH  = ../../z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = ../haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
GOLIB    = $(TOOLPATH)golib00.exe
COPY     = copy
DEL      = del

# �f�t�H���g����

default :
	$(MAKE) apilib.lib

# �t�@�C�������K��

apilib.lib : Makefile $(OBJS_API)
	$(GOLIB) $(OBJS_API) out:apilib.lib

# ���ʋK��

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# �R�}���h

clean :
	-$(DEL) *.lst
	-$(DEL) *.obj

src_only :
	$(MAKE) clean
	-$(DEL) apilib.lib
