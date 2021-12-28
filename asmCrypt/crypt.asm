%define MAX 0x100000
global _start
SECTION .bss
buffer resb MAX
SECTION .text
_start:
mov edx,0x1
mov ecx,buffer
dec ecx
xor ebx,ebx
.nactiznak:
inc ecx
mov eax,0x3
int 0x80
cmp byte [ecx],0xa
jnz .nactiznak
mov byte [ecx],0x0
mov eax,0x5
mov ebx,buffer
xor ecx,ecx
int 0x80
mov ecx,buffer
mov ebx,eax
mov esi,0x1
.nacti:
mov edx,MAX
mov eax,0x3
int 0x80
mov edi,eax
mov edx,eax
mov al,10101010b
xor ah,ah
.sifruj:
xor byte [ecx],al
inc ecx
dec edi
jnz .sifruj
mov eax,0x4
xchg ebx,esi
mov ecx,buffer
int 0x80
xchg ebx,esi
cmp edx,MAX
jz .nacti
mov eax,0x1
mov ebx,0x0
int 0x80

