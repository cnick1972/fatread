[bits 16]
[org 0x0600]

entry:
    jmp start

ReadSectors:
    mov ah, 0x42
    mov dl, [bootdrive]
    mov si, dap
    int 0x13
    ret

start:
  cli                         ; We do not want to be interrupted
  xor ax, ax                  ; 0 AX
  mov ds, ax                  ; Set Data Segment to 0
  mov es, ax                  ; Set Extra Segment to 0
  mov ss, ax                  ; Set Stack Segment to 0
  mov sp, ax                  ; Set Stack Pointer to 0
  .CopyLower:
    mov cx, 0x0100            ; 256 WORDs in MBR
    mov si, 0x7C00            ; Current MBR Address
    mov di, 0x0600            ; New MBR Address
    rep movsw                 ; Copy MBR
  jmp 0:LowStart              ; Jump to new Address

LowStart:
    mov byte [bootdrive], dl
    .CheckPartitions:
        mov bx, PT1
        mov cx, 4
        .CKPTloop:
            mov al, byte [bx]
            test al, 0x80
            jnz .CKPTFound
            add bx, 0x10
            dec cx
            jnz .CKPTloop
        jmp ERROR
        .CKPTFound:
            mov WORD [PToff], bx
            add bx, 8
    
    .ReadVBR:
        mov ebx, dword [bx]
        mov [lba], ebx
        mov ax, 0x7c00
        mov [buf_off], ax
        mov cx, 1
        call ReadSectors

    .JumpToVBR:
        cmp WORD [0x7dfe], 0xaa55
        jne ERROR
        mov si, word [PToff]
        mov dl, byte [bootdrive]
        jmp 0x7c00

ERROR:
    cli
    hlt
PToff           db 0    
testinfo        db 0xff
bootdrive       db 0

dap:
packetSize: db 0x10
reserved:   db 0x0
sectorsNumber:  dw 0x1
buf_off:    dw 0x0000             ; Place offset before segment
buf_seg:    dw 0x0000
lba:        dd 0x0
            dd 0x0

times (0x1be - ($-$$))  nop

PT1: