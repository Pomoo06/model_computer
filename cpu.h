#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>

class CPU
{
public:

    //
    // 内存大小
    //
    static const int MEM_SIZE = 256;

    //
    // 微周期
    //
    enum Cycle
    {
        T0,
        T1,
        T2,
        T3
    };

    //
    // 指令集
    //
    enum OpCode
    {
        OP_NOP  = 0x0,

        OP_LDI  = 0x1,

        OP_MOV  = 0x2,

        OP_ADD  = 0x3,

        OP_SUB  = 0x4,

        OP_INC  = 0x5,

        OP_DEC  = 0x6,

        OP_JMP  = 0x7,

        OP_JC   = 0x8,

        OP_ST   = 0x9,

        OP_LD   = 0xA,

        OP_HALT = 0xF
    };

    //
    // 构造
    //
    CPU();

    //
    // CPU控制
    //
    void Reset();

    void Clock();

    void Step();

    //
    // Memory
    //
    void setMem(uint16_t addr,
                uint16_t val);

    uint16_t getMem(uint16_t addr) const;

    //
    // 调试输出
    //
    std::string getStateString();

    //
    // 通用寄存器
    //
    uint16_t R[8];

    //
    // CPU寄存器
    //
    uint16_t PC;
    uint16_t IR;
    uint16_t MAR;
    uint16_t MDR;
    uint16_t BUS;

    //
    // Flags
    //
    bool z_flag;
    bool c_flag;
    bool s_flag;
    bool o_flag;

    //
    // CPU状态
    //
    bool halted;

    //
    // 当前微周期
    //
    Cycle current_cycle;

private:

    //
    // Memory
    //
    uint16_t memory[MEM_SIZE];

    //
    // ALU临时寄存器
    //
    uint16_t LA;
    uint16_t LT;

    //
    // 控制信号
    //
    bool mem_read;
    bool mem_write;
    bool reg_write;

    //
    // 当前指令
    //
    OpCode opcode;

    //
    // 指令字段
    //
    uint8_t src_reg;

    uint8_t dest_reg;

    uint16_t immediate;

    //
    // BUS
    //
    void WriteBus(uint16_t data);

    uint16_t ReadBus();

    //
    // Fetch
    //
    void Fetch_T0();

    void Fetch_T1();

    void Fetch_T2();

    //
    // CPU阶段
    //
    void Decode();

    void Execute();

    //
    // ALU
    //
    void ALU_Operation();

    //
    // Flags
    //
    void UpdateFlags(uint16_t a,
                     uint16_t b,
                     uint32_t result);
};

#endif