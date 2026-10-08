    .option norvc
    .option norelax

    .equ MAX_DEPTH, 14
    .equ CUBIES, 7
    .equ MOVES, 9

    # --------------------------------------------------------
    # IDA* stack frame = 48 bytes
    #
    # p[7]      @ 0
    # o[7]      @ 8
    # next_move @ 16
    # last_face @ 20
    # sum_lb    @ 24
    # bad_ori   @ 28
    # count_lb2 @ 32
    # corner_lb @ 36..42
    # --------------------------------------------------------
    .equ FP_P,        0
    .equ FP_O,        8
    .equ FP_M,        16
    .equ FP_FACE,     20
    .equ FP_SUM,      24
    .equ FP_BAD,      28
    .equ FP_D2,       32
    .equ FP_CDIST,    36
    .equ FP_PERM, 44
    .equ FRAME_SIZE,  48


    # ========================================================
    # READ-ONLY DATA
    # ========================================================
    .section .rodata
    .align 2

    .align 2

    .align 2
perm_factorials:
    .word 720,120,24,6,2,1,1

    .align 1
perm_transitions:
    .incbin "permutation_transition.bin"
perm_transitions_end:


    .align 1
ori_transitions:
    .incbin "orientation_transition.bin"
ori_transitions_end:
    .align 2
corner3_positions:
    .incbin "corner3_positions.bin"
corner3_positions_end:
    .align 1
corner3_pdb:
    .incbin "corner3_pdb.bin"
corner3_pdb_end:
    .align 2
orientation_compact_pdb:
    .incbin "orientation_compact.bin"
orientation_compact_pdb_end:

    .align 2
corner_pdb:
    .incbin "corner_pdb.bin"
corner_pdb_end:
    .align 2

permutation_pdb:
    .incbin "permutation_pdb.bin"
permutation_pdb_end:

orientation_pdb:
    .incbin "orientation_pdb.bin"
orientation_pdb_end:
    .align 2

    # --------------------------------------------------------
    # Move transitions.
    # These are transition constants only.
    # There is NO PDB / heuristic distance table here.
    # --------------------------------------------------------

# Four moved positions per face (R, D, B). No need to loop over stationary corners.
face_positions:
    .byte 0,1,3,4
    .byte 3,4,5,6
    .byte 1,2,4,5
move_source:
    # R
    .byte 1,4,2,0,3,5,6,0
    # R2
    .byte 4,3,2,1,0,5,6,0
    # R'
    .byte 3,0,2,4,1,5,6,0

    # D
    .byte 0,1,2,4,5,6,3,0
    # D2
    .byte 0,1,2,5,6,3,4,0
    # D'
    .byte 0,1,2,6,3,4,5,0

    # B
    .byte 0,2,5,3,1,4,6,0
    # B2
    .byte 0,5,4,3,2,1,6,0
    # B'
    .byte 0,4,1,3,5,2,6,0

    .align 2

move_twist:
    # R
    .byte 1,2,0,2,1,0,0,0
    # R2
    .byte 0,0,0,0,0,0,0,0
    # R'
    .byte 1,2,0,2,1,0,0,0

    # D
    .byte 0,0,0,1,2,1,2,0
    # D2
    .byte 0,0,0,0,0,0,0,0
    # D'
    .byte 0,0,0,1,2,1,2,0

    # B
    .byte 0,0,0,0,0,0,0,0
    # B2
    .byte 0,0,0,0,0,0,0,0
    # B'
    .byte 0,0,0,0,0,0,0,0


    # --------------------------------------------------------
    # Output strings
    # --------------------------------------------------------

str_ida_bound: .asciz "IDA bound: "
str_header:
    .asciz "=== Pure RV32I Solver (Early PDB Pruning) ===\n"

str_lbracket:
    .asciz "["

str_input:
    .asciz "] Input: "

str_nl:
    .asciz "\n"

str_solved:
    .asciz "  -> Solved in "

str_moves:
    .asciz " moves: "

str_space:
    .asciz " "

str_pass:
    .asciz " [PASS]\n"

str_fail:
    .asciz " [FAIL]\n"

str_nosol:
    .asciz "  -> No solution! [FAIL]\n"

str_all_pass:
    .asciz "\nAll 3 tests PASSED!\n"

str_some_fail:
    .asciz "\nTests FAILED!\n"


    # --------------------------------------------------------
    # Move names
    # Output-only lookup, not a heuristic table.
    # --------------------------------------------------------

move_names:
    .word move_R, move_R2, move_Ri
    .word move_D, move_D2, move_Di
    .word move_B, move_B2, move_Bi

move_R:
    .asciz "R"

move_R2:
    .asciz "R2"

move_Ri:
    .asciz "R'"

move_D:
    .asciz "D"

move_D2:
    .asciz "D2"

move_Di:
    .asciz "D'"

move_B:
    .asciz "B"

move_B2:
    .asciz "B2"

move_Bi:
    .asciz "B'"


    # --------------------------------------------------------
    # Number strings 0..14
    # --------------------------------------------------------

num_names:
    .word num_0,  num_1,  num_2,  num_3
    .word num_4,  num_5,  num_6,  num_7
    .word num_8,  num_9,  num_10, num_11
    .word num_12, num_13, num_14

num_0:
    .asciz "0"
num_1:
    .asciz "1"
num_2:
    .asciz "2"
num_3:
    .asciz "3"
num_4:
    .asciz "4"
num_5:
    .asciz "5"
num_6:
    .asciz "6"
num_7:
    .asciz "7"
num_8:
    .asciz "8"
num_9:
    .asciz "9"
num_10:
    .asciz "10"
num_11:
    .asciz "11"
num_12:
    .asciz "12"
num_13:
    .asciz "13"
num_14:
    .asciz "14"


    # --------------------------------------------------------
    # Tests
    # --------------------------------------------------------

test1_name:
    .asciz "Test 1"

test1_state:
    .asciz "12345671111111"

test2_name:
    .asciz "Test 2"

test2_state:
    .asciz "25314672313211"

test3_name:
    .asciz "Test 3"

test3_state:
    .asciz "21345671111111"


    # ========================================================
    # BSS
    # ========================================================
    .section .bss
    .align 4

stack_mem:
    .space 15*FRAME_SIZE

solution_path:
    .space MAX_DEPTH

solution_len:
    .word 0


    # ========================================================
    # TEXT
    # ========================================================
    .section .text
    .align 2
    .globl _start
    .type _start,@function


# ============================================================
# _start
# ============================================================
_start:

    la   a0, str_header
    jal  ra, print_str

    # Test 1
    la   a0, test1_name
    la   a1, test1_state
    addi a2, x0, 0
    jal  ra, run_test
    addi s0, a0, 0

    # Test 2
    la   a0, test2_name
    la   a1, test2_state
    addi a2, x0, 1
    jal  ra, run_test
    addi s1, a0, 0

    # Test 3
    la   a0, test3_name
    la   a1, test3_state
    addi a2, x0, 11
    jal  ra, run_test
    addi s2, a0, 0

    and  t0, s0, s1
    and  t0, t0, s2
    beqz t0, all_fail

    la   a0, str_all_pass
    jal  ra, print_str

    addi a0, x0, 0
    jal  x0, exit_prog


all_fail:
    la   a0, str_some_fail
    jal  ra, print_str

    addi a0, x0, 1
    jal  x0, exit_prog


# ============================================================
# print_str
# a0 = char *
# Ripes syscall: a7=4
# ============================================================
print_str:
    addi a7, x0, 4
    ecall
    jalr x0, 0(ra)


# ============================================================
# exit_prog
# a0 = exit code
# Ripes syscall: a7=93
# ============================================================
exit_prog:
    addi a7, x0, 93
    ecall
    jalr x0, 0(ra)


# ============================================================
# print_move
# a0 = move index 0..8
# ============================================================
print_move:
    addi sp, sp, -16
    sw   ra, 12(sp)

    slli t0, a0, 2
    la   t1, move_names
    add  t1, t1, t0
    lw   a0, 0(t1)

    jal  ra, print_str

    lw   ra, 12(sp)
    addi sp, sp, 16
    jalr x0, 0(ra)


# ============================================================
# print_num
# a0 = 0..14
# ============================================================
print_num:
    addi sp, sp, -16
    sw   ra, 12(sp)

    slli t0, a0, 2
    la   t1, num_names
    add  t1, t1, t0
    lw   a0, 0(t1)

    jal  ra, print_str

    lw   ra, 12(sp)
    addi sp, sp, 16
    jalr x0, 0(ra)


# ============================================================
# corner_lb
#
# a0 = cubie id
# a1 = current position
# a2 = orientation
#
# Returns a conservative lower bound in {0,1,2}.
#
# No heuristic lookup table.
#
# A single face move can:
#   R : positions {0,1,3,4}
#   D : positions {3,4,5,6}
#   B : positions {1,2,4,5}
#
# We detect whether the one-corner state can be solved in one
# move. Otherwise we safely return 2.
# ============================================================

corner_lb:
    # index = (cubie*7 + position)*3 + orientation

    # cubie * 7
    slli t0, a0, 3
    sub  t0, t0, a0

    # + position
    add  t0, t0, a1

    # * 3
    slli t1, t0, 1
    add  t0, t0, t1

    # + orientation
    add  t0, t0, a2

    # lookup exact single-corner distance
    la   t1, corner_pdb
    add  t1, t1, t0
    lbu  a0, 0(t1)

    jalr x0, 0(ra)

heuristic:

    addi sp, sp, -32

    sw   ra, 28(sp)
    sw   s0, 24(sp)
    sw   s1, 20(sp)
    sw   s2, 16(sp)
    sw   s3, 12(sp)

    addi s0, a0, 0          # p
    addi s1, a1, 0          # o
    addi s2, x0, 0          # sum
    addi s3, x0, 0          # bad orientation count

    addi t0, x0, 0


heuristic_loop:

    add  t1, s0, t0
    lbu  a0, 0(t1)

    add  t1, s1, t0
    lbu  a2, 0(t1)

    addi a1, t0, 0

    jal  ra, corner_lb

    add  s2, s2, a0

    add  t1, s1, t0
    lbu  t2, 0(t1)

    beq  t2, x0, heuristic_not_bad
    addi s3, s3, 1

heuristic_not_bad:

    addi t0, t0, 1
    addi t1, x0, 7
    blt  t0, t1, heuristic_loop


    # ceil(sum/4)
    addi t0, s2, 3
    srli t0, t0, 2

    # ceil(bad/4)
    addi t1, s3, 3
    srli t1, t1, 2

    bge  t0, t1, heuristic_done

    addi t0, t1, 0


heuristic_done:

    addi a0, t0, 0

    lw   s3, 12(sp)
    lw   s2, 16(sp)
    lw   s1, 20(sp)
    lw   s0, 24(sp)
    lw   ra, 28(sp)

    addi sp, sp, 32

    jalr x0, 0(ra)


# ============================================================
# ida_search
#
# a0 = root p
# a1 = root o
# a2 = threshold
#
# Returns a0 = 1 if found, otherwise 0.
#
# NON-RECURSIVE.
# ============================================================

# ------------------------------------------------------------
# pdb_lookup(a0 = pointer to 7 orientation bytes)
# Return a0 = exact orientation-only distance.
# Index = o[0] + 3*o[1] + ... + 729*o[6].
# RV32I only: no multiplication or division.
# ------------------------------------------------------------
pdb_lookup:
    lbu  t0, 6(a0)
    slli t1, t0, 1
    add  t0, t0, t1
    lbu  t1, 5(a0)
    add  t0, t0, t1
    slli t1, t0, 1
    add  t0, t0, t1
    lbu  t1, 4(a0)
    add  t0, t0, t1
    slli t1, t0, 1
    add  t0, t0, t1
    lbu  t1, 3(a0)
    add  t0, t0, t1
    slli t1, t0, 1
    add  t0, t0, t1
    lbu  t1, 2(a0)
    add  t0, t0, t1
    slli t1, t0, 1
    add  t0, t0, t1
    lbu  t1, 1(a0)
    add  t0, t0, t1
    slli t1, t0, 1
    add  t0, t0, t1
    lbu  t1, 0(a0)
    add  t0, t0, t1

    la   t1, orientation_pdb
    add  t1, t1, t0
    lbu  a0, 0(t1)
    jalr x0, 0(ra)



# ------------------------------------------------------------
# perm_pdb_lookup(a0 = pointer to permutation[7])
# Return a0 = exact permutation-only distance.
# Lehmer rank without MUL or DIV (pure RV32I).
# ------------------------------------------------------------
perm_pdb_lookup:
    addi t0, x0, 0      # rank
    addi t1, x0, 0      # i

perm_rank_i:
    addi t2, x0, 0      # smaller count
    addi t3, t1, 1      # j

    add  t4, a0, t1
    lbu  t5, 0(t4)      # p[i]

perm_rank_j:
    addi t6, x0, 7
    bge  t3, t6, perm_rank_weight

    add  t4, a0, t3
    lbu  t6, 0(t4)      # p[j]

    bge  t6, t5, perm_rank_j_next
    addi t2, t2, 1

perm_rank_j_next:
    addi t3, t3, 1
    jal  x0, perm_rank_j

perm_rank_weight:
    la   t4, perm_factorials
    slli t6, t1, 2
    add  t4, t4, t6
    lw   t6, 0(t4)

    # rank += smaller * factorial[i]
    # repeated addition; smaller <= 6
perm_rank_add:
    beq  t2, x0, perm_rank_i_next
    add  t0, t0, t6
    addi t2, t2, -1
    jal  x0, perm_rank_add

perm_rank_i_next:
    addi t1, t1, 1
    addi t6, x0, 7
    blt  t1, t6, perm_rank_i

    addi a1, t0, 0
    la   t4, permutation_pdb
    add  t4, t4, t0
    lbu  a0, 0(t4)
    jalr x0, 0(ra)

ida_search:

    addi sp, sp, -64

    sw   ra, 60(sp)
    sw   s0, 56(sp)
    sw   s1, 52(sp)
    sw   s2, 48(sp)
    sw   s3, 44(sp)
    sw   s4, 40(sp)
    sw   s5, 36(sp)
    sw   s6, 32(sp)
    sw   s7, 28(sp)
    sw   s8, 24(sp)
    sw   s9, 20(sp)
    sw   s10,16(sp)
    sw   s11,12(sp)

    # s0 = root p
    # s1 = root o
    # s2 = sum
    # s3 = bad
    # s4 = d2 count
    # s5 = current frame
    # s6 = depth g
    # s7 = stack base
    # s8 = threshold
    # s9 = child frame
    # s10 = position index
    # s11 = current move face

    addi s0, a0, 0
    addi s1, a1, 0
    addi s8, a2, 0

    la   s7, stack_mem


    # --------------------------------------------------------
    # Build root frame
    # --------------------------------------------------------

    addi s2, x0, 0
    addi s3, x0, 0
    addi s4, x0, 0
    addi s10,x0, 0


root_init_loop:

    # p[i]
    add  t0, s0, s10
    lbu  t1, 0(t0)

    add  t0, s7, s10
    sb   t1, FP_P(t0)

    # o[i]
    add  t0, s1, s10
    lbu  t2, 0(t0)

    add  t0, s7, s10
    addi t0, t0, FP_O
    sb   t2, 0(t0)

    # corner lower bound
    addi a0, t1, 0
    addi a1, s10,0
    addi a2, t2, 0

    jal  ra, corner_lb

    add  s2, s2, a0

    # save per-position lower bound
    add  t0, s7, s10
    addi t0, t0, FP_CDIST
    sb   a0, 0(t0)

    # count lb == 2
    addi t3, x0, 2
    bne  a0, t3, root_not_d2
    addi s4, s4, 1

root_not_d2:

    # count non-zero orientation
    beq  t2, x0, root_not_bad
    addi s3, s3, 1

root_not_bad:

    addi s10, s10, 1
    addi t0, x0, 7
    blt  s10, t0, root_init_loop


    sw   s2, FP_SUM(s7)
    sw   s3, FP_BAD(s7)
    sw   s4, FP_D2(s7)

    sw   x0, FP_M(s7)

    addi t0, x0, -1
    sw   t0, FP_FACE(s7)


    # root h
    addi t0, s2, 3
    srli t0, t0, 2

    addi t1, s3, 3
    srli t1, t1, 2

    bge  t0, t1, root_h12

    addi t0, t1, 0


root_h12:

    # If any corner has lb 2, h >= 2.
    beq  s4, x0, root_h_done

    addi t1, x0, 2

    bge  t0, t1, root_h_done

    addi t0, x0, 2


root_h_done:

    # h = max(original h, orientation PDB)
    addi sp, sp, -16
    sw   t0, 0(sp)
    addi a0, s7, FP_O
    jal  ra, pdb_lookup
    lw   t0, 0(sp)
    addi sp, sp, 16

    bge  t0, a0, root_pdb_done
    addi t0, a0, 0

root_pdb_done:

    # Combine permutation PDB at root
    addi sp, sp, -16
    sw   t0, 0(sp)
    addi a0, s7, FP_P
    jal  ra, perm_pdb_lookup
    sw   a1, FP_PERM(s7)
    lw   t0, 0(sp)
    addi sp, sp, 16

    bge  t0, a0, root_perm_done
    addi t0, a0, 0

root_perm_done:
    # Cache compact orientation rank (six independent base-3 digits).
    # Two unused bytes in the existing 48-byte frame:
    #   byte 7  = orientation rank low 8 bits
    #   byte 43 = orientation rank high 8 bits
    # t0 holds root heuristic; do not clobber it.
    addi t3, x0, 5
    addi t4, x0, 0
root_ori_index_loop:
    add  t5, s7, t3
    lbu  t6, FP_O(t5)
    slli t1, t4, 1
    add  t4, t4, t1
    add  t4, t4, t6
    addi t3, t3, -1
    bge  t3, x0, root_ori_index_loop
    sb   t4, 7(s7)
    srli t3, t4, 8
    sb   t3, 43(s7)

    bgt  t0, s8, ida_false

    beq  t0, x0, ida_root_solved

    addi s6, x0, 0


# ------------------------------------------------------------
# Main iterative DFS loop
# ------------------------------------------------------------
ida_loop:

    # frame = stack + g*48
    slli t0, s6, 5
    slli t1, s6, 4
    add  t0, t0, t1
    add  s5, s7, t0

    lw   s4, FP_M(s5)

    addi t0, x0, 9
    bge  s4, t0, ida_backtrack


    # --------------------------------------------------------
    # move face
    # --------------------------------------------------------
    addi s11, x0, 0

    addi t0, x0, 3
    blt  s4, t0, face_ready

    addi s11, x0, 1

    addi t0, x0, 6
    blt  s4, t0, face_ready

    addi s11, x0, 2


face_ready:

    # same-face pruning
    lw   t0, FP_FACE(s5)

    bltz t0, face_ok

    beq  s11, t0, skip_move


face_ok:

    # child frame = current + 48
    addi s9, s5, FRAME_SIZE

    # Early permutation pruning before updating 7 corners
    lw   t3, FP_PERM(s5)

    # offset = (rank * 9 + move) * 2
    slli t4, t3, 3
    add  t4, t4, t3
    add  t4, t4, s4
    slli t4, t4, 1

    la   t5, perm_transitions
    add  t5, t5, t4
    lhu  t3, 0(t5)

    la   t5, permutation_pdb
    add  t5, t5, t3
    lbu  t6, 0(t5)

    # If g + 1 + h_perm > threshold, skip move
    addi t1, s6, 1
    add  t1, t1, t6
    bgt  t1, s8, skip_move
    # Cache next permutation rank once.  Do not look it up again after apply_loop.
    sw   t3, FP_PERM(s9)

    # Early orientation pruning: compact rank stored in two free frame bytes.
    lbu  t3, 7(s5)
    lbu  t4, 43(s5)
    slli t4, t4, 8
    or   t3, t3, t4
    # (ori_rank * 9 + move) * 2
    slli t4, t3, 3
    add  t4, t4, t3
    add  t4, t4, s4
    slli t4, t4, 1
    la   t5, ori_transitions
    add  t5, t5, t4
    lhu  t3, 0(t5)
    la   t5, orientation_compact_pdb
    add  t5, t5, t3
    lbu  t6, 0(t5)
    addi t1, s6, 1
    add  t1, t1, t6
    bgt  t1, s8, skip_move
    # Copy the unchanged baseline frame bytes in 12 memory instructions.
    # This preserves stationary corners and their cached per-corner distances.
    # Bytes 7 and 43 are only overwritten by the new compact orientation rank below.
    lw   t0, 0(s5)
    sw   t0, 0(s9)
    lw   t0, 4(s5)
    sw   t0, 4(s9)
    lw   t0, 8(s5)
    sw   t0, 8(s9)
    lw   t0, 12(s5)
    sw   t0, 12(s9)
    lw   t0, 36(s5)
    sw   t0, 36(s9)
    lw   t0, 40(s5)
    sw   t0, 40(s9)
    # Cache next compact orientation rank for descendants.
    sb   t3, 7(s9)
    srli t3, t3, 8
    sb   t3, 43(s9)

    # parent heuristic state
    lw   s2, FP_SUM(s5)
    lw   s3, FP_BAD(s5)
    lw   s0, FP_D2(s5)


    # source = move_source + m*8
    slli t0, s4, 3

    la   s1, move_source
    add  s1, s1, t0


    # record move
    la   t0, solution_path
    add  t0, t0, s6
    sb   s4, 0(t0)


    addi s10, x0, 0


# ------------------------------------------------------------
# Apply one move
# ------------------------------------------------------------
apply_loop:
    # Iterate only the four positions changed by this face move.
    la   t0, face_positions
    slli t1, s11, 2
    add  t0, t0, t1
    add  t0, t0, s10
    lbu  a1, 0(t0)          # destination position

    # source position and the cubie at source
    add  t0, s1, a1
    lbu  t1, 0(t0)          # source position
    add  t0, s5, t1
    lbu  t2, FP_P(t0)
    add  t0, s9, a1
    sb   t2, FP_P(t0)

    # New orientation = old orientation at source + move twist (modulo 3)
    add  t0, s5, t1
    lbu  a2, FP_O(t0)
    addi t6, s1, 72
    add  t6, t6, a1
    lbu  t0, 0(t6)
    add  a2, a2, t0
    addi t0, x0, 3
    blt  a2, t0, twist_ok
    addi a2, a2, -3

twist_ok:
    add  t0, s9, a1
    sb   a2, FP_O(t0)

    # Incrementally update sum and count of corners at distance exactly 2.
    add  t0, s5, a1
    lbu  t6, FP_CDIST(t0)
    sub  s2, s2, t6
    addi a0, t2, 0
    # a1 already holds the destination position, a2 the new orientation
    jal  ra, corner_lb
    add  s2, s2, a0
    add  t0, s9, a1
    sb   a0, FP_CDIST(t0)
    addi t0, x0, 2
    bne  t6, t0, old_not_d2
    addi s0, s0, -1
old_not_d2:
    bne  a0, t0, new_not_d2
    addi s0, s0, 1
new_not_d2:

    # Update twisted-corner count for the changed position.
    add  t0, s5, a1
    lbu  t6, FP_O(t0)
    beq  t6, x0, old_ori_ok
    addi s3, s3, -1
old_ori_ok:
    beq  a2, x0, new_ori_ok
    addi s3, s3, 1
new_ori_ok:
    addi s10, s10, 1
    addi t0, x0, 4
    blt  s10, t0, apply_loop

    # --------------------------------------------------------
    # Store child metadata
    # --------------------------------------------------------
    sw   s2, FP_SUM(s9)
    sw   s3, FP_BAD(s9)
    sw   s0, FP_D2(s9)
    sw   x0, FP_M(s9)
    sw   s11, FP_FACE(s9)


    # --------------------------------------------------------
    # child h
    # --------------------------------------------------------
    addi t0, s2, 3
    srli t0, t0, 2

    addi t1, s3, 3
    srli t1, t1, 2

    bge  t0, t1, child_h12

    addi t0, t1, 0


child_h12:

    beq  s0, x0, child_h_ready

    addi t1, x0, 2

    bge  t0, t1, child_h_ready

    addi t0, x0, 2


child_h_ready:
    # Both full PDB lower bounds have already been tested before apply_loop.
    # First test the incremental corner-based bound.
    addi t1, s6, 1
    add  t1, t1, t0
    bgt  t1, s8, advance_move

    # Three-corner position+orientation abstraction (cubies 0,1,2).
    # The cached child permutation rank indexes packed positions:
    # bits 0..2 = position of cubie 0
    # bits 3..5 = position of cubie 1
    # bits 6..8 = position of cubie 2
    lw   t3, FP_PERM(s9)
    slli t3, t3, 1
    la   t4, corner3_positions
    add  t4, t4, t3
    lhu  t3, 0(t4)
    andi a0, t3, 7
    srli a1, t3, 3
    andi a1, a1, 7
    srli a2, t3, 6
    andi a2, a2, 7

    # rank3positions = p0*30 + adjusted_p1*5 + adjusted_p2
    slli t3, a0, 5
    slli t4, a0, 1
    sub  t3, t3, t4
    slt  t4, a0, a1
    sub  t5, a1, t4
    slli t4, t5, 2
    add  t4, t4, t5
    add  t3, t3, t4
    slt  t4, a0, a2
    sub  t5, a2, t4
    slt  t4, a1, a2
    sub  t5, t5, t4
    add  t3, t3, t5

    # rank3positions * 27
    slli t4, t3, 5
    slli t5, t3, 2
    sub  t4, t4, t5
    sub  t3, t4, t3

    # Orientations of the same three cubies in the already-built child.
    add  t4, s9, a0
    lbu  t4, FP_O(t4)
    add  t5, s9, a1
    lbu  t5, FP_O(t5)
    add  t6, s9, a2
    lbu  t6, FP_O(t6)
    slli t1, t4, 3
    add  t1, t1, t4
    slli t4, t5, 1
    add  t4, t4, t5
    add  t1, t1, t4
    add  t1, t1, t6
    add  t3, t3, t1

    la   t4, corner3_pdb
    add  t4, t4, t3
    lbu  t3, 0(t4)
    addi t1, s6, 1
    add  t1, t1, t3
    bgt  t1, s8, advance_move

child_perm_done:
    beq  t0, x0, found_solution


    /* if depth+1+h > threshold -> next move */
    addi t1, s6, 1
    add  t1, t1, t0

    bgt  t1, s8, advance_move


    
    # Check actual search depth, not g+h
    addi t1, s6, 1
    addi t2, x0, MAX_DEPTH
    bge  t1, t2, advance_move



    # descend
    addi s6, s6, 1
    jal  x0, ida_loop


advance_move:

    addi s4, s4, 1
    sw   s4, FP_M(s5)

    jal  x0, ida_loop


skip_move:

    addi s4, s4, 1
    sw   s4, FP_M(s5)

    jal  x0, ida_loop


ida_backtrack:

    addi s6, s6, -1
    bltz s6, ida_false

    # parent frame
    slli t0, s6, 5
    slli t1, s6, 4
    add  t0, t0, t1
    add  t0, s7, t0

    lw   t1, FP_M(t0)
    addi t1, t1, 1
    sw   t1, FP_M(t0)

    jal  x0, ida_loop


found_solution:

    addi t0, s6, 1
    la   t1, solution_len
    sw   t0, 0(t1)

    addi a0, x0, 1
    jal  x0, ida_epilogue


ida_root_solved:

    la   t0, solution_len
    sw   x0, 0(t0)

    addi a0, x0, 1
    jal  x0, ida_epilogue


ida_false:

    addi a0, x0, 0


ida_epilogue:

    lw   s11,12(sp)
    lw   s10,16(sp)
    lw   s9, 20(sp)
    lw   s8, 24(sp)
    lw   s7, 28(sp)
    lw   s6, 32(sp)
    lw   s5, 36(sp)
    lw   s4, 40(sp)
    lw   s3, 44(sp)
    lw   s2, 48(sp)
    lw   s1, 52(sp)
    lw   s0, 56(sp)
    lw   ra, 60(sp)

    addi sp, sp, 64
    jalr x0, 0(ra)


# ============================================================
# validate_solution
#
# a0 = original p*
# a1 = original o*
#
# Replays the found path and verifies the final state.
# ============================================================
validate_solution:

    addi sp, sp, -80

    sw   ra, 76(sp)
    sw   s0, 72(sp)
    sw   s1, 68(sp)
    sw   s2, 64(sp)
    sw   s3, 60(sp)
    sw   s4, 56(sp)
    sw   s5, 52(sp)
    sw   s6, 48(sp)
    sw   s7, 44(sp)
    sw   s8, 40(sp)

    addi s0, a0, 0
    addi s1, a1, 0

    # buffer A: p@sp, o@sp+8
    addi s2, sp, 0

    # buffer B: p@sp+16, o@sp+24
    addi s3, sp, 16


    # copy input into A
    addi s6, x0, 0

vs_copy:

    add  t0, s0, s6
    lbu  t1, 0(t0)

    add  t0, s2, s6
    sb   t1, 0(t0)

    add  t0, s1, s6
    lbu  t1, 0(t0)

    add  t0, s2, s6
    addi t0, t0, 8
    sb   t1, 0(t0)

    addi s6, s6, 1
    addi t0, x0, 7
    blt  s6, t0, vs_copy


    addi s4, s2, 0
    addi s5, s2, 8

    addi s6, s3, 0
    addi s7, s3, 8

    addi s8, x0, 0

    la   t0, solution_len
    lw   t1, 0(t0)


vs_move_loop:

    bge  s8, t1, vs_check_goal

    la   t0, solution_path
    add  t0, t0, s8
    lbu  t2, 0(t0)

    # source = move_source + m*8
    slli t0, t2, 3

    la   t3, move_source
    add  t3, t3, t0

    # twist = source + 72
    addi t4, t3, 72

    addi t5, x0, 0


vs_apply:

    # src
    add  t0, t3, t5
    lbu  t6, 0(t0)

    # new p
    add  t0, s4, t6
    lbu  a0, 0(t0)

    add  t0, s6, t5
    sb   a0, 0(t0)

    # new o
    add  t0, s5, t6
    lbu  a1, 0(t0)

    add  t0, t4, t5
    lbu  a2, 0(t0)

    add  a1, a1, a2

    addi t0, x0, 3
    blt  a1, t0, vs_tw_ok

    addi a1, a1, -3


vs_tw_ok:

    add  t0, s7, t5
    sb   a1, 0(t0)


    addi t5, t5, 1

    addi t0, x0, 7
    blt  t5, t0, vs_apply


    # swap buffers
    addi t0, s4, 0
    addi s4, s6, 0
    addi s6, t0, 0

    addi t0, s5, 0
    addi s5, s7, 0
    addi s7, t0, 0


    addi s8, s8, 1
    jal  x0, vs_move_loop


vs_check_goal:

    addi s8, x0, 0


vs_goal_loop:

    add  t0, s4, s8
    lbu  t1, 0(t0)

    bne  t1, s8, vs_bad

    add  t0, s5, s8
    lbu  t1, 0(t0)

    bne  t1, x0, vs_bad

    addi s8, s8, 1

    addi t0, x0, 7
    blt  s8, t0, vs_goal_loop


    addi a0, x0, 1
    jal  x0, vs_epilogue


vs_bad:

    addi a0, x0, 0


vs_epilogue:

    lw   s8,40(sp)
    lw   s7,44(sp)
    lw   s6,48(sp)
    lw   s5,52(sp)
    lw   s4,56(sp)
    lw   s3,60(sp)
    lw   s2,64(sp)
    lw   s1,68(sp)
    lw   s0,72(sp)
    lw   ra,76(sp)

    addi sp, sp, 80
    jalr x0, 0(ra)


# ============================================================
# run_test
#
# a0 = test name
# a1 = 14-character state string
# a2 = expected distance
# ============================================================
run_test:

    addi sp, sp, -64

    sw   ra, 60(sp)
    sw   s0, 56(sp)
    sw   s1, 52(sp)
    sw   s2, 48(sp)
    sw   s3, 44(sp)
    sw   s4, 40(sp)
    sw   s5, 36(sp)


    addi s0, a0, 0
    addi s1, a1, 0
    addi s2, a2, 0


    # [name] Input: state
    la   a0, str_lbracket
    jal  ra, print_str

    addi a0, s0, 0
    jal  ra, print_str

    la   a0, str_input
    jal  ra, print_str

    addi a0, s1, 0
    jal  ra, print_str

    la   a0, str_nl
    jal  ra, print_str


    # p buffer = sp+16
    # o buffer = sp+24
    addi s3, sp, 16
    addi s4, sp, 24

    addi s5, x0, 0


rt_decode:

    # p[i] = state[i]-'1'
    add  t0, s1, s5
    lbu  t1, 0(t0)

    addi t1, t1, -49

    add  t0, s3, s5
    sb   t1, 0(t0)


    # o[i] = state[7+i]-'1'
    add  t0, s1, s5
    addi t0, t0, 7
    lbu  t1, 0(t0)

    addi t1, t1, -49

    add  t0, s4, s5
    sb   t1, 0(t0)


    addi s5, s5, 1

    addi t0, x0, 7
    blt  s5, t0, rt_decode


    # start heuristic
    addi a0, s3, 0
    addi a1, s4, 0
    jal  ra, heuristic

    addi s5, a0, 0


    # Solved immediately

    # Initial threshold = max(original, orientation PDB, permutation PDB)

    # Orientation PDB
    addi a0, s4, 0
    jal  ra, pdb_lookup
    bge  s5, a0, rt_ori_h_done
    addi s5, a0, 0

rt_ori_h_done:

    # Permutation PDB
    addi a0, s3, 0
    jal  ra, perm_pdb_lookup
    bge  s5, a0, rt_perm_h_done
    addi s5, a0, 0

rt_perm_h_done:

    beq  s5, x0, rt_solved_zero



rt_threshold_loop:

    addi t1, x0, MAX_DEPTH
    bgt  s5, t1, rt_no_solution

    # Debug: show current IDA* threshold
    la   a0, str_ida_bound
    jal  ra, print_str
    addi a0, s5, 0
    jal  ra, print_num
    la   a0, str_nl
    jal  ra, print_str



    addi a0, s3, 0
    addi a1, s4, 0
    addi a2, s5, 0

    jal  ra, ida_search

    beq  a0, x0, rt_next_threshold


    # -----------------------------------------
    # Print found length
    # -----------------------------------------
    la   t1, solution_len
    lw   t1, 0(t1)

    addi s5, t1, 0

    la   a0, str_solved
    jal  ra, print_str

    addi a0, s5, 0
    jal  ra, print_num

    la   a0, str_moves
    jal  ra, print_str


    # -----------------------------------------
    # Print solution path
    # -----------------------------------------
    addi s0, x0, 0


rt_print_moves:

    bge  s0, s5, rt_print_done

    la   t0, solution_path
    add  t0, t0, s0
    lbu  a0, 0(t0)

    jal  ra, print_move

    addi t0, s5, -1
    beq  s0, t0, rt_no_space

    la   a0, str_space
    jal  ra, print_str


rt_no_space:

    addi s0, s0, 1
    jal  x0, rt_print_moves


rt_print_done:

    # -----------------------------------------
    # Validate actual path
    # -----------------------------------------
    addi a0, s3, 0
    addi a1, s4, 0

    jal  ra, validate_solution

    addi s0, a0, 0


    # solution length == expected
    la   t0, solution_len
    lw   t0, 0(t0)

    sub  t1, t0, s2

    beq  t1, x0, rt_len_ok

    addi s0, x0, 0


rt_len_ok:

    beq  s0, x0, rt_fail

    la   a0, str_pass
    jal  ra, print_str

    addi a0, x0, 1
    jal  x0, rt_epilogue


rt_solved_zero:

    la   a0, str_solved
    jal  ra, print_str

    addi a0, x0, 0
    jal  ra, print_num

    la   a0, str_moves
    jal  ra, print_str

    beq  s2, x0, rt_zero_pass

    la   a0, str_fail
    jal  ra, print_str

    addi a0, x0, 0
    jal  x0, rt_epilogue


rt_zero_pass:

    la   a0, str_pass
    jal  ra, print_str

    addi a0, x0, 1
    jal  x0, rt_epilogue


rt_next_threshold:

    addi s5, s5, 1
    jal  x0, rt_threshold_loop


rt_no_solution:

    la   a0, str_nosol
    jal  ra, print_str

    addi a0, x0, 0


rt_fail:

    la   a0, str_fail
    jal  ra, print_str

    addi a0, x0, 0


rt_epilogue:

    lw   s5,36(sp)
    lw   s4,40(sp)
    lw   s3,44(sp)
    lw   s2,48(sp)
    lw   s1,52(sp)
    lw   s0,56(sp)
    lw   ra,60(sp)

    addi sp, sp, 64
    jalr x0, 0(ra)
