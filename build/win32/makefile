####################################################
# Makefile for Sandesha2			   #
# you can do 					   #
# nmake dist     - distribution  (dist / samples ) #
# nmake clean  	 - clean			   #
# nmake samples  - samples 			   #
####################################################

!include configure.in
#define folders 

SANDESHASRC = ..\..
SANDESHADISTDIR = ..\sandesha2
SANDESHA_DLL = sandesha2


SANDESHA2_CODE = $(SANDESHASRC)\src\core\*.c \
		 $(SANDESHASRC)\src\client\*.c \
		 $(SANDESHASRC)\src\storage\beans\*.c \
		 $(SANDESHASRC)\src\storage\inmemory\*.c \
		 $(SANDESHASRC)\src\storage\permanent\*.c \
		 $(SANDESHASRC)\src\storage\*.c \
		 $(SANDESHASRC)\src\transport\*.c \
		 $(SANDESHASRC)\src\polling\*.c \
		 $(SANDESHASRC)\src\msgprocessors\*.c \
		 $(SANDESHASRC)\src\handlers\*.c \
		 $(SANDESHASRC)\src\transport\*.c \
                 $(SANDESHASRC)\src\util\*.c \
 		 $(SANDESHASRC)\src\workers\*.c \
	         $(SANDESHASRC)\src\wsrm\*.c 

#compiler options
CC = cl.exe
CFLAGS = /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "AXIS2_DECLARE_EXPORT" /w /nologo


#linker options 
LD = link.exe
LDFLAGS = /nologo /LIBPATH:$(AXIS2_INSTALL_DIR)\lib /LIBPATH:$(SQLITE_SRC_DIR) /LIBPATH:$(SANDESHADISTDIR)\sandesha2
LIBS = axis2_util.lib axiom.lib axis2_parser.lib axis2_engine.lib sqlite.lib axis2_http_receiver.lib axis2_http_sender.lib

#debug symbols
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) /D "_DEBUG" /Od /Z7
LDFLAGS = $(LDFLAGS) /DEBUG /NODEFAULTLIB:LIBCMTD.lib
!else
CFLAGS = $(CFLAGS) /D "NDEBUG" /O2 
LDFLAGS = $(LDFLAGS)
!endif

CFLAGS = $(CFLAGS) /I$(AXIS2_INSTALL_DIR)\include /I$(SQLITE_SRC_DIR) /I$(SANDESHASRC)\include

#create the director structure

distdir:
	if not exist $(SANDESHADISTDIR) mkdir $(SANDESHADISTDIR)
	if not exist $(SANDESHADISTDIR)\sandesha2 mkdir $(SANDESHADISTDIR)\sandesha2
	if not exist $(SANDESHADISTDIR)\include mkdir $(SANDESHADISTDIR)\include
	if not exist $(SANDESHADISTDIR)\samples mkdir $(SANDESHADISTDIR)\samples
	if not exist $(SANDESHADISTDIR)\samples\bin mkdir $(SANDESHADISTDIR)\samples\bin
	
clean :
	if exist $(SANDESHADISTDIR) rmdir /S /Q $(SANDESHADISTDIR)

clean_mkdir:
	del *.obj del *.exe del *.exp

copy_extra:
	copy $(SANDESHASRC)\config\sandesha2_schema.bat $(SANDESHADISTDIR)

copy_include:
	xcopy /E $(SANDESHASRC)\include $(SANDESHADISTDIR)\include

sandesha2:
	$(CC) $(CFLAGS) $(SANDESHA2_CODE) /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj /DLL /OUT:$(SANDESHA_DLL).dll /IMPLIB:$(SANDESHA_DLL).lib
	copy $(SANDESHA_DLL).dll $(SANDESHADISTDIR)\sandesha2
	copy $(SANDESHA_DLL).lib $(SANDESHADISTDIR)\sandesha2
	copy *.pdb $(SANDESHADISTDIR)\sandesha2

	del *.obj
	del *.dll
	del *.exp
	del *.lib

copy_xml:
	copy $(SANDESHASRC)\config\axis2.xml  $(SANDESHADISTDIR)\axis2.xml
	copy $(SANDESHASRC)\config\module.xml $(SANDESHADISTDIR)\sandesha2\module.xml

### sample clients

rm_ping_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_ping\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_ping.exe 
	if not exist $(SANDESHADISRDIR)\samples\rm_ping mkdir $(SANDESHADISTDIR)\samples\rm_ping
	copy rm_ping.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_ping\*.c $(SANDESHADISTDIR)\samples\rm_ping
	del *.exe
	del *.obj

rm_echo_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_echo.exe 
	if not exist $(SANDESHADISRDIR)\samples\rm_echo mkdir $(SANDESHADISTDIR)\samples\rm_echo
	copy rm_echo.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_echo\*.c \
	$(SANDESHADISTDIR)\samples\rm_echo 
	copy $(SANDESHASRC)\samples\rm_echo\*.h \
	$(SANDESHADISTDIR)\samples\rm_echo\

	del *.exe
	del *.obj

rm_mtom_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_mtom\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_mtom.exe 
	if not exist $(SANDESHADISTDIR)\samples\rm_mtom mkdir $(SANDESHADISTDIR)\samples\rm_mtom
	copy rm_mtom.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_mtom\*.c $(SANDESHADISTDIR)\samples\rm_mtom
	copy $(SANDESHASRC)\samples\rm_mtom\*.h $(SANDESHADISTDIR)\samples\rm_mtom

	del *.exe
	del *.obj

rm_1_1_echo_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_1_1_echo\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_1_1_echo.exe 
	if not exist $(SANDESHADISTDIR)\samples\rm_1_1_echo mkdir $(SANDESHADISTDIR)\samples\rm_1_1_echo
	copy rm_1_1_echo.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_1_1_echo\*.c $(SANDESHADISTDIR)\samples\rm_1_1_echo 
	copy $(SANDESHASRC)\samples\rm_1_1_echo\*.h $(SANDESHADISTDIR)\samples\rm_1_1_echo


	del *.exe
	del *.obj

all_clients: rm_ping_client rm_1_1_echo_client rm_echo_client rm_mtom_client


#### services
RMSampleService:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\RMSampleService\*.c /c
	$(LD) $(LDFLAGS) *.obj $(LIBS) /DLL /OUT:RMSampleService.dll  
	del *.obj
	
	if not exist $(SANDESHADISTDIR)\samples\RMSampleService mkdir $(SANDESHADISTDIR)\samples\RMSampleService

	copy RMSampleService* $(SANDESHADISTDIR)\samples\RMSampleService\

	del *.dll
	del *.exp
	del *.lib
	
	copy $(SANDESHASRC)\samples\RMSampleService\services.xml $(SANDESHADISTDIR)\samples\RMSampleService

all_services: RMSampleService

samples: all_clients all_services

dist: clean clean_mkdir distdir sandesha2  samples copy_xml copy_include copy_extra


	 


	






	



