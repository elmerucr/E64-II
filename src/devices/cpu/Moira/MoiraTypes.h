// -----------------------------------------------------------------------------
// This file is part of Moira - A Motorola 68k emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef MOIRA_TYPES_H
#define MOIRA_TYPES_H

#include <stdint.h>

namespace moira {

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef enum
{
    M68000     // We only support the 68000 yet
}
CPUModel;

typedef enum
{
    ILLEGAL,   // Illegal instruction
    LINE_A,    // Unused instruction (line A)
    LINE_F,    // Unused instruction (line F)

    ABCD,      // Add decimal with extend
    ADD,       // Add binary
    ADDA,      // Add address
    ADDI,      // Add immediate
    ADDQ,      // Add quick
    ADDX,      // Add extended
    AND,       // AND logical
    ANDI,      // AND immediate
    ANDICCR,   // AND immediate to condition code register
    ANDISR,    // AND immediate to status register
    ASL,       // Arithmetic shift left
    ASR,       // Arithmetic shift right
    BCC,       // Branch on carry clear
    BCS,       // Branch on carry set
    BEQ,       // Branch on equal
    BGE,       // Branch on greater than or equal
    BGT,       // Branch on greater than
    BHI,       // Branch on higher than
    BLE,       // Branch on less than or equal
    BLS,       // Branch on lower than or same
    BLT,       // Branch on less than
    BMI,       // Branch on minus
    BNE,       // Branch on not equal
    BPL,       // Branch on plus
    BVC,       // Branch on overflow clear
    BVS,       // Branch on overflow set
    BCHG,      // Test a bit and change
    BCLR,      // Test a bit and clear
    BRA,       // Branch always
    BSET,      // Test a bit and set
    BSR,       // Branch to subroutine
    BTST,      // Test a bit
    CHK,       // Check register against bounds
    CLR,       // Clear an operand
    CMP,       // Compare
    CMPA,      // Compare address
    CMPI,      // Compare immediate
    CMPM,      // Compare memory with memory
    DBCC,      // Test, decrement, and branch on carry clear
    DBCS,      // Test, decrement, and branch on carry set
    DBEQ,      // Test, decrement, and branch on equal
    DBGE,      // Test, decrement, and branch on greater than or equal
    DBGT,      // Test, decrement, and branch on greater than
    DBHI,      // Test, decrement, and branch on higher than
    DBLE,      // Test, decrement, and branch on less than or equal
    DBLS,      // Test, decrement, and branch on lower than or same
    DBLT,      // Test, decrement, and branch on less than
    DBMI,      // Test, decrement, and branch on minus
    DBNE,      // Test, decrement, and branch on not equal
    DBPL,      // Test, decrement, and branch on on plus
    DBVC,      // Test, decrement, and branch on overflow clear
    DBVS,      // Test, decrement, and branch on overflow set
    DBF,       // Test, decrement, and branch on false (never)
    DBT,       // Test, decrement, and branch on true (always)
    DIVS,      // Signed divide
    DIVU,      // Unsigned divide
    EOR,       // Exclusive OR logical
    EORI,      // Exclusive OR immediate
    EORICCR,   // Exclusive OR immediate to condition code register
    EORISR,    // Exclusive OR immediate to status register
    EXG,       // Exchange registers
    EXT,       // Sign-extend a data register
    JMP,       // Jump
    JSR,       // Jump to subroutine
    LEA,       // Load effective address
    LINK,      // Link and allocate
    LSL,       // Logical shift left
    LSR,       // Logical shift right
    MOVE,      // Copy data from source to destination
    MOVEA,     // Move address
    MOVECCR,   // Copy data to condition code register from source
    MOVEFSR,   // Copy data from status register to destination
    MOVETSR,   // Copy data to status register from source
    MOVEUSP,   // Copy data to or from USP
    MOVEM,     // Move multiple registers
    MOVEP,     // Move peripheral data
    MOVEQ,     // Move quick
    MULS,      // Signed multiply
    MULU,      // Unsigned multiply
    NBCD,      // Negate decimal with sign extend
    NEG,       // Negate
    NEGX,      // Negate with extend
    NOP,       // No operation
    NOT,       // Logical complement
    OR,        // OR logical
    ORI,       // OR immediate
    ORICCR,    // OR immediate to condition code register
    ORISR,     // OR immediate to status register
    PEA,       // Push effective address
    RESET,     // Reset external devices
    ROL,       // Rotate left
    ROR,       // Rotate right
    ROXL,      // Rotate left with extend
    ROXR,      // Rotate righ with extend
    RTE,       // Return from exception
    RTR,       // Return and restore condition codes
    RTS,       // Return from subroutine
    SBCD,      // Subtract decimal with extend
    SCC,       // Set on carry clear
    SCS,       // Set on carry set
    SEQ,       // Set on equal
    SGE,       // Set on greater than or equal
    SGT,       // Set on greater than
    SHI,       // Set on higher than
    SLE,       // Set on less than or equal
    SLS,       // Set on lower than or same
    SLT,       // Set on less than
    SMI,       // Set on minus
    SNE,       // Set on not equal
    SPL,       // Set on plus
    SVC,       // Set on overflow clear
    SVS,       // Set on overflow set
    SF,        // Set on false (never set)
    ST,        // Set on true (always set)
    STOP,      // Load status register and stop
    SUB,       // Subtract binary
    SUBA,      // Subtract address
    SUBI,      // Subtract immediate
    SUBQ,      // Subtract quick
    SUBX,      // Subtract extended
    SWAP,      // Swap register halves
    TAS,       // Test and set an operand
    TRAP,      // Trap
    TRAPV,     // Trap on overflow
    TST,       // Test an operand
    UNLK       // Unlink
}
Instr;

typedef enum {

    MODE_DN,   //           Dn : Data register direct
    MODE_AN,   //           An : Address register direct
    MODE_AI,   //         (An) : Register indirect
    MODE_PI,   //        (An)+ : Postincrement register indirect
    MODE_PD,   //        -(An) : Predecrement register indirect
    MODE_DI,   //       (d,An) : Register indirect with displacement
    MODE_IX,   //    (d,An,Xi) : Indexed register indirect with displacement
    MODE_AW,   //     (####).w : Absolute addressing short
    MODE_AL,   //     (####).l : Absolute addressing long
    MODE_DIPC, //       (d,PC) : PC relative with displacement
    MODE_PCIX, //    (d,An,Xi) : Indexed PC relative with displacement
    MODE_IM,   //         #### : Immediate data addressing
    MODE_IP    //         ---- : Implied addressing
}
Mode;

inline bool isRegMode(Mode M) { return M == 0 || M == 1;  }
inline bool isAbsMode(Mode M) { return M == 7 || M == 8;  }
inline bool isIdxMode(Mode M) { return M == 6 || M == 10; }
inline bool isMemMode(Mode M) { return M >= 2 && M <= 10; }
inline bool isPrgMode(Mode M) { return M == 9 || M == 10; }
inline bool isImmMode(Mode M) { return M == 11; }

typedef enum
{
    Byte = 1,  // .b : Byte addressing
    Word = 2,  // .w : Word addressing
    Long = 4   // .l : Long word addressing
}
Size;

typedef struct
{
    Instr I;
    Mode  M;
    Size  S;
}
InstrInfo;

typedef enum
{
    IRQ_AUTO,
    IRQ_USER,
    IRQ_SPURIOUS,
    IRQ_UNINITIALIZED
}
IrqMode;

typedef enum
{
    FC_USER_DATA       = 1,
    FC_USER_PROG       = 2,
    FC_SUPERVISOR_DATA = 5,
    FC_SUPERVISOR_PROG = 6
}
FunctionCode;

struct StatusRegister {

    bool t;               // Trace flag
    bool s;               // Supervisor flag
    bool x;               // Extend flag
    bool n;               // Negative flag
    bool z;               // Zero flag
    bool v;               // Overflow flag
    bool c;               // Carry flag

    u8 ipl;               // Required Interrupt Priority Level
};

struct Registers {

    u32 pc;               // Program counter
    StatusRegister sr;    // Status register

    union {
        struct {
            u32 d[8];     // D0, D1 ... D7
            u32 a[8];     // A0, A1 ... A7
        };
        struct {
            u32 r[16];    // D0, D1 ... D7, A0, A1 ... A7
        };
        struct {
            u32 _pad[15];
            u32 sp;       // Visible stack pointer (overlays a[7])
        };
    };

    u32 usp;              // User Stack Pointer
    u32 ssp;              // Supervisor Stack Pointer

    u8 ipl;               // Polled Interrupt Priority Level
};

}
#endif
