RV32I minirubik optimized V4 - based directly on the user's PASS_36M assembly.

DO NOT DELETE OR OVERWRITE target_rv32i_PASS_36M.S or its ELF.

Files in this bundle:
  target_rv32i_optimized_v4.S   3 original tests (0,1,11 moves)
  target_rv32i_h3_v4.S          single distance-11 test, change test3_state only
  gen_orientation_fast.c       builds orientation_compact.bin and orientation_transition.bin
  gen_corner3.c                builds corner3_pdb.bin and corner3_positions.bin (cubies 0,1,2)
  verify_corner3_full.c        complete BFS (3,674,160 states) verifying the new pattern PDB
  build_v4.sh                  build script for WSL Ubuntu
  v4_local_counts.tsv          local RV32I emulator results for all 2,644 distance-11 states

Put all files alongside the original .bin files in your minirubik directory.
In WSL Ubuntu:
    cd /mnt/c/Users/Winson/minirubik
    bash build_v4.sh

The build script uses four original .bin files already in your folder.
It generates four new .bin files (two orientation, two corner3) and links two ELF files.
It does not alter the existing PASS_36M source or ELF.

In Ripes load target_rv32i_optimized_v4.elf for three tests.
Load target_rv32i_h3_v4.elf to measure the fixed test3 state alone.
For other distance-11 inputs, replace the .asciz directly after test3_state: in
  target_rv32i_h3_v4.S, or use the existing batch substitution workflow.

LOCAL TEST SUMMARY (NOT OFFICIAL Ripes RV32_ISS measurement):
- 2,644 / 2,644 distance-11 states passed the actual RV32I ELF execution.
- Local emulator maximum: 39,623,636 retired instructions, for state 54721631111111.
- No case exceeded 50,000,000 in this local emulator.
- A control run on the original PASS_36M program reproduced 36,490,942 instructions.

FINAL OFFICIAL EVIDENCE: Run the user-pinned Ripes RV32_ISS with --iret, and
confirm all states and code size. Do not claim that the local emulator is Ripes.
