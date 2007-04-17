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
LIBS = axutil.lib axiom.lib axis2_parser.lib axis2_engine.lib axis2_http_receiver.lib axis2_http_sender.lib

#debug symbols
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) /D "_DEBUG" /Od /Z7
LDFLAGS = $(LDFLAGS) /DEBUG /NODEFAULTLIB:LIBCMTD.lib /LIBPATH:$(MYSQL_BIN_DIR)\lib\debug libmysql.lib
!else
CFLAGS = $(CFLAGS) /D "NDEBUG" /O2 
LDFLAGS = $(LDFLAGS) /LIBPATH:$(MYSQL_BIN_DIR)\lib\opt libmysql.lib
!endif

CFLAGS = $(CFLAGS) /I$(AXIS2_INSTALL_DIR)\include /I$(SQLITE_SRC_DIR) /I$(SANDESHASRC)\include /I$(MYSQL_BIN_DIR)\include

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

	del *.obj
	
	copy $(SANDESHA_DLL).* $(SANDESHADISTDIR)\sandesha2

	del *.dll
	del *.exp
	del *.lib

copy_xml:
	copy $(SANDESHASRC)\config\axis2.xml  $(SANDESHADISTDIR)\axis2.xml
	copy $(SANDESHASRC)\config\module.xml $(SANDESHADISTDIR)\sandesha2\module.xml

### sample clients

rm_ping_client_1_0:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_ping_1_0\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_ping_1_0.exe 
	if not exist $(SANDESHADISRDIR)\samples\rm_ping mkdir $(SANDESHADISTDIR)\samples\rm_ping_1_0
	copy rm_ping_1_0.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_ping_1_0\*.c $(SANDESHADISTDIR)\samples\rm_ping_1_0
	del *.exe
	del *.obj

rm_ping_client_1_1:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_ping_1_1\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_ping_1_1.exe 
	if not exist $(SANDESHADISRDIR)\samples\rm_ping mkdir $(SANDESHADISTDIR)\samples\rm_ping_1_1
	copy rm_ping_1_1.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_ping_1_1\*.c $(SANDESHADISTDIR)\samples\rm_ping_1_1
	del *.exe
	del *.obj

rm_echo_1_0_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_1_0\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_echo.exe 
	if not exist $(SANDESHADISRDIR)\samples\rm_echo_1_0 mkdir $(SANDESHADISTDIR)\samples\rm_echo_1_0
	copy rm_echo.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_echo_1_0\*.c \
	$(SANDESHADISTDIR)\samples\rm_echo_1_0 
	copy $(SANDESHASRC)\samples\rm_echo_1_0\*.h \
	$(SANDESHADISTDIR)\samples\rm_echo_1_0\

	del *.exe
	del *.obj

rm_mtom_1_0_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_mtom_1_0\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_mtom.exe 
	if not exist $(SANDESHADISTDIR)\samples\rm_mtom_1_0 mkdir $(SANDESHADISTDIR)\samples\rm_mtom_1_0
	copy rm_mtom.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_mtom_1_0\*.c $(SANDESHADISTDIR)\samples\rm_mtom_1_0
	copy $(SANDESHASRC)\samples\rm_mtom_1_0\*.h $(SANDESHADISTDIR)\samples\rm_mtom_1_0

	del *.exe
	del *.obj

rm_echo_client_1_1:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_1_1\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_echo_1_1.exe 
	if not exist $(SANDESHADISTDIR)\samples\rm_echo_1_1 mkdir $(SANDESHADISTDIR)\samples\rm_echo_1_1
	copy rm_echo_1_1.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_echo_1_1\*.c $(SANDESHADISTDIR)\samples\rm_echo_1_1 
	copy $(SANDESHASRC)\samples\rm_echo_1_1\*.h $(SANDESHADISTDIR)\samples\rm_echo_1_1


	del *.exe
	del *.obj

rm_echo_singel_client_1_0:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_single_1_0\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_echo_single_1_0.exe 
	if not exist $(SANDESHADISTDIR)\samples\rm_echo_single_1_0 mkdir $(SANDESHADISTDIR)\samples\rm_echo_single_1_0
	copy rm_echo_single_1_0.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_echo_single_1_0\*.c $(SANDESHADISTDIR)\samples\rm_echo_single_1_0
	copy $(SANDESHASRC)\samples\rm_echo_single_1_0\*.h $(SANDESHADISTDIR)\samples\rm_echo_single_1_0


	del *.exe
	del *.obj

rm_echo_single_client_1_1:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\rm_echo_single_1_1\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj $(SANDESHA_DLL).lib /OUT:rm_echo_single_1_1.exe 
	if not exist $(SANDESHADISTDIR)\samples\rm_echo_single_1_1 mkdir $(SANDESHADISTDIR)\samples\rm_echo_single_1_1
	copy rm_echo_single_1_1.exe $(SANDESHADISTDIR)\samples\bin
	copy $(SANDESHASRC)\samples\rm_echo_single_1_1\*.c $(SANDESHADISTDIR)\samples\rm_echo_single_1_1
	copy $(SANDESHASRC)\samples\rm_echo_single_1_1\*.h $(SANDESHADISTDIR)\samples\rm_echo_single_1_1


	del *.exe
	del *.obj


all_clients: rm_ping_client_1_0 rm_ping_client_1_1 rm_echo_client_1_1 rm_echo_1_0_client rm_mtom_1_0_client rm_echo_singel_client_1_0 rm_echo_single_client_1_1


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


	 


	






	



