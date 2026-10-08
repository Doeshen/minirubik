.option norvc
.option norelax

.section .text
.globl _start

_start:
    # LED Matrix base address
    lui t0, 0xF0000

    # Red = 0x00FF0000
    lui t1, 0x00FF0

    # Write red to LED 0
    sw t1, 0(t0)

    # Green = 0x0000FF00
    lui t1, 0x00010
    addi t1, t1, -256

    # Write green to LED 1
    sw t1, 4(t0)

    # Exit
    addi a0, zero, 0
    addi a7, zero, 93
    ecall
