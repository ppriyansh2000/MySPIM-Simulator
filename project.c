// Group Members:
// Priyansh Patel, David Winfield, My Huynh
// MIPS simulator

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
  switch((int)ALUControl) // need to cast ALU control to allow switch case to work
  {
    // Z = A + B
    case 0:
      *ALUresult = A + B;
      break;

    // Z = A - B
    case 1:
      *ALUresult = A - B;
      break;

    // if A < B, Z = 1; otherwise, Z = 0
    case 2:
      *ALUresult = ((signed)A < (signed)B) ? 1 : 0;
      break;

    // if A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)
    case 3:
      *ALUresult = (A < B) ? 1 : 0;
      break;

    // Z = A AND B
    case 4:
      *ALUresult = A & B;
      break;

    // Z = A OR B
    case 5:
      *ALUresult = A | B;
      break;

    // Shift left B by 16 bits
    case 6:
      *ALUresult = B << 16;
      break;

      // Z = NOT A
    case 7:
      *ALUresult = ~A;
      break;
  }

  // zero checker
  *Zero = (*ALUresult == 0) ? 1 : 0;

  return;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
  unsigned index = PC >> 2;
  unsigned check_word = PC % 4;

   // check for word aligned
  if(check_word == 0)
  {
    *instruction = Mem[index];
  }
  else
  {
    // PC is not word aligned
    return 1;
  }

  return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
  // (instruction [31 - 26])
  *op = (instruction >> 26) & 0x3f;

  // rs (instruction [25 - 21])
  *r1 = (instruction >> 21) & 0x1f;

  // rt (instruction [20 - 16])
  *r2 = (instruction >> 16) & 0x1f;

  // rd (instruction [15 - 11])
  *r3 = (instruction >> 11) & 0x1f;

  // (instruction [5 - 0])
  *funct = instruction & 0x3f;

  // (instruction [15 - 0]
  *offset = instruction & 0xffff;

  // (instruction [25 - 0]
  *jsec = instruction & 0x3ffffff;

  return;
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
  // case numbers are op codes
  switch(op)
  {
    // R-type instructions (add, sub, and, or, slt, sltu)
    // requires funct to specify which instruction is being used
    case 0:
    controls->RegDst = 1;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 7; // need funct
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 1;
    break;

    // J-type instruction
    // jump instruction
    case 2:
    controls->RegDst = 2;
    controls->Jump = 1;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 2;
    controls->ALUOp = 0; // don't care
    controls->MemWrite = 0;
    controls->ALUSrc = 2;
    controls->RegWrite = 0;
    break;

    // I-type instructions
    // branch (Beq) instruction
    case 4:
    controls->RegDst = 2;
    controls->Jump = 0;
    controls->Branch = 1;
    controls->MemRead = 0;
    controls->MemtoReg = 2;
    controls->ALUOp = 1; // subtraction
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;
    break;

    // addi instruction
    case 8:
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0; // addition
    controls->MemWrite = 0;
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    break;

    // slti instruction
    case 10:
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 2; // signed
    controls->MemWrite = 0;
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    break;

    // sltiu instruction
    case 11:
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 3; // unsigned
    controls->MemWrite = 0;
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    break;

    // load upper imediate instruction
    case 15:
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 6; // shift left by 16 bits
    controls->MemWrite = 0;
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    break;

    // load instruction
    case 35:
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 1;
    controls->MemtoReg = 1;
    controls->ALUOp = 0; // addition
    controls->MemWrite = 0;
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
    break;

    // Store instruction
    case 43:
    controls->RegDst = 2;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 2;
    controls->ALUOp = 0; // addition
    controls->MemWrite = 1;
    controls->ALUSrc = 1;
    controls->RegWrite = 0;
    break;

    // halt
    default:
    return 1;
  }

  return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
  *data1 = Reg[r1];
  *data2 = Reg[r2];

  return;
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
  if ((offset >> 15) != 1)
  {
    // positive sign
    *extended_value = (offset & 0x0000ffff);
  }
  else
  {
    // negative sign
    *extended_value = (offset | 0xffff0000);
  }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
  //Check data on ALUSrc
  if(ALUSrc == 1)
  {
    data2 = extended_value;
  }

  // If ALUOp is 7 funct is used because its an R-type instruction
  if(ALUOp == 7)
  {
    // cases numbers are funct codes
    switch(funct) {

      // shift left extended_value by 16 bits
      case 4:
      ALUOp = 6;
      break;

      // add operation
      case 32:
      ALUOp = 0;
      break;

      // subtract operation
      case 34:
      ALUOp = 1;
      break;

      // and operation
      case 36:
      ALUOp = 4;
      break;

      // or operation
      case 37:
      ALUOp = 5;
      break;

      // not/nor operation
      case 39:
      ALUOp = 7;
      break;

      // set less than operation (signed)
      case 42:
      ALUOp = 2;
      break;

      // set less than operation (unsigned)
      case 43:
      ALUOp = 3;
      break;

      // halt
      default:
      return 1;
    }
    // funct is used (ALUOp)
    ALU(data1,data2,ALUOp,ALUresult,Zero);
  }
  else
  {
    // funct is not used (ALUOp is not an R-type)
    ALU(data1,data2,ALUOp,ALUresult,Zero);
  }

  return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
  unsigned res = ALUresult % 4;

  // reading from memory check
    if (MemRead == 1)
    {
      // check if word aligned
        if(res == 0)
        {
            *memdata = Mem[ALUresult >> 2];
        }
        // stop
        else
        {
            return 1;
        }
    }

    // write to memory check
    if (MemWrite == 1)
    {
      // check if word aligned
        if(res == 0)
        {
            Mem[ALUresult >> 2] = data2;
        }
        // stop
        else
        {
            return 1;
        }
    }

  return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
  //writting check
  if(RegWrite == 1)
  {
        // memory to r2
        if (RegDst == 0 && MemtoReg == 1)
        {
            Reg[r2] = memdata;
        }

         // memory to r3
        else if(RegDst == 1 && MemtoReg == 1)
        {
            Reg[r3] = memdata;
        }

        // ALUresult to r3
        else if (RegDst == 1 && MemtoReg == 0)
        {
            Reg[r3] = ALUresult;
        }

        // ALUresult to r2
        else if (RegDst == 0 && MemtoReg == 0)
        {
            Reg[r2] = ALUresult;
        }
    }

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
  // increments of 4
  *PC += 4;

  // if Branch and Zero = 1 -> add (extended value * 4)
  if ( Branch == 1 && Zero == 1 )
  {
    *PC += extended_value << 2;
  }

  // if Jump -> combine PC
  if ( Jump == 1 )
  {
    *PC = (jsec << 2) | (*PC & 0xf0000000);
  }

}
