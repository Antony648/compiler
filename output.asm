_start:
function_main:
function_b_start0:
	push ebp
	mov ebp,esp
;create variable varno:1
	sub esp,4
;create variable varno:2
	sub esp,4
for_start_0:
for_init_start0:
;create and init variable varno:3
	sub esp,4
	mov eax,0
	mov dword[ebp+12],eax
for_init_end0:
	mov eax,dword[ebp+12]
	push eax
	mov eax,10
	pop ebx
	cmp ebx,eax
	setl al
	movzx eax,al
	cmp eax,0
	jne for_b_start0
	jmp for_b_end0
for_b_start0:
	mov eax,dword[ebp+4]
	push eax
	mov eax,dword[ebp+12]
	pop ebx
	add eax,ebx
	mov dword[ebp+4],eax
	jmp for_init_end0
for_b_end0:
	add esp,4
normal_b_start0:
;create and init variable varno:4
	sub esp,4
	mov eax,0
	mov dword[ebp+12],eax
	mov eax,dword[ebp+4]
	push eax
	mov eax,dword[ebp+8]
	pop ebx
	add eax,ebx
	mov dword[ebp+12],eax
normal_b_end0:
	add esp,4
while_start_0:
	mov eax,1
	cmp eax,0
	jne while_b_start0
	jmp while_b_end0
while_b_start0:
	mov eax,dword[ebp+8]
	push eax
	mov eax,1
	pop ebx
	add eax,ebx
	mov dword[ebp+8],eax
	mov eax,dword[ebp+4]
	push eax
	mov eax,3
	pop ebx
	mul eax,ebx
	mov dword[ebp+4],eax
	jmp while_start_0
while_b_end0:
	mov eax,dword[ebp+4]
	ret
function_b_end0:
_end:
