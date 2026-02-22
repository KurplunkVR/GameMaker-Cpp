#include "VM_Executor.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace GM {

VirtualMachine::VirtualMachine() {
}

void VirtualMachine::AddCodeBlock(const CodeBlock& block) {
    codeBlocks_[block.name] = block;
}

void VirtualMachine::LoadCodeBlocks(const std::vector<CodeBlock>& blocks) {
    for (const auto& block : blocks) {
        AddCodeBlock(block);
    }
}

Value VirtualMachine::ExecuteFunction(const std::string& functionName) {
    auto it = codeBlocks_.find(functionName);
    if (it == codeBlocks_.end()) {
        LogDebug("Function not found: " + functionName);
        return Value(0.0);  // Return 0 if function not found
    }

    ExecutionFrame frame(functionName);
    callStack_.push_back(frame);
    
    Execute(it->second);
    
    Value result = callStack_.back().returnValue;
    callStack_.pop_back();
    
    return result;
}

void VirtualMachine::Execute(const CodeBlock& code) {
    currentCode_ = const_cast<CodeBlock*>(&code);
    instructionPointer_ = 0;

    while (instructionPointer_ < code.instructions.size()) {
        try {
            const auto& instr = code.instructions[instructionPointer_];
            ExecuteInstruction(instr);
            
            // Check for early return
            if (!callStack_.empty() && callStack_.back().returnValue.AsReal() != 0) {
                // In a simple VM, we exit on return
                break;
            }
        } catch (const std::exception& e) {
            LogDebug("Execution error: " + std::string(e.what()));
            break;
        }

        instructionPointer_++;
    }
}

Value VirtualMachine::ExecuteInstruction(const Instruction& instr) {
    switch (instr.op) {
        // Stack operations
        case OpCode::PUSH:
            PushStack(instr.operand1);
            break;

        case OpCode::PUSHI:
            PushStack(Value(instr.operand1.AsReal()));
            break;

        case OpCode::PUSHF:
            PushStack(Value(instr.operand1.AsReal()));
            break;

        case OpCode::PUSHS:
            PushStack(Value(instr.operandStr));
            break;

        case OpCode::PUSHB:
            PushStack(Value(static_cast<bool>(instr.operand1.AsReal())));
            break;

        case OpCode::PUSHU:
            PushStack(Value());  // Undefined
            break;

        case OpCode::POP: {
            Value val = PopStack();
            if (!instr.operandStr.empty()) {
                globals_[instr.operandStr] = val;
            }
            break;
        }

        // Arithmetic
        case OpCode::ADD: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a + b);
            break;
        }

        case OpCode::SUB: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a - b);
            break;
        }

        case OpCode::MUL: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a * b);
            break;
        }

        case OpCode::DIV: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a / b);
            break;
        }

        case OpCode::MOD: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a % b);
            break;
        }

        case OpCode::NEG: {
            Value a = PopStack();
            PushStack(-a);
            break;
        }

        // Bitwise
        case OpCode::AND: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a & b);
            break;
        }

        case OpCode::OR: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a | b);
            break;
        }

        case OpCode::XOR: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a ^ b);
            break;
        }

        case OpCode::COM: {
            Value a = PopStack();
            PushStack(~a);
            break;
        }

        case OpCode::SHL: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a << b);
            break;
        }

        case OpCode::SHR: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(a >> b);
            break;
        }

        // Comparison
        case OpCode::TEQ: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a == b ? 1.0 : 0.0));
            break;
        }

        case OpCode::TNE: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a != b ? 1.0 : 0.0));
            break;
        }

        case OpCode::TLT: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a < b ? 1.0 : 0.0));
            break;
        }

        case OpCode::TLE: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a <= b ? 1.0 : 0.0));
            break;
        }

        case OpCode::TGT: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a > b ? 1.0 : 0.0));
            break;
        }

        case OpCode::TGE: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a >= b ? 1.0 : 0.0));
            break;
        }

        // Logical
        case OpCode::LAND: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a.AsBool() && b.AsBool() ? 1.0 : 0.0));
            break;
        }

        case OpCode::LOR: {
            Value b = PopStack();
            Value a = PopStack();
            PushStack(Value(a.AsBool() || b.AsBool() ? 1.0 : 0.0));
            break;
        }

        case OpCode::NOT: {
            Value a = PopStack();
            PushStack(!a);
            break;
        }

        // Control flow
        case OpCode::JMP:
            if (instr.jumpTarget >= 0) {
                instructionPointer_ = instr.jumpTarget - 1;  // -1 because loop will increment
            }
            break;

        case OpCode::BT: {
            Value cond = PopStack();
            if (cond.AsBool() && instr.jumpTarget >= 0) {
                instructionPointer_ = instr.jumpTarget - 1;
            }
            break;
        }

        case OpCode::BF: {
            Value cond = PopStack();
            if (!cond.AsBool() && instr.jumpTarget >= 0) {
                instructionPointer_ = instr.jumpTarget - 1;
            }
            break;
        }

        case OpCode::RET: {
            Value ret = PopStack();
            if (!callStack_.empty()) {
                callStack_.back().returnValue = ret;
            }
            return ret;
        }

        case OpCode::CALL: {
            // Simple function call by name
            if (!instr.operandStr.empty()) {
                ExecuteFunction(instr.operandStr);
            }
            break;
        }

        case OpCode::NOP:
            // No operation
            break;

        case OpCode::DUP: {
            Value val = PeekStack();
            PushStack(val);
            break;
        }

        case OpCode::DROP: {
            PopStack();
            break;
        }

        default:
            LogDebug("Unknown opcode: " + OpCodeToString(instr.op));
            break;
    }

    return Value();
}

Value VirtualMachine::PopStack() {
    if (stack_.empty()) {
        LogDebug("Stack underflow!");
        return Value(0.0);
    }
    Value val = stack_.top();
    stack_.pop();
    return val;
}

void VirtualMachine::PushStack(const Value& v) {
    stack_.push(v);
}

Value VirtualMachine::PeekStack() const {
    if (stack_.empty()) {
        return Value(0.0);
    }
    return stack_.top();
}

Value VirtualMachine::CallBuiltIn(const std::string& name, const std::vector<Value>& args) {
    // Basic built-in functions
    if (name == "print" && !args.empty()) {
        std::cout << args[0].AsString() << std::endl;
        return args[0];
    }
    if (name == "abs" && !args.empty()) {
        return Value(std::abs(args[0].AsReal()));
    }
    if (name == "round" && !args.empty()) {
        return Value(std::round(args[0].AsReal()));
    }
    if (name == "floor" && !args.empty()) {
        return Value(std::floor(args[0].AsReal()));
    }
    if (name == "ceil" && !args.empty()) {
        return Value(std::ceil(args[0].AsReal()));
    }
    if (name == "sqrt" && !args.empty()) {
        return Value(std::sqrt(args[0].AsReal()));
    }
    if (name == "sin" && !args.empty()) {
        return Value(std::sin(args[0].AsReal()));
    }
    if (name == "cos" && !args.empty()) {
        return Value(std::cos(args[0].AsReal()));
    }
    if (name == "tan" && !args.empty()) {
        return Value(std::tan(args[0].AsReal()));
    }
    
    return Value(0.0);
}

std::string VirtualMachine::OpCodeToString(OpCode op) const {
    switch (op) {
        case OpCode::PUSH: return "PUSH";
        case OpCode::POP: return "POP";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::JMP: return "JMP";
        case OpCode::BT: return "BT";
        case OpCode::BF: return "BF";
        case OpCode::RET: return "RET";
        case OpCode::CALL: return "CALL";
        case OpCode::NOP: return "NOP";
        default: return "UNKNOWN";
    }
}

void VirtualMachine::LogDebug(const std::string& msg) const {
    if (debugOutput_) {
        std::cout << "[VM] " << msg << std::endl;
    }
}

std::string VirtualMachine::GetCallStack() const {
    std::string result = "Call Stack:\n";
    for (size_t i = 0; i < callStack_.size(); ++i) {
        result += "  [" + std::to_string(i) + "] " + callStack_[i].functionName + 
                  " @ " + std::to_string(callStack_[i].instructionPointer) + "\n";
    }
    return result;
}

} // namespace GM
