data:
	db 7
	db 4, 5, 6
	dw 7
	dw 4, 5, 6
	dd 7
	dd 4, 5, 6
aaaa:
	mov eax,4
	je aaaa
      jmp xxxx
bbbb:
	mov eax,5
	mov ebx,[ecx]
	je bbbb
xxxx:
	call bbbb
	ret