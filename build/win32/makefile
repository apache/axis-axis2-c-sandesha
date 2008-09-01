####################################################
# Makefile for Sandesha2			   #
# you can do 			                   #
# nmake dist     - distribution  (dist / samples ) #
# nmake clean  	 - clean	                   #
# nmake samples  - samples 			   #
####################################################

AUTOCONF = configure.in
!include $(AUTOCONF)

#define folders 

SANDESHASRC = ..\..
SANDESHADISTDIR = ..\sandesha2
SANDESHA2_INTDIR = .\int.msvc
SANDESHA_DLL = sandesha2
SANDESHA_CLIENT = sandesha2_client

#compiler options
CC = @cl.exe
CFLAGS = /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "AXIS2_DECLARE_EXPORT" /D "_WIN32_WINNT=0x0400" /w /nologo \
	/I$(AXIS2_BIN_DIR)\include  /I$(SANDESHASRC)\include 

#linker options 
LD = @link.exe
LDFLAGS = /nologo /LIBPATH:$(AXIS2_BIN_DIR)\lib /LIBPATH:$(SANDESHADISTDIR)\modules\sandesha2 \
	/LIBPATH:$(SANDESHADISTDIR)\lib
LIBS = axutil.lib axiom.lib axis2_parser.lib axis2_engine.lib axis2_http_receiver.lib axis2_http_sender.lib


#debug symbols
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) /D "_DEBUG" /Od /Z7 $(CRUNTIME)d
LDFLAGS = $(LDFLAGS) /DEBUG  
!else
CFLAGS = $(CFLAGS) /D "NDEBUG" /O2 $(CRUNTIME)
LDFLAGS = $(LDFLAGS) /LIBPATH:$(MYSQL_BIN_DIR)\lib\opt
!endif

######################
MT=mt.exe
MT="$(MT)"

!if "$(EMBED_MANIFEST)" == "0"
_VC_MANIFEST_EMBED_EXE=
_VC_MANIFEST_EMBED_DLL=
!else
_VC_MANIFEST_EMBED_EXE= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;1
_VC_MANIFEST_EMBED_DLL= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;2
!endif
#######################


#create the directory structure

distdir:
	if not exist $(SANDESHADISTDIR) mkdir $(SANDESHADISTDIR)
	if not exist $(SANDESHADISTDIR)\modules\sandesha2 mkdir $(SANDESHADISTDIR)\modules\sandesha2
	if not exist $(SANDESHADISTDIR)\lib mkdir $(SANDESHADISTDIR)\lib
	if not exist $(SANDESHADISTDIR)\include mkdir $(SANDESHADISTDIR)\include
	if not exist $(SANDESHADISTDIR)\samples mkdir $(SANDESHADISTDIR)\samples
	if not exist $(SANDESHADISTDIR)\bin\samples mkdir $(SANDESHADISTDIR)\bin\samples
	if not exist $(SANDESHADISTDIR)\bin\samples\sandesha2 mkdir $(SANDESHADISTDIR)\bin\samples\sandesha2
	if not exist $(SANDESHADISTDIR)\services\RMSampleService mkdir $(SANDESHADISTDIR)\services\RMSampleService
	if not exist $(SANDESHADISTDIR)\config mkdir $(SANDESHADISTDIR)\config
	if not exist $(SANDESHADISTDIR)\docs mkdir $(SANDESHADISTDIR)\docs


intdir:
	if not exist $(SANDESHA2_INTDIR) mkdir $(SANDESHA2_INTDIR)
	if not exist $(SANDESHA2_INTDIR)\samples mkdir $(SANDESHA2_INTDIR)\samples
	if not exist $(SANDESHA2_INTDIR)\samples\rm_echo_1_0 mkdir $(SANDESHA2_INTDIR)\samples\rm_echo_1_0
	if not exist $(SANDESHA2_INTDIR)\samples\rm_echo_1_1 mkdir $(SANDESHA2_INTDIR)\samples\rm_echo_1_1
	if not exist $(SANDESHA2_INTDIR)\samples\rm_echo_single_1_0 mkdir $(SANDESHA2_INTDIR)\samples\rm_echo_single_1_0
	if not exist $(SANDESHA2_INTDIR)\samples\rm_echo_single_1_1 mkdir $(SANDESHA2_INTDIR)\samples\rm_echo_single_1_1
	if not exist $(SANDESHA2_INTDIR)\samples\rm_mtom_1_0 mkdir $(SANDESHA2_INTDIR)\samples\rm_mtom_1_0
	if not exist $(SANDESHA2_INTDIR)\samples\rm_ping_1_0 mkdir $(SANDESHA2_INTDIR)\samples\rm_ping_1_0 
	if not exist $(SANDESHA2_INTDIR)\samples\rm_ping_1_1 mkdir $(SANDESHA2_INTDIR)\samples\rm_Ping_1_1
	if not exist $(SANDESHA2_INTDIR)\samples\rm_report mkdir $(SANDESHA2_INTDIR)\samples\rm_report
	if not exist $(SANDESHA2_INTDIR)\samples\RMSampleService mkdir $(SANDESHA2_INTDIR)\samples\RMSampleService

clean :
	@if exist $(SANDESHADISTDIR) rmdir /S /Q $(SANDESHADISTDIR)
	@if exist $(SANDESHA2_INTDIR) rmdir /S /Q $(SANDESHA2_INTDIR)

distclean :
	@if exist $(SANDESHADISTDIR) rmdir /S /Q $(SANDESHADISTDIR)
	@if exist $(SANDESHA2_INTDIR) rmdir /S /Q $(SANDESHA2_INTDIR)


copy_extra:
	@copy $(SANDESHASRC)\config\* $(SANDESHADISTDIR)\config
	@copy $(SANDESHASRC)\LICENSE $(SANDESHADISTDIR)
	@copy $(SANDESHASRC)\README $(SANDESHADISTDIR)
	@copy $(SANDESHASRC)\INSTALL $(SANDESHADISTDIR)
	@copy $(SANDESHASRC)\NEWS $(SANDESHADISTDIR)
	if exist $(SANDESHASRC)\docs xcopy /E /I /Y $(SANDESHASRC)\docs $(SANDESHADISTDIR)\docs

copy_samples:
	@xcopy /E $(SANDESHASRC)\samples $(SANDESHADISTDIR)\samples
	@del /s /q $(SANDESHADISTDIR)\samples\*.am
	@del /q $(SANDESHADISTDIR)\samples\*.*

copy_include:
	@xcopy /E $(SANDESHASRC)\include $(SANDESHADISTDIR)\include


SANDESHA2_CODE = $(SANDESHASRC)\src

SANDESHA2_INTDIR1 = $(SANDESHA2_INTDIR)\src\core
SANDESHA2_INTDIR2 = $(SANDESHA2_INTDIR)\src\storage\sqlite
SANDESHA2_INTDIR3 = $(SANDESHA2_INTDIR)\src\storage\beans
SANDESHA2_INTDIR4 = $(SANDESHA2_INTDIR)\src\storage\common
SANDESHA2_INTDIR5 = $(SANDESHA2_INTDIR)\src\transport
SANDESHA2_INTDIR6 = $(SANDESHA2_INTDIR)\src\polling
SANDESHA2_INTDIR7 = $(SANDESHA2_INTDIR)\src\msgprocessors
SANDESHA2_INTDIR8 = $(SANDESHA2_INTDIR)\src\handlers
SANDESHA2_INTDIR9 = $(SANDESHA2_INTDIR)\src\util
SANDESHA2_INTDIR10 = $(SANDESHA2_INTDIR)\src\wsrm

SANDESHA2_OBJS = $(SANDESHA2_INTDIR1)\*.obj \
		 $(SANDESHA2_INTDIR2)\*.obj \
		 $(SANDESHA2_INTDIR3)\*.obj \
		 $(SANDESHA2_INTDIR4)\*.obj \
		 $(SANDESHA2_INTDIR5)\*.obj \
		 $(SANDESHA2_INTDIR6)\*.obj \
		 $(SANDESHA2_INTDIR7)\*.obj \
		 $(SANDESHA2_INTDIR8)\*.obj \
		 $(SANDESHA2_INTDIR9)\*.obj \
		 $(SANDESHA2_INTDIR10)\*.obj 


$(SANDESHA2_INTDIR1) :
	if not exist $(SANDESHA2_INTDIR1) mkdir $(SANDESHA2_INTDIR1)

$(SANDESHA2_INTDIR2) :
	if not exist $(SANDESHA2_INTDIR2) mkdir $(SANDESHA2_INTDIR2)

$(SANDESHA2_INTDIR3) :
	if not exist $(SANDESHA2_INTDIR3) mkdir $(SANDESHA2_INTDIR3)

$(SANDESHA2_INTDIR4) :	
	if not exist $(SANDESHA2_INTDIR4) mkdir $(SANDESHA2_INTDIR4)

$(SANDESHA2_INTDIR5) :
	if not exist $(SANDESHA2_INTDIR5) mkdir $(SANDESHA2_INTDIR5)

$(SANDESHA2_INTDIR6) :
	if not exist $(SANDESHA2_INTDIR6) mkdir $(SANDESHA2_INTDIR6)

$(SANDESHA2_INTDIR7) :
	if not exist $(SANDESHA2_INTDIR7) mkdir $(SANDESHA2_INTDIR7)

$(SANDESHA2_INTDIR8) :
	if not exist $(SANDESHA2_INTDIR8) mkdir $(SANDESHA2_INTDIR8)

$(SANDESHA2_INTDIR9) :
	if not exist $(SANDESHA2_INTDIR9) mkdir $(SANDESHA2_INTDIR9)

$(SANDESHA2_INTDIR10) :
	if not exist $(SANDESHA2_INTDIR10) mkdir $(SANDESHA2_INTDIR10)


{$(SANDESHA2_CODE)\core}.c{$(SANDESHA2_INTDIR1)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR1)\ /c $<

{$(SANDESHA2_CODE)\storage\sqlite}.c{$(SANDESHA2_INTDIR2)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR2)\ /c $<

{$(SANDESHA2_CODE)\storage\beans}.c{$(SANDESHA2_INTDIR3)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR3)\ /c $<

{$(SANDESHA2_CODE)\storage\common}.c{$(SANDESHA2_INTDIR4)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR4)\ /c $<

{$(SANDESHA2_CODE)\transport}.c{$(SANDESHA2_INTDIR5)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR5)\ /c $<

{$(SANDESHA2_CODE)\polling}.c{$(SANDESHA2_INTDIR6)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR6)\ /c $<

{$(SANDESHA2_CODE)\msgprocessors}.c{$(SANDESHA2_INTDIR7)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR7)\ /c $<

{$(SANDESHA2_CODE)\handlers}.c{$(SANDESHA2_INTDIR8)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR8)\ /c $<

{$(SANDESHA2_CODE)\util}.c{$(SANDESHA2_INTDIR9)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR9)\ /c $<

{$(SANDESHA2_CODE)\wsrm}.c{$(SANDESHA2_INTDIR10)}.obj ::
	$(CC) $(CFLAGS) /Fo$(SANDESHA2_INTDIR10)\ /c $<


$(SANDESHA2_OBJS) : $(SANDESHA2_INTDIR1) $(SANDESHA2_INTDIR2) $(SANDESHA2_INTDIR3) \
	$(SANDESHA2_INTDIR4) $(SANDESHA2_INTDIR5) $(SANDESHA2_INTDIR6) $(SANDESHA2_INTDIR7) \
	$(SANDESHA2_INTDIR8) $(SANDESHA2_INTDIR9) $(SANDESHA2_INTDIR10)

$(SANDESHADISTDIR)\modules\sandesha2\$(SANDESHA_DLL).dll : $(SANDESHA2_OBJS)
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_OBJS) /DLL /OUT:$(SANDESHADISTDIR)\modules\sandesha2\$(SANDESHA_DLL).dll \
	/IMPLIB:$(SANDESHADISTDIR)\modules\sandesha2\$(SANDESHA_DLL).lib
	-@$(_VC_MANIFEST_EMBED_DLL)

sandesha2: $(SANDESHADISTDIR)\modules\sandesha2\$(SANDESHA_DLL).dll
	
copy_xml:
	@copy $(SANDESHASRC)\config\module.xml $(SANDESHADISTDIR)\modules\sandesha2\module.xml
	@copy $(SANDESHASRC)\config\sqlite_schema.bat $(SANDESHADISTDIR)\config\
	

### Sandesha2 Client
SANDESHA2_CLIENT_INTDIR = $(SANDESHA2_INTDIR)\src\client

$(SANDESHA2_CLIENT_INTDIR) :
	if not exist $(SANDESHA2_CLIENT_INTDIR) mkdir $(SANDESHA2_CLIENT_INTDIR)

$(SANDESHADISTDIR)\lib\$(SANDESHA_CLIENT).dll : $(SANDESHA2_CLIENT_INTDIR) 
	$(CC) $(CFLAGS) $(SANDESHASRC)\src\client\*.c /Fo$(SANDESHA2_CLIENT_INTDIR)\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_CLIENT_INTDIR)\*.obj $(SANDESHA2_OBJS) \
	/IMPLIB:$(SANDESHADISTDIR)\lib\$(SANDESHA_CLIENT).lib /DLL \
	/OUT:$(SANDESHADISTDIR)\lib\$(SANDESHA_CLIENT).dll
	-@$(_VC_MANIFEST_EMBED_EXE)

sandesha2_client: $(SANDESHADISTDIR)\lib\$(SANDESHA_CLIENT).dll
	
### sample clients

$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_ping_1_0.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_ping_1_0\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_ping_1_0\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_ping_1_0\*.obj $(SANDESHA_DLL).lib \
	/OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_ping_1_0.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

rm_ping_1_0: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_ping_1_0.exe


$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_ping_1_1.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_ping_1_1\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_ping_1_1\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_ping_1_0\*.obj $(SANDESHA_DLL).lib \
	/OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_ping_1_1.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

rm_ping_1_1: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_ping_1_1.exe 

$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_1_0.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_1_0\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_echo_1_0\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_echo_1_0\*.obj $(SANDESHA_DLL).lib $(SANDESHA_CLIENT).lib \
	/OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_1_0.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

rm_echo_1_0: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_1_0.exe
	
	
$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_1_1.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_1_1\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_echo_1_1\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_echo_1_1\*.obj \
	$(SANDESHA_DLL).lib $(SANDESHA_CLIENT).lib /OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_1_1.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

rm_echo_1_1: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_1_1.exe

$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_mtom_1_0.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_mtom_1_0\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_mtom_1_0\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_mtom_1_0\*.obj $(SANDESHA_DLL).lib $(SANDESHA_CLIENT).lib \
	/OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_mtom_1_0.exe
	-@$(_VC_MANIFEST_EMBED_EXE)
rm_mtom_1_0: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_mtom_1_0.exe

$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_single_1_0.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_single_1_0\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_echo_single_1_0\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_echo_single_1_0\*.obj \
	$(SANDESHA_DLL).lib $(SANDESHA_CLIENT).lib \
	/OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_single_1_0.exe
	-@$(_VC_MANIFEST_EMBED_EXE)
rm_echo_single_1_0: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_single_1_0.exe

$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_single_1_1.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_single_1_1\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_echo_single_1_1\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_echo_single_1_1\*.obj \
	$(SANDESHA_DLL).lib $(SANDESHA_CLIENT).lib /OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_single_1_1.exe
	-@$(_VC_MANIFEST_EMBED_EXE)
rm_echo_single_1_1: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_echo_single_1_1.exe

$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_report.exe :
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_report\*.c /Fo$(SANDESHA2_INTDIR)\samples\rm_report\ /c
	$(LD) $(LDFLAGS) $(LIBS) $(SANDESHA2_INTDIR)\samples\rm_report\*.obj \
	$(SANDESHA_DLL).lib $(SANDESHA_CLIENT).lib /OUT:$(SANDESHADISTDIR)\bin\samples\sandesha2\rm_report.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

rm_report: $(SANDESHADISTDIR)\bin\samples\sandesha2\rm_report.exe
	

all_clients: rm_ping_1_0 rm_ping_1_1 rm_echo_1_1 rm_echo_1_0 rm_mtom_1_0 rm_echo_single_1_0 rm_echo_single_1_1 rm_report


#### services
RMSampleService:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\RMSampleService\*.c /Fo$(SANDESHA2_INTDIR)\samples\RMSampleService\ /c
	$(LD) $(LDFLAGS) $(SANDESHA2_INTDIR)\samples\RMSampleService\*.obj $(LIBS) /DLL \
	/OUT:$(SANDESHADISTDIR)\services\RMSampleService\RMSampleService.dll
	
	@copy $(SANDESHASRC)\samples\RMSampleService\services.xml $(SANDESHADISTDIR)\services\RMSampleService

all_services: RMSampleService

samples: all_clients all_services

dist: clean distdir intdir sandesha2 sandesha2_client copy_samples samples copy_xml copy_include copy_extra

dist_no_samples: clean distdir intdir sandesha2 sandesha2_client copy_xml copy_include copy_extra
