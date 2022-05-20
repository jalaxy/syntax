.global printl
.global printc
.global _start

.data

.text
printl:
    pushl %ebp
    movl  %esp, %ebp
    subl  $16, %esp
    movl  8(%ebp), %eax
    movl  %ebp, %esi
l:  subl  $1, %esi
    movl  $0, %edx
    movl  $10, %ecx
    idiv  %ecx
    addl  $48, %edx
    movb  %dl, 0(%esi)
    cmp   $0, %eax
    jnz   l
    movl  $4, %eax # write syscall
    movl  $1, %ebx
    movl  %esi, %ecx
    movl  %ebp, %edx
    subl  %esi, %edx
    int   $128
    movl  %edx, %eax
    leave
    ret

printc:
    movl  $4, %eax # write syscall
    movl  $1, %ebx
    movl  %esp, %ecx
    addl  $4, %ecx
    movl  $1, %edx
    int   $128
    ret

# main:
#     pushl %ebp
#     movl  %esp, %ebp
#     pushl $2147483647
#     call  print
#     addl  $4, %esp
#     movl  $0, %eax
#     leave
#     ret

_start:
    call main
    movl $1, %eax # exit(0)
    movl $0, %ebx
    int  $0x80
