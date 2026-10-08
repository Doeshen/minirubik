#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
for f in orientation_pdb.bin permutation_pdb.bin permutation_transition.bin corner_pdb.bin; do
    if [[ ! -f "$f" ]]; then
        echo "Missing required existing PDB: $f" >&2
        exit 1
    fi
done
cc -O3 -std=c99 -Wall -Wextra -Wpedantic gen_orientation_fast.c -o gen_orientation_fast
./gen_orientation_fast
cc -O3 -std=c99 -Wall -Wextra -Wpedantic gen_corner3.c -o gen_corner3
./gen_corner3 0 1 2
cc -O3 -std=c99 -Wall -Wextra -Wpedantic verify_corner3_full.c -o verify_corner3_full
./verify_corner3_full
riscv64-unknown-elf-gcc \
    -march=rv32i -mabi=ilp32 -nostdlib -nostartfiles \
    -msmall-data-limit=0 -Wl,-e,_start \
    -o target_rv32i_optimized_v4.elf target_rv32i_optimized_v4.S
riscv64-unknown-elf-gcc \
    -march=rv32i -mabi=ilp32 -nostdlib -nostartfiles \
    -msmall-data-limit=0 -Wl,-e,_start \
    -o target_rv32i_h3_v4.elf target_rv32i_h3_v4.S
riscv64-unknown-elf-size -A target_rv32i_optimized_v4.elf
STATIC_BYTES="$(riscv64-unknown-elf-size -A target_rv32i_optimized_v4.elf | awk '$1==".data"||$1==".rodata"||$1==".bss"{sum+=$2} END{print sum+0}')"
echo "STATIC DATA: $STATIC_BYTES / 131072 bytes"
if (( STATIC_BYTES > 131072 )); then
    echo "FAIL: static data exceeds assignment limit" >&2
    exit 1
fi
echo "BUILD V4 PASSED. Old PASS_36M source/ELF unchanged."
