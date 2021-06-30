/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction()
{
    /* example2, 3 그리고 leaf 테스트 케이스에서 input file에 존재하는 인스트럭션의 수 만큼 수행하고 난 뒤에도 계속 수행돼서 PC 값이 이상한 곳을 가리키는 것이 문제
     * PC가 범위를 넘어선 메모리 주소를 가리키는 것을 RUN_BIT 를 끔으로써 방지 */
    if((CURRENT_STATE.PC - MEM_TEXT_START) >= (NUM_INST << 2))
    {
        RUN_BIT = FALSE;
    }

    int i;

    // Current PC가 가리키는 인스트럭션의 값을 가져온다.
    uint32_t value_of_current_pc = mem_read_32(CURRENT_STATE.PC);

    // 현재 PC가 가리키는 인스트럭션의 값과 INST_INFO에 존재하는 인스트럭션의 값이 일치할 때까지 반복
    for (i = 0; i < NUM_INST; i++)
    {
        if (value_of_current_pc == INST_INFO[i].value)
        {
            CURRENT_STATE.PC += BYTES_PER_WORD;
            break;
        }
    }

     // opcode를 통해서 포맷 구분
    switch(INST_INFO[i].opcode)
    {
        // R-format
        case 0x0:
            // funct 로 인스트럭션 구분
            switch(INST_INFO[i].func_code)
            {
                case 0x0:   //(0x000000)SLL
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] << INST_INFO[i].r_t.r_i.r_i.r.shamt;
                    break;
                case 0x2:   //(0x000010)SRL
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] >> INST_INFO[i].r_t.r_i.r_i.r.shamt;
                    break;
                case 0x08:  //(0x001000)JR
                    CURRENT_STATE.PC = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs];
                    break;
                case 0x21:  //(0x100001)ADDU
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] + CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];
                    break;
                case 0x23:  //(0x100011)SUBU
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] - CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];
                    break;
                case 0x24:  //(0x100100)AND
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] & CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];
                    break;
                case 0x25:  //(0x100101)OR
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] | CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt];
                    break;
                case 0x27:  //(0x100111)NOR
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = ~(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] | CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]);
                    break;
                case 0x2b:  //(0x101011)SLTU
                    CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.r_i.r.rd] = (CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] < CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]) ? 1 : 0;
                    break;
                default:
                    break;
            }
            break;

        // J-format
        case 0x2:		//(0x000010)J
            CURRENT_STATE.PC = INST_INFO[i].r_t.target << 2;
            break;
        case 0x3:		//(0x000011)JAL
            CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + BYTES_PER_WORD;  // $ra
            CURRENT_STATE.PC = INST_INFO[i].r_t.target << 2;
            break;

        // I-format
        case 0x9:		//(0x001001)ADDIU
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] + INST_INFO[i].r_t.r_i.r_i.imm;
            break;
        case 0xc:		//(0x001100)ANDI
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] & INST_INFO[i].r_t.r_i.r_i.imm;
            break;
        case 0xf:		//(0x001111)LUI
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = INST_INFO[i].r_t.r_i.r_i.imm << 16;
            break;
        case 0xd:		//(0x001101)ORI
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] | INST_INFO[i].r_t.r_i.r_i.imm;
            break;
        case 0xb:		//(0x001011)SLTIU
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = (CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] < INST_INFO[i].r_t.r_i.r_i.imm) ? 1 : 0;
            break;
        case 0x23:		//(0x100011)LW
            CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt] = mem_read_32(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] + INST_INFO[i].r_t.r_i.r_i.imm);
            break;
        case 0x2b:		//(0x101011)SW
            mem_write_32(CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] + INST_INFO[i].r_t.r_i.r_i.imm, CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt]);
            break;
        case 0x4:		//(0x000100)BEQ
            if (CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] == CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt])
            {
                CURRENT_STATE.PC += (INST_INFO[i].r_t.r_i.r_i.imm << 2);
            }
            break;
        case 0x5:		//(0x000101)BNE
            if (CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rs] != CURRENT_STATE.REGS[INST_INFO[i].r_t.r_i.rt])
            {
                CURRENT_STATE.PC += (INST_INFO[i].r_t.r_i.r_i.imm << 2);
            }
            break;
        default:
            break;
    }
}
