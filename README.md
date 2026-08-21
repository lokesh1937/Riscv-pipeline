# RISC-V Processor Pipeline Simulator

A comprehensive educational simulator for a 5-stage pipelined RISC-V processor implemented in C++. This project demonstrates how modern CPUs execute instructions with pipelining, hazard handling, and control flow management.

## 📋 Project Overview

This project consists of two main components:

1. **RISC-V Assembler** - Converts assembly code to 32-bit machine code
2. **Pipeline Simulator** - Executes the machine code through a 5-stage pipeline with hazard detection and forwarding

## 🏗️ Architecture

### Pipeline Stages

The processor implements a classic 5-stage pipeline:

```
Instruction Flow: IF → ID → EX → MEM → WB
```

| Stage | Function |
|-------|----------|
| **IF (Instruction Fetch)** | Retrieves instruction from instruction memory at program counter |
| **ID (Instruction Decode)** | Decodes opcode, extracts register indices and immediates, generates control signals |
| **EX (Execute)** | Performs ALU operations, computes addresses, evaluates branch conditions |
| **MEM (Memory)** | Performs load/store operations from/to data memory |
| **WB (Write-back)** | Writes results back to the register file |

### Key Features

- ✅ **Data Forwarding**: Resolves Read-After-Write (RAW) hazards
- ✅ **Pipeline Flushing**: Handles branch/jump control flow changes
- ✅ **Complete RISC-V ISA Support**: All major instruction types
- ✅ **Register File Simulation**: 32 general-purpose registers (x0-x31)
- ✅ **Data Memory**: Simulated memory for load/store operations
- ✅ **Detailed Logging**: Cycle-by-cycle execution traces

## 📦 Supported Instruction Types

### R-Type (Register-Register Operations)
- Arithmetic: `add`, `sub`
- Logical: `and`, `or`, `xor`
- Shift: `sll`, `srl`, `sra`
- Comparison: `slt`, `sltu`

### R-M Type (Multiply/Divide Extension)
- `mul`, `mulh`, `mulsu`, `mulu`
- `div`, `divu`, `rem`, `remu`

### I-Type (Immediate Operations)
- Arithmetic: `addi`, `andi`, `ori`, `xori`
- Shift: `slli`, `srli`, `srai`
- Comparison: `slti`, `sltiu`
- Load: `lw`, `lb`, `lh`, `lbu`, `lhu`
- Jump: `jalr`

### S-Type (Store Operations)
- `sw` (Store Word), `sb` (Store Byte), `sh` (Store Halfword)

### B-Type (Branch Operations)
- `beq`, `bne`, `blt`, `bge`, `bltu`, `bgeu`

### J-Type (Jump Operations)
- `jal` (Jump and Link)

## 🚀 Getting Started

### Prerequisites
- C++ compiler (C++11 or later)
- Linux/Unix environment (or Windows with WSL/MinGW)

### Compilation

```bash
# Compile assembler
g++ -o assembler assembler.cpp

# Compile pipeline simulator
g++ -o pipeline pipeline.cpp
```

### Usage

#### Step 1: Prepare Input Assembly Code
Create an `input.txt` file with RISC-V assembly instructions:

```assembly
addi a5,x0,5
sw a5,-24(s0)
addi a5,x0,1
sw a5,-28(s0)
addi a5,x0,2
sw a5,-20(s0)
jal x1,16
```

#### Step 2: Run the Assembler
```bash
./assembler
```

This generates `output.txt` containing 32-bit binary machine code.

#### Step 3: Run the Pipeline Simulator
```bash
./pipeline
```

The simulator will:
- Load instructions from `output.txt`
- Execute them through the 5-stage pipeline
- Print cycle-by-cycle execution traces
- Output final register and memory states

## 📊 Example: Prime Number Checker

The included test case checks if 5 is prime:

```assembly
addi a5,x0,5        # Load 5 into a5
sw a5,-24(s0)       # Store in memory
addi a5,x0,1        # Initialize result to 1
sw a5,-28(s0)       # Store result
addi a5,x0,2        # Start divisor at 2
sw a5,-20(s0)       # Store divisor
jal x1,16           # Jump ahead
lw a4,-24(s0)       # Load n
lw a5,-20(s0)       # Load divisor
rem a5,a4,a5        # Compute remainder
bne a5,x0,13        # Branch if not equal (prime)
sw x0,-28(s0)       # Mark as not prime
lw a5,-20(s0)       # Load divisor
addi a5,a5,1        # Increment divisor
sw a5,-20(s0)       # Store back
lw a4,-20(s0)       # Load divisor
lw a5,-24(s0)       # Load n
blt a4,a5,8         # Branch if divisor < n
addi a5,x0,0        # Clear a5
```

**Output**: Stored result indicates whether 5 is prime

## 🔍 Understanding the Output

### Assembler Output (output.txt)
32-bit binary representations of each instruction:
```
00000000010100000000011110010011  # addi a5,x0,5
11111110111101000010001000100011  # sw a5,-24(s0)
...
```

### Pipeline Simulator Output (Console)
Detailed cycle-by-cycle execution:
```
--cycle 0---
[IF] Fetched: 00000000010100000000011110010011
--cycle 1---
[ID] opcode=0010011 rs1=0 rs2=0 rd=15 imm=5
[IF] Fetched: 11111110111101000010001000100011
--cycle 2---
[EX] ALU=ADD result=5
[ID] opcode=0100011 rs1=8 rs2=15 rd=0 imm=-24
...
Final result(which stored in memory): <value>
```

## 📁 File Structure

```
lokesh1937/Riscv-pipeline/
├── README.md              # This file
├── assembler.cpp          # RISC-V assembler implementation
├── pipeline.cpp           # 5-stage pipeline simulator
├── input.txt             # Sample RISC-V assembly code
└── output.txt            # Generated machine code
```

## 🎓 Educational Value

This project is ideal for learning:
- **Computer Architecture**: Understanding pipelined processor design
- **RISC-V ISA**: Hands-on experience with modern instruction set
- **Hazard Resolution**: Data forwarding and pipeline flushing techniques
- **Digital Logic**: How CPUs execute instructions at the microarchitectural level

## 🔧 Register Naming Convention

The simulator supports both numeric (x0-x31) and ABI names:

| ABI Name | Register | Purpose |
|----------|----------|---------|
| `zero` | x0 | Always zero |
| `ra` | x1 | Return address |
| `sp` | x2 | Stack pointer |
| `gp` | x3 | Global pointer |
| `tp` | x4 | Thread pointer |
| `t0-t6` | x5-x7, x28-x31 | Temporary |
| `s0-s11` | x8-x9, x18-x27 | Saved |
| `a0-a7` | x10-x17 | Arguments/Return values |

## ⚠️ Known Limitations

- Single-cycle ALU operations (no multi-cycle support)
- Simplified memory model (byte-addressed linear array)
- No exception handling
- No privilege levels (M-mode only)
- Limited floating-point support

## 🤝 Future Enhancements

- [ ] Tomasulo's algorithm for out-of-order execution
- [ ] Branch prediction
- [ ] Cache simulation (L1, L2)
- [ ] Multi-cycle operations
- [ ] Interrupt handling
- [ ] Debug visualization tools

## 📝 License

This project is open source and available for educational purposes.

## 👨‍💻 Author

Created as an educational project for computer architecture learning.

---

**Happy Learning! 🎉**

For questions or improvements, feel free to open an issue or pull request.
