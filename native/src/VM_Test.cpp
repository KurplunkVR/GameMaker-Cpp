#include <iostream>
#include "../include/VM_Executor.h"

int main() {
    GM::VirtualMachine vm;
    vm.SetDebugOutput(true);

    // Create a simple test: push 5, push 3, add, return
    GM::CodeBlock testAdd;
    testAdd.name = "TestAdd";
    testAdd.id = 1;
    testAdd.instructions = {
        { GM::OpCode::PUSHI, GM::Value(5.0), GM::Value(), "" },
        { GM::OpCode::PUSHI, GM::Value(3.0), GM::Value(), "" },
        { GM::OpCode::ADD, GM::Value(), GM::Value(), "" },
        { GM::OpCode::RET, GM::Value(), GM::Value(), "" }
    };

    vm.AddCodeBlock(testAdd);
    
    // Execute and check result
    GM::Value result = vm.ExecuteFunction("TestAdd");
    std::cout << "Result of 5 + 3 = " << result.AsReal() << std::endl;
    
    if (result.AsReal() == 8.0) {
        std::cout << "SUCCESS: VM arithmetic test passed!" << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Expected 8.0, got " << result.AsReal() << std::endl;
        return 1;
    }
}
