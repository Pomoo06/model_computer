#include "cpu.h"
#include <sstream>
#include <iomanip>

CPU::CPU() {
    Reset();
}

void CPU::Reset() {
    PC = 0;
    IR = 0;
    MAR = 0;
    MDR = 0;
    BUS = 0;

    LA = 0;
    LT = 0;

    halted = false;

    // 微周期
    current_cycle = T0;

    // 通用寄存器
    for (int i = 0; i < 8; i++)
        R[i] = 0;

    // 标志位
    z_flag = false;
    c_flag = false;
    s_flag = false;
    o_flag = false;

    // 控制信号
    mem_read = false;
    mem_write = false;
    reg_write = false;

    opcode = OP_NOP;
    src_reg = 0;
    dest_reg = 0;
    immediate = 0;

    // 内存清零
    for (int i = 0; i < MEM_SIZE; i++)
        memory[i] = 0;
}

void CPU::setMem(uint16_t addr, uint16_t val) {
    if (addr < MEM_SIZE)
        memory[addr] = val;
}

uint16_t CPU::getMem(uint16_t addr) const {
    if (addr < MEM_SIZE)
        return memory[addr];

    return 0;
}

void CPU::WriteBus(uint16_t data) {
    BUS = data;
}

uint16_t CPU::ReadBus() {
    return BUS;
}

//
// ==================== 取指 ====================
//

void CPU::Fetch_T0() {
    // MAR <- PC
    WriteBus(PC);
    MAR = ReadBus();
}

void CPU::Fetch_T1() {
    // MDR <- MEM[MAR]
    mem_read = true;

    MDR = memory[MAR];

    // PC++
    PC++;

    mem_read = false;
}

void CPU::Fetch_T2() {
    // IR <- MDR
    WriteBus(MDR);
    IR = ReadBus();
}

//
// ==================== 译码 ====================
//

void CPU::Decode() {
    opcode = static_cast<OpCode>((IR >> 12) & 0x0F);

    dest_reg = (IR >> 9) & 0x07;

    src_reg = (IR >> 6) & 0x07;

    // 改成 8 位立即数
    immediate = IR & 0x00FF;

    reg_write = false;
    mem_write = false;
}

//
// ==================== 标志位 ====================
//

void CPU::UpdateFlags(uint16_t a, uint16_t b, uint32_t result) {

    uint16_t res16 = (uint16_t)result;

    // Zero
    z_flag = (res16 == 0);

    // Carry
    c_flag = (result > 0xFFFF);

    // Sign
    s_flag = ((res16 & 0x8000) != 0);

    // Overflow
    switch (opcode) {

    case OP_ADD:
        o_flag =
            (((a ^ res16) & (b ^ res16)) & 0x8000) != 0;
        break;

    case OP_SUB:
        o_flag =
            (((a ^ b) & (a ^ res16)) & 0x8000) != 0;
        break;

    case OP_INC:
        o_flag = (a == 0x7FFF);
        break;

    case OP_DEC:
        o_flag = (a == 0x8000);
        break;

    default:
        o_flag = false;
    }
}

//
// ==================== ALU ====================
//

void CPU::ALU_Operation() {

    uint32_t result = 0;

    LA = R[dest_reg];
    LT = R[src_reg];

    switch (opcode) {

    case OP_ADD:
        result = (uint32_t)LA + (uint32_t)LT;
        break;

    case OP_SUB:
        result = (uint32_t)LA - (uint32_t)LT;
        break;

    case OP_INC:
        result = (uint32_t)LA + 1;
        break;

    case OP_DEC:
        result = (uint32_t)LA - 1;
        break;

    default:
        result = LA;
        break;
    }

    // ALU 输出到总线
    WriteBus((uint16_t)result);

    if (reg_write) {

        // BUS -> 寄存器
        R[dest_reg] = ReadBus();

        UpdateFlags(LA, LT, result);
    }
}

//
// ==================== 执行 ====================
//

void CPU::Execute() {

    switch (opcode) {

    case OP_NOP:
        break;

    //
    // LDI
    //
    case OP_LDI:

        WriteBus(immediate);

        reg_write = true;

        if (reg_write)
            R[dest_reg] = ReadBus();

        break;

    //
    // MOV
    //
    case OP_MOV:

        WriteBus(R[src_reg]);

        reg_write = true;

        if (reg_write)
            R[dest_reg] = ReadBus();

        break;

    //
    // ALU
    //
    case OP_ADD:
    case OP_SUB:
    case OP_INC:
    case OP_DEC:

        reg_write = true;

        ALU_Operation();

        break;

    //
    // JMP
    //
    case OP_JMP:

        PC = immediate;

        break;

    //
    // JC
    //
    case OP_JC:

        if (c_flag)
            PC = immediate;

        break;

    //
    // ST
    //
    case OP_ST:

        // MAR <- imm
        WriteBus(immediate);
        MAR = ReadBus();

        // MDR <- R[dest]
        WriteBus(R[dest_reg]);
        MDR = ReadBus();

        mem_write = true;

        memory[MAR] = MDR;

        mem_write = false;

        break;

    //
    // LD
    //
    case OP_LD:

        // MAR <- imm
        WriteBus(immediate);
        MAR = ReadBus();

        mem_read = true;

        MDR = memory[MAR];

        mem_read = false;

        // MDR -> BUS
        WriteBus(MDR);

        reg_write = true;

        if (reg_write)
            R[dest_reg] = ReadBus();

        break;

    //
    // HALT
    //
    case OP_HALT:

        halted = true;

        break;
    }
}

//
// ==================== 时钟 ====================
//

void CPU::Clock() {

    if (halted)
        return;

    switch (current_cycle) {

    case T0:

        Fetch_T0();

        current_cycle = T1;

        break;

    case T1:

        Fetch_T1();

        current_cycle = T2;

        break;

    case T2:

        Fetch_T2();

        current_cycle = T3;

        break;

    case T3:

        Decode();

        Execute();

        current_cycle = T0;

        break;
    }
}

//
// ==================== 单步 ====================
//

void CPU::Step() {

    if (halted)
        return;

    do {

        Clock();

    } while (current_cycle != T0 && !halted);
}

//
// ==================== 状态输出 ====================
//

std::string CPU::getStateString() {

    std::stringstream ss;

    ss << "PC:0x"
       << std::hex
       << std::setw(4)
       << std::setfill('0')
       << PC;

    ss << " IR:0x"
       << std::setw(4)
       << IR;

    ss << " MAR:0x"
       << std::setw(4)
       << MAR;

    ss << " MDR:0x"
       << std::setw(4)
       << MDR;

    ss << " BUS:0x"
       << std::setw(4)
       << BUS;

    ss << " ";

    for (int i = 0; i < 8; i++) {

        ss << "R" << i << ":0x"
           << std::setw(4)
           << R[i]
           << " ";
    }

    ss << "Z:" << z_flag
       << " C:" << c_flag
       << " S:" << s_flag
       << " O:" << o_flag;

    return ss.str();
}