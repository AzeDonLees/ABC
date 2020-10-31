;Задание:
;Разработать программу, вычисляющую спомощью степенного ряда с точностью не хуже 0,1%
;значение   функции  tan(х)  длязаданного параметра x (использовать FPU)
format PE console
include 'win32a.inc'
entry start

.data:
        strNotify db "Please enter X to calculate tan(x): ", 13,10 ,0;строка для просьбы Х
        strFloat db '%f', 0 ;строка для ввода-вывода в printf и scanf
        strPrintResut db 'Result for tan(x): %f', 13,10,0;строка вывода результата
        strWrongFormat db 'Wrong format! X cannot be more than 38 and less than -38.', 0
        x dd ?              ;переменная для Х
        sin dd ?            ;переменная для вычисленного синуса
        cos dd ?            ;переменная для вычисленного косинуса
        tan dq ?            ;тангенс
        АСС dd 0.001        ;константа для точности
        Cn1 dd -1.0         ;константа для -1f
        C2 dd 2.0           ;константа 2f
        i dd ?              ;переменная для цикла
        NULL = 0            ;константа для ExitProcess

.code:
start:
        ;вызов вывода просьбы ввести Х
        push strNotify
        call [printf]

        ;ввод числа, число сохраняем в x
        push x
        push strFloat
        call [scanf]
        ;присваеваем к Х
        mov eax, [x]
        ;проверка х<38 и x>-38
        cmp eax, 38.0
        jl secondCase
        ;Вывод ошибки
        call printWrongResult

;Кейс для сравнения
secondCase:
 ;проверка x>-38
 cmp eax, -38.0
 ;Вызов калькуляции
 jg normalCalc
 ;Вывод ошибки
 call printWrongResult

;конец программы
programEnd:
  ;ожидание нажатия любой клавиши чтобы приложение не закрылось сразу
  call [getch]
  ;Для закрытия программы без ошибок
  push NULL
  call [ExitProcess]
  ret

;Если все ок вызываем функции вычисления
normalCalc:
  ;вызов функции вычисления тангенса
  call TanFunc
  ;вывод тангента
  push dword[tan+4]    ;вывод через double
  push dword[tan]      ;заполняем ячейку
  push strPrintResut
  call [printf]
  ;вызов конца программы
  call programEnd
  ret

;вывод если ввели не верно
printWrongResult:
   push strWrongFormat
   call [printf]
   ;вызов конца программы
   call programEnd
   ret

;функция вычисления тангенса
TanFunc:
        ;вызов функции вычисления синуса
        call SinFunc
        ;вызов функции вычисления косинуса
        call CosFunc

        ;берем синус
        fld dword[sin]
        ;делим на косинус
        fdiv dword[cos]
        ;выделяем результат в переменную tan
        fstp qword[tan]
        ret

;функция вычисления синуса
SinFunc:
        ;загружаем точность
        fld dword[АСС]
        ;n=x
        fld dword[x]
        ;ячейка для суммы
        fldz
        ;i=1
        mov [i],1
        ;начало цикла
        .loop:
        ;сумма плюс n
        fadd st0,st1
        ;заполняем Х
        fld dword[x]
        ;умножаем X на себя
        fmul st0,st0
        ;умножаем на константу -1
        fmul dword[Cn1]
        ;заполняем i
        fild [i]
        ;умножаем на 2
        fmul dword[C2]
        ;дублируем 2*i
        fld st0
        ;заполняем 1
        fld1
        ;добавляем 1 к 2*i, после очищаем 1
        faddp st1,st0
        ;умножаем 2*i на 2*i+1
        fmulp st1,st0
        ;делим -1*x*x на (2*i)*(2*i+1)
        fdivp st1,st0
        ;умножаем результат на n
        fmul st0,st2
        ;записываем результат в n, очищаем ячейку
        fstp st2
        ;увеличиваем i на 1
        inc [i]
        ;загружаем копию n
        fld st1
        ;берем модуль
        fabs
        ;сравниваем последнее число
        fcomip st3
        ;если |n|>ACC то начинаем цикл заново
        jae .loop
        ;заполняем ответ в sin
        fstp dword[sin]
        ;очищаем занятые ячейки
        fstp st0
        fstp st0
        ret

;функция вычисления косинуса
CosFunc:
        ;загружаем точность
        fld dword[АСС]
        ;n=1
        fld1
        ;ячейка для суммы
        fldz
        ;i=1
        mov [i],1
        ;начало цикла
        .loop:
        ;сумма плюс n
        fadd st0,st1
        ;заполняем Х
        fld dword[x]
        fmul st0,st0
        fmul dword[Cn1]
        ;заполняем I
        fild [i]
        ;умножаем на 2
        fmul dword[C2]
        ;дублируем 2*1
        fld st0
        ;заполняем 1
        fld1
        ;меняем знак единицы
        fchs
        faddp st1,st0
        fmulp st1,st0

        fdivp st1,st0
        fmul st0,st2
        ;записываем результат в n
        fstp st2
        ;увеличиваем i
        inc [i]
        fld st1
        ;берем модуль
        fabs
        ;сравниваем число
        fcomip st3
        jae .loop

        ;выгружаем ответ в переменную cos
        fstp dword[cos]
        ;очищаем занятые ячейки
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

