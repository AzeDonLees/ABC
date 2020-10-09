format PE console
include 'win32a.inc'
entry start

.data:
        get_int db '%d',0
        put_int db ' %d',0
        ;Вывод текста для запроса размера массива
        Enter_size db 'Reserve array size:',13,10,0
        ;Вывод текста для заполнения массива
        Enter_elements db 'Put array itself:',13,10,0
        Result db 'Modified: ',0
        temp dd 0
        array1 dd 0
        array2 dd 0
        array1size dd 0
.code:

start:
        ;Вызов определение размера массива
        cinvoke printf,Enter_size
        ;Заполнение масиива
        cinvoke scanf,get_int,array1size

        mov eax,[array1size]
        shl eax,2
        cinvoke malloc,eax
        mov [array1],eax
        mov eax,[array1size]
        shl eax,2
        push eax
        cinvoke malloc,eax
        mov [array2],eax

        ;Реализация ввода в массив
        call ArrayInput
        ;Реализация функции расчета для массива
        call Function
        ;Реализация вывода массива
        call PrintResult

        ;освобождение памяти
        cinvoke free,[array1]
        cinvoke free,[array2]
        cinvoke scanf,get_int,temp
        ret
;Функция заполнения массива
ArrayInput:
        cinvoke printf,Enter_elements
        mov eax,[array1]
        mov ecx,[array1size]
        .input_loop:
        pusha
        cinvoke scanf,get_int,eax
        popa
        add eax,4
        loop .input_loop
        ret
;Фукнция поворота массива
Function:
        mov eax,[array1]
        mov ecx,[array1size]
        mov edx,ecx
        dec edx
        shl edx,2
        add edx,[array2]
        .fl:
        mov ebx,[eax]
        mov [edx],ebx
        add eax,4
        sub edx,4
        loop .fl
        ret
;Функция вывода в консоль
PrintResult:
        cinvoke printf,Result
        mov eax,[array2]
        mov ecx,[array1size]
        .print_loop:
        pusha
        cinvoke printf,put_int,[eax]
        popa
        add eax,4
        loop .print_loop
        ret



section 'import' import readable

library msvcrt,'msvcrt.dll'
import msvcrt,\
       printf,'printf',\
       scanf,'scanf',\
       free,'free',\
       malloc,'malloc'
