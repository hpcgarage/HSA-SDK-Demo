#!/bin/bash

# Assenble HSAIL
hsailasm -o=demo.brig demo.hsail

# Extract sections of the generated BRIG (it has ELF format)
objdump -h demo.brig | grep .strtab | awk '{print "dd if=demo.brig of=demo.strtab bs=1 count=$[0x" $3 "] skip=$[0x" $6 "]"}' | bash
objdump -h demo.brig | grep .code | awk '{print "dd if=demo.brig of=demo.code bs=1 count=$[0x" $3 "] skip=$[0x" $6 "]"}' | bash
objdump -h demo.brig | grep .operands | awk '{print "dd if=demo.brig of=demo.operands bs=1 count=$[0x" $3 "] skip=$[0x" $6 "]"}' | bash
objdump -h demo.brig | grep .directives | awk '{print "dd if=demo.brig of=demo.directives bs=1 count=$[0x" $3 "] skip=$[0x" $6 "]"}' | bash
objdump -h demo.brig | grep .debug | awk '{print "dd if=demo.brig of=demo.debug bs=1 count=$[0x" $3 "] skip=$[0x" $6 "]"}' | bash
# Convert extracted sections to C byte sequence
bin2h 80 < demo.strtab > demo.strtab.h
bin2h 80 < demo.code > demo.code.h
bin2h 80 < demo.operands > demo.operands.h
bin2h 80 < demo.directives > demo.directives.h
bin2h 80 < demo.debug > demo.debug.h
