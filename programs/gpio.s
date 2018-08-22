mov r2, #0x40000
ld r0, =0x20200004
str r2, [r0]
mov r1, #0x10000
str r1, [r0, #40]
loop:
str r1, [r0, #40]
mov r2, #0xFFFFFF
wait1:
sub r2, r2, #1
cmp r2, #0
bne wait1
str r1, [r0, #28]
mov r2, #0xFFFFFF
wait2:
sub r2, r2, #1
cmp r2, #0
bne wait2
b loop