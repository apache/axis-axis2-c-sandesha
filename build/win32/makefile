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
		 $(SANDESHASRC)\src\handlers\*.c \
		 $(SANDESHASRC)\src\msgprocessors\*.c \
		 $(SANDESHASRC)\src\transport\*.c \
                 $(SANDESHASRC)\src\util\*.c \
 		 $(SANDESHASRC)\src\workers\*.c \
	         $(SANDESHASRC)\src\wsrm\*.c\
		 $(SANDESHASRC)\src\storage\beans\*.c \
		 $(SANDESHASRC)\src\storage\inmemory\*.c

#compiler options
CC = cl.exe
CFLAGS = /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "AXIS2_DECLARE_EXPORT" /w /nologo
CFLAGS = $(CFLAGS) /I$(AXIS2_INSTALL_DIR)\include /I$(SANDESHASRC)\include

#linker options 
LD = link.exe
LDFLAGS = /nologo /LIBPATH:$(AXIS2_INSTALL_DIR)\lib
LIBS = axis2_util.lib axiom.lib axis2_parser.lib axis2_engine.lib axis2_http_receiver.lib axis2_http_sender.lib

#debug symbols
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) /D "_DEBUG" /Od /Z7
LDFLAGS = $(LDFLAGS) /DEBUG /NODEFAULTLIB:LIBCMTD.lib
!else
CFLAGS = $(CFLAGS) /D "NDEBUG" /O2 
LDFLAGS = $(LDFLAGS)
!endif

#create the director structure

distdir:
	if not exist $(SANDESHADISTDIR) mkdir $(SANDESHADISTDIR)
	if not exist $(SANDESHADISTDIR)\sandesha2 mkdir $(SANDESHADISTDIR)\sandesha2
	if not exist $(SANDESHADISTDIR)\include mkdir $(SANDESHADISTDIR)\include
	if not exist $(SANDESHADISTDIR)\samples mkdir $(SANDESHADISTDIR)\samples
	if not exist $(SANDESHADISTDIR)\samples\client mkdir $(SANDESHADISTDIR)\samples\client
	if not exist $(SANDESHADISTDIR)\samples\server mkdir $(SANDESHADISTDIR)\samples\server
	
clean :
	if exist $(SANDESHADISTDIR) rmdir /S /Q $(SANDESHADISTDIR)


copy_include:
	xcopy /E $(SANDESHASRC)\include $(SANDESHADISTDIR)\include

sandesha2:
	$(CC) $(CFLAGS) $(SANDESHA2_CODE) /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj /DLL /OUT:$(SANDESHA_DLL).dll /IMPLIB:$(SANDESHA_DLL).lib
	copy $(SANDESHA_DLL).dll $(SANDESHADISTDIR)\sandesha2
	copy $(SANDESHA_DLL).lib $(SANDESHADISTDIR)\sandesha2
	del *.obj
	del *.dll
	del *.exp
	del *.lib

copy_xml:
	copy $(SANDESHASRC)\config\axis2.xml  $(SANDESHADISTDIR)\axis2.xml
	copy $(SANDESHASRC)\config\module.xml $(SANDESHADISTDIR)\sandesha2\module.xml

### sample clients

rm_ping_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\client\rm_ping\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj /OUT:rm_ping.exe 
	if not exist $(SANDESHADISRDIR)\samples\client\rm_ping mkdir $(SANDESHADISTDIR)\samples\client\rm_ping
	copy rm_ping.exe $(SANDESHADISTDIR)\samples\client\rm_ping
	copy $(SANDESHASRC)\samples\client\rm_ping\*.c $(SANDESHADISTDIR)\samples\client\rm_ping
	del *.exe
	del *.obj

rm_echo_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\client\rm_echo\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj /OUT:rm_echo.exe 
	if not exist $(SANDESHADISRDIR)\samples\client\rm_echo mkdir $(SANDESHADISTDIR)\samples\client\rm_echo
	copy rm_echo.exe $(SANDESHADISTDIR)\samples\client\rm_echo
	copy $(SANDESHASRC)\samples\client\rm_echo\*.c \
	$(SANDESHADISTDIR)\samples\client\rm_echo 
	copy $(SANDESHASRC)\samples\client\rm_echo\*.h \
	$(SANDESHADISTDIR)\samples\client\rm_echo\

	del *.exe
	del *.obj

rm_echo_blocking_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\client\rm_echo_blocking\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj /OUT:rm_echo_blocking.exe 
	if not exist $(SANDESHADISRDIR)\samples\client\rm_echo_blocking \
		mkdir $(SANDESHADISTDIR)\samples\client\rm_echo_blocking

	copy rm_echo_blocking.exe $(SANDESHADISTDIR)\samples\client\rm_echo_blocking
	copy  $(SANDESHASRC)\samples\client\rm_echo_blocking\*.c \
	$(SANDESHADISTDIR)\samples\client\rm_echo_blocking
	copy  $(SANDESHASRC)\samples\client\rm_echo_blocking\*.h \
	$(SANDESHADISTDIR)\samples\client\rm_echo_blocking

	del *.exe
	del *.obj

rm_mtom_client:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\client\rm_mtom\*.c /c
	$(LD) $(LDFLAGS) $(LIBS) *.obj /OUT:rm_mtom.exe 
	if not exist $(SANDESHADISRDIR)\samples\client\rm_mtom \
		mkdir $(SANDESHADISTDIR)\samples\client\rm_mtom

	copy rm_mtom.exe $(SANDESHADISTDIR)\samples\client\rm_mtom
	copy  $(SANDESHASRC)\samples\client\rm_mtom\*.c $(SANDESHADISTDIR)\samples\client\rm_mtom
	del *.exe
	del *.obj

all_clients: rm_ping_client rm_echo_client rm_echo_blocking_client rm_mtom_client

#### services
rm_services_mtom:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\server\rm_mtom\*.c /c
	$(LD) $(LDFLAGS) *.obj $(LIBS) /DLL /OUT:rm_mtom.dll  
	del *.obj
	
	if not exist $(SANDESHADISTDIR)\samples\server\rm_mtom mkdir $(SANDESHADISTDIR)\samples\server\rm_mtom

	copy rm_mtom.dll $(SANDESHADISTDIR)\samples\server\rm_mtom

	del *.dll
	del *.exp
	del *.lib
	
	copy $(SANDESHASRC)\samples\server\rm_mtom\services.xml $(SANDESHADISTDIR)\samples\server\rm_mtom

rm_services_ping:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\server\rm_ping\*.c /c
	$(LD) $(LDFLAGS) *.obj $(LIBS) /DLL /OUT:rm_ping.dll  
	del *.obj
	
	if not exist $(SANDESHADISTDIR)\samples\server\rm_ping mkdir $(SANDESHADISTDIR)\samples\server\rm_ping

	copy rm_ping.dll $(SANDESHADISTDIR)\samples\server\rm_ping

	del *.dll
	del *.exp
	del *.lib
	
	copy $(SANDESHASRC)\samples\server\rm_ping\services.xml $(SANDESHADISTDIR)\samples\server\rm_ping

rm_services_echo:
	$(CC) $(CFLAGS) $(SANDESHASRC)\samples\server\rm_echo\*.c /c
	$(LD) $(LDFLAGS) *.obj $(LIBS) /DLL /OUT:rm_echo.dll  
	del *.obj
	
	if not exist $(SANDESHADISTDIR)\samples\server\rm_echo mkdir $(SANDESHADISTDIR)\samples\server\rm_echo

	copy rm_echo.dll $(SANDESHADISTDIR)\samples\server\rm_echo

	del *.dll
	del *.exp
	del *.lib
	
	copy $(SANDESHASRC)\samples\server\rm_echo\services.xml $(SANDESHADISTDIR)\samples\server\rm_echo

all_services: rm_services_mtom rm_services_ping rm_services_echo

samples: all_clients all_services

dist: clean distdir sandesha2  samples copy_xml copy_include

	 


	






	



