#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "VM_Value.h"

namespace GM {

/**
 * GML VM Instruction Set
 * Based on GameMaker bytecode opcodes
 */
enum class OpCode {
    // Stack operations
    PUSH,           // Push constant onto stack
    POP,            // Pop from stack into variable
    PUSHI,          // Push integer
    PUSHF,          // Push float
    PUSHS,          // Push string
    PUSHB,          // Push bool
    PUSHU,          // Push undefined
    PUSHVN,         // Push variable by name
    POPVN,          // Pop into variable by name

    // Arithmetic
    ADD,            // Addition
    SUB,            // Subtraction
    MUL,            // Multiplication
    DIV,            // Division
    MOD,            // Modulo
    NEG,            // Negate

    // Bitwise
    AND,            // Bitwise AND
    OR,             // Bitwise OR
    XOR,            // Bitwise XOR
    COM,            // Bitwise complement
    SHL,            // Shift left
    SHR,            // Shift right

    // Logical
    TEQ,            // Test equal
    TNE,            // Test not equal
    TLT,            // Test less than
    TLE,            // Test less than or equal
    TGT,            // Test greater than
    TGE,            // Test greater than or equal
    LAND,           // Logical AND
    LOR,            // Logical OR
    NOT,            // Logical NOT

    // Control flow
    JMP,            // Unconditional jump
    BT,             // Branch if true
    BF,             // Branch if false
    RET,            // Return from function
    CALL,           // Call function
    CALLV,          // Call function by variable
    NOP,            // No operation
    EXIT,           // Exit game

    // Variables
    LDGLB,          // Load global variable
    STGLB,          // Store global variable
    LDLOC,          // Load local variable
    STLOC,          // Store local variable
    LDINST,         // Load instance variable
    STINST,         // Store instance variable

    // Type conversion
    CONV,           // Type conversion

    // Stack manipulation
    DUP,            // Duplicate top of stack
    DROP,           // Discard top of stack

    // End marker
    INVALID
};

/**
 * Single VM instruction
 */
struct Instruction {
    OpCode op = OpCode::INVALID;
    Value operand1;
    Value operand2;
    std::string operandStr;  // For string operands
    int32_t jumpTarget = -1; // For JMP/BT/BF

    Instruction() = default;
    explicit Instruction(OpCode op) : op(op) {}
    Instruction(OpCode op, const Value& o1, const Value& o2, const std::string& s = "") 
        : op(op), operand1(o1), operand2(o2), operandStr(s) {}
};

/**
 * Code block - sequence of instructions
 */
struct CodeBlock {
    std::string name;
    std::vector<Instruction> instructions;
    int id = -1;
    
    CodeBlock() = default;
    explicit CodeBlock(const std::string& n) : name(n) {}
};

} // namespace GM
