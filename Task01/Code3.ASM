format pe
include 'win32a.inc'
section "code" readable writeable executable
start:
        cinvoke printf,msg
        cinvoke gets,buff
        cinvoke printf,buff
        invoke ExitProcess,0
section '.data' data readable writeable
msg:
        db 'Input string:',0
buff:
        times 0xff db 0
section '.idata' import data readable writeable
  library kernel32,'kernel32.dll',\
          crtdll,'crtdll.dll'
  import kernel32,\
         ExitProcess,'ExitProcess'
  import crtdll,\
         printf,'printf',\
         gets,'gets'