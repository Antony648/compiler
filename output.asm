global _start
_start:
	call function_main
	mov ebx,eax
	mov eax,1
	int 0x80
function_add:
function_b_start0:
	push ebp
	mov ebp,esp
;create and init variable varno:1
	sub esp,4
	mov eax,dword[ebp+8]
	push eax
	mov eax,dword[ebp+12]
	pop ebx
	add eax,ebx
	mov dword[ebp-4],eax
	mov esp,ebp
	pop ebp
	ret
function_b_end0:
function_main:
function_b_start1:
	push ebp
	mov ebp,esp
;create and init variable varno:1
	sub esp,4
	mov eax,2
	mov dword[ebp-4],eax
;create and init variable varno:2
	sub esp,4
	mov eax,3
	mov dword[ebp-8],eax
	mov eax,dword[ebp-8]
	push eax
	mov eax,dword[ebp-4]
	push eax
	call function_add
	add esp,8
for_start_0:
for_init_start0:
;create and init variable varno:3
	sub esp,4
	mov eax,0
	mov dword[ebp-12],eax
for_init_end0:
	mov eax,dword[ebp-12]
	push eax
	mov eax,10
	pop ebx
	cmp ebx,eax
	setl al
	movzx eax,al
	cmp eax,0
	jne for_b_start0
	jmp for_b_dealloc0
for_b_start0:
	mov eax,dword[ebp-4]
	push eax
	mov eax,1
	pop ebx
	add eax,ebx
	mov dword[ebp-4],eax
	mov eax,dword[ebp-12]
	push eax
	mov eax,1
	pop ebx
	add eax,ebx
	mov dword[ebp-12],eax
	jmp for_init_end0
for_b_dealloc0:
	add esp,4
for_b_end0:
;create and init variable varno:4
	sub esp,4
	mov eax,9
	mov dword[ebp-12],eax
	mov eax,0
	mov esp,ebp
	pop ebp
	ret
function_b_end1:
_end:
