;�������:
;����������� ���������, ����������� �������� ���������� ���� � ��������� �� ���� 0,1%
;��������   �������  tan(�)  ������������ ��������� x (������������ FPU)
format PE console
include 'win32a.inc'
entry start

.data:
        strNotify db "Please enter X to calculate tan(x): ", 13,10 ,0;������ ��� ������� �
        strFloat db '%f', 0 ;������ ��� �����-������ � printf � scanf
        strPrintResut db 'Result for tan(x): %f', 13,10,0;������ ������ ����������
        strWrongFormat db 'Wrong format! X cannot be more than 38 and less than -38.', 0
        x dd ?              ;���������� ��� �
        sin dd ?            ;���������� ��� ������������ ������
        cos dd ?            ;���������� ��� ������������ ��������
        tan dq ?            ;�������
        ��� dd 0.001        ;��������� ��� ��������
        Cn1 dd -1.0         ;��������� ��� -1f
        C2 dd 2.0           ;��������� 2f
        i dd ?              ;���������� ��� �����
        NULL = 0            ;��������� ��� ExitProcess

.code:
start:
        ;����� ������ ������� ������ �
        push strNotify
        call [printf]

        ;���� �����, ����� ��������� � x
        push x
        push strFloat
        call [scanf]
        ;����������� � �
        mov eax, [x]
        ;�������� �<38 � x>-38
        cmp eax, 38.0
        jl secondCase
        ;����� ������
        call printWrongResult

;���� ��� ���������
secondCase:
 ;�������� x>-38
 cmp eax, -38.0
 ;����� �����������
 jg normalCalc
 ;����� ������
 call printWrongResult

;����� ���������
programEnd:
  ;�������� ������� ����� ������� ����� ���������� �� ��������� �����
  call [getch]
  ;��� �������� ��������� ��� ������
  push NULL
  call [ExitProcess]
  ret

;���� ��� �� �������� ������� ����������
normalCalc:
  ;����� ������� ���������� ��������
  call TanFunc
  ;����� ��������
  push dword[tan+4]    ;����� ����� double
  push dword[tan]      ;��������� ������
  push strPrintResut
  call [printf]
  ;����� ����� ���������
  call programEnd
  ret

;����� ���� ����� �� �����
printWrongResult:
   push strWrongFormat
   call [printf]
   ;����� ����� ���������
   call programEnd
   ret

;������� ���������� ��������
TanFunc:
        ;����� ������� ���������� ������
        call SinFunc
        ;����� ������� ���������� ��������
        call CosFunc

        ;����� �����
        fld dword[sin]
        ;����� �� �������
        fdiv dword[cos]
        ;�������� ��������� � ���������� tan
        fstp qword[tan]
        ret

;������� ���������� ������
SinFunc:
        ;��������� ��������
        fld dword[���]
        ;n=x
        fld dword[x]
        ;������ ��� �����
        fldz
        ;i=1
        mov [i],1
        ;������ �����
        .loop:
        ;����� ���� n
        fadd st0,st1
        ;��������� �
        fld dword[x]
        ;�������� X �� ����
        fmul st0,st0
        ;�������� �� ��������� -1
        fmul dword[Cn1]
        ;��������� i
        fild [i]
        ;�������� �� 2
        fmul dword[C2]
        ;��������� 2*i
        fld st0
        ;��������� 1
        fld1
        ;��������� 1 � 2*i, ����� ������� 1
        faddp st1,st0
        ;�������� 2*i �� 2*i+1
        fmulp st1,st0
        ;����� -1*x*x �� (2*i)*(2*i+1)
        fdivp st1,st0
        ;�������� ��������� �� n
        fmul st0,st2
        ;���������� ��������� � n, ������� ������
        fstp st2
        ;����������� i �� 1
        inc [i]
        ;��������� ����� n
        fld st1
        ;����� ������
        fabs
        ;���������� ��������� �����
        fcomip st3
        ;���� |n|>ACC �� �������� ���� ������
        jae .loop
        ;��������� ����� � sin
        fstp dword[sin]
        ;������� ������� ������
        fstp st0
        fstp st0
        ret

;������� ���������� ��������
CosFunc:
        ;��������� ��������
        fld dword[���]
        ;n=1
        fld1
        ;������ ��� �����
        fldz
        ;i=1
        mov [i],1
        ;������ �����
        .loop:
        ;����� ���� n
        fadd st0,st1
        ;��������� �
        fld dword[x]
        fmul st0,st0
        fmul dword[Cn1]
        ;��������� I
        fild [i]
        ;�������� �� 2
        fmul dword[C2]
        ;��������� 2*1
        fld st0
        ;��������� 1
        fld1
        ;������ ���� �������
        fchs
        faddp st1,st0
        fmulp st1,st0

        fdivp st1,st0
        fmul st0,st2
        ;���������� ��������� � n
        fstp st2
        ;����������� i
        inc [i]
        fld st1
        ;����� ������
        fabs
        ;���������� �����
        fcomip st3
        jae .loop

        ;��������� ����� � ���������� cos
        fstp dword[cos]
        ;������� ������� ������
        fstp st0
        fstp st0
        ret

section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll'

include 'api\kernel32.inc'
import kernel,\
ExitProcess, 'ExitProcess'

import msvcrt,\
printf, 'printf', \
getch, '_getch', \
scanf, 'scanf'

