#pragma once

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <memory>
#include "VM_Value.h"
#include "VM_Instruction.h"

namespace GM {

/**
 * Execution context for a function call
 */
struct ExecutionFrame {
    std::string functionName;
    size_t instructionPointer = 0;
    std::map<std::string, Value> locals;
    Value returnValue;
    
    ExecutionFrame() = default;
    explicit ExecutionFrame(const std::string& name) : functionName(name) {}
};

/**
 * GML Virtual Machine
 * Stack-based bytecode interpreter for GameMaker code
 */
class VirtualMachine {
public:
    VirtualMachine();
    ~VirtualMachine() = default;

    // Load code blocks
    void AddCodeBlock(const CodeBlock& block);
    void LoadCodeBlocks(const std::vector<CodeBlock>& blocks);

    // Execution
    Value ExecuteFunction(const std::string& functionName);
    bool IsValid() const { return !codeBlocks_.empty(); }

    // Debugging
    void SetDebugOutput(bool enabled) { debugOutput_ = enabled; }
    std::string GetCallStack() const;

private:
    // Code storage
    std::map<std::string, CodeBlock> codeBlocks_;
    
    // Execution state
    std::stack<Value> stack_;
    std::vector<ExecutionFrame> callStack_;
    std::map<std::string, Value> globals_;

    // Current execution
    CodeBlock* currentCode_ = nullptr;
    size_t instructionPointer_ = 0;
    
    // Debug
    bool debugOutput_ = false;

    // Execution
    void Execute(const CodeBlock& code);
    Value ExecuteInstruction(const Instruction& instr);
    
    // Stack operations
    Value PopStack();
    void PushStack(const Value& v);
    Value PeekStack() const;
    
    // Built-in functions
    Value CallBuiltIn(const std::string& name, const std::vector<Value>& args);
    
    // Helper methods
    std::string OpCodeToString(OpCode op) const;
    void LogDebug(const std::string& msg) const;
};

} // namespace GM
