;; ==========================================================================================
;; Copyright (C) gd Industries 2018. All Rights Reserved.
;; 
;; This software or any its part is distributed under terms of gd Industries End User
;; License Agreement. By downloading or using this software or any its part you agree with 
;; terms of gd Industries End User License Agreement.
;; ==========================================================================================

.Code

;; ------------------------------------------------------------------------------------------
; [rax] char const* 
; goddamn_strrstr(
;	[rcx] char const* cstr,	
;	[rdx] char const* subCStr
;	);
goddamn_strrstr Proc
	cmp  Word Ptr [rdx], 0		; if (*subCStr != '\0')
	jne  @gd_strrstr_start		;	 goto @gd_strrstr_start;
	mov  rax,  rcx				; rax := cstr;
	ret							; return rax;

	@gd_strrstr_start:
	push rcx
	push rdi
	push rsi
	push r10
	push r11
	push r12
	
	mov  r10,  rcx				

	xor  al,   al				; al := 0;
	xor  rcx,  rcx				
	not  rcx					; rcx := ~0ull;
	mov  rdi,  r10				
	cld							; for (*rdi != al && rcx > 0; --rcx;)
	repne scasb					;	rdi += sizeof(byte); 
	dec  rdi					; rdi -= sizeof(byte);		
	mov  r11,  rdi				; r11 := cstr + strlen(cstr);
	 
	xor  al,   al				; al := 0;
	xor  rcx,  rcx				
	not  rcx					; rcx := ~0ull;
	mov  rdi,  rdx				; rdi := subCStr;
	cld							; for (*rdi != al && rcx > 0; --rcx;)
	repne scasb					;	rdi += sizeof(byte); 
	sub  rdi,  rdx				; rdi -= subCStr;
	dec  rdi					; rdi -= sizeof(byte);
	mov  r12,  rdi				; r12 := strlen(subCStr);
	sub  r11,  rdi				; r11 := cstr + strlen(cstr) - strlen(subCStr);
	
	mov  rax,  r11				; rax := r11;

	@gd_strrstr_match:
	mov  rdi,  rax				; rdi := rax
	mov  rsi,  rdx				; rsi := rdx
	mov  rcx,  r12				; rcx := r12
	cld                         ; for (*rdi == *rsi && rcx > 0; --rcx;)
	repe cmpsb                  ;    rdi += sizeof(byte), rsi += sizeof(byte);
	je   @gd_strrstr_done		; if (rcx == 0) goto @gd_strrstr_done;
	cmp  rax,  r10				; if (rax == r10)
	je   @gd_strrstr_not_found	;	goto @gd_strrstr_not_found;
	dec  rax					; rdi -= sizeof(byte);
	jmp  @gd_strrstr_match		; goto @gd_strrstr_match;

	@gd_strrstr_not_found: 
	mov  rax,  0				; rax := nullptr;

	@gd_strrstr_done:
	pop  r12
	pop  r11
	pop  r10
	pop  rsi
	pop  rdi
	pop  rcx
	ret 
goddamn_strrstr EndP
;; ------------------------------------------------------------------------------------------

;; ------------------------------------------------------------------------------------------
; [rax] wchar_t const* 
; goddamn_wcsrstr(
;	[rcx] wchar_t const* cstr,	
;	[rdx] wchar_t const* subCStr
;	);
goddamn_wcsrstr Proc
	cmp  Word Ptr [rdx], 0		; if (*subCStr != L'\0')
	jne  @gd_wcsrstr_start		;	 goto @gd_wcsrstr_start;
	mov  rax,  rcx				; rax := cstr;
	ret							; return_rax;

	@gd_wcsrstr_start:
	push rcx
	push rdi
	push rsi
	push r10
	push r11
	push r12
	
	mov  r10,  rcx				; r10 := rcx

	xor  ax,   ax				; ax  := 0;
	xor  rcx,  rcx				; rcx := 0;
	not  rcx					; rcx := ~rcx;
	mov  rdi,  r10				; rdi := cstr;
	cld							; for (*rdi != ax && rcx > 0; --rcx;)
	repne scasw					;	rdi += sizeof(word); 
	sub  rdi,  2				; rdi -= sizeof(word);		
	mov  r11,  rdi				; r11 := rdi; // r11 := cstr + wcslen(cstr)

	xor  ax,   ax				; ax  := 0;
	xor  rcx,  rcx				; rcx := 0;
	not  rcx					; rcx := ~rcx;
	mov  rdi,  rdx				; rdi := subCStr;
	cld							; for (*rdi != ax && rcx > 0; --rcx;)
	repne scasw					;	rdi += sizeof(word); 
	sub  rdi,  rdx				; rdi -= subCStr;
	sub  rdi,  2				; rdi -= sizeof(word);		
	mov  r12,  rdi				; r12 := rdi; // r12 := strlen(subCStr)
	sub  r11,  rdi				; r11 -= rdi; // r11 := cstr + strlen(cstr) - strlen(subCStr)

	mov  rax,  r11				; rax := r11;

	@gd_wcsrstr_match:
	mov  rdi,  rax				; rdi := rax
	mov  rsi,  rdx				; rsi := rdx
	mov  rcx,  r12				; rcx := r12
	cld                         ; for (*rdi == *rsi && rcx > 0; --rcx;)
	repe cmpsb                  ;    rdi += sizeof(byte), rsi += sizeof(byte);
	je   @gd_wcsrstr_done		; if (rcx == 0) goto @gd_wcsrstr_done;
	cmp  rax,  r10				; if (rax == r10)
	je   @gd_wcsrstr_not_found	;	goto @gd_wcsrstr_not_found;
	dec  rax					; rdi -= sizeof(byte);
	jmp  @gd_wcsrstr_match		; goto @gd_wcsrstr_match;

	@gd_wcsrstr_not_found: 
	mov  rax, 0					; rax := nullptr;

	@gd_wcsrstr_done:
	pop  r12
	pop  r11
	pop  r10
	pop  rsi
	pop  rdi
	pop  rcx
	ret 
goddamn_wcsrstr EndP
;; ------------------------------------------------------------------------------------------

End
