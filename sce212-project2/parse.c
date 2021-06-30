/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   parse.c                                                   */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/
// 인스트럭션의 시작 부분에서 각 필드에 도달하기 까지의 비트 수
#define rs_location 6
#define rt_location 11
#define rd_location 16
#define shamt_location 21
#define funct_location 26
#define imm_location 16
#define target_location 6

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;

     // Read opcode
    char *field_for_6bits = (char*)malloc(sizeof(char) * 7); // opcode, funct 와 같은 6 비트 필드를 위한 char 배열

    int opcode_length = 6;
    strncpy(field_for_6bits, buffer, opcode_length);

    field_for_6bits[opcode_length] = '\0';

    // assign instr.opcode
    instr.opcode = fromBinary(field_for_6bits);

    // rs, rt, rd 등과 같은 5 비트 필드를 담을 배열 공간
    char *field_for_5bits = (char*)malloc(sizeof(char) * 6);

    // target address 26 비트를 담을 배열 공간
    char *field_for_target = (char*)malloc(sizeof(char) * 27);

    // immediate 16 비트를 담을 배열 공간
    char *field_for_imm = (char*)malloc(sizeof(char) * 17);

    switch(instr.opcode)
    {
        // R-format
        case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR

            // Read rs field
            strncpy(field_for_5bits, buffer + rs_location, 5);
            field_for_5bits[5] = '\0';
            instr.r_t.r_i.rs = fromBinary(field_for_5bits);

            // Read rt field
            strncpy(field_for_5bits, buffer + rt_location, 5);
            field_for_5bits[5] = '\0';
            instr.r_t.r_i.rt = fromBinary(field_for_5bits);

            // Read rd field
            strncpy(field_for_5bits, buffer + rd_location, 5);
            field_for_5bits[5] = '\0';
            instr.r_t.r_i.r_i.r.rd = fromBinary(field_for_5bits);

            // Read shamt field
            strncpy(field_for_5bits, buffer + shamt_location, 5);
            field_for_5bits[5] = '\0';
            instr.r_t.r_i.r_i.r.shamt = fromBinary(field_for_5bits);

            // Read funct field
            strncpy(field_for_6bits, buffer + funct_location, 6);
            field_for_6bits[6] = '\0';
            instr.func_code = fromBinary(field_for_6bits);

            break;

        // J-format
        case 0x2:		//(0x000010)J
        case 0x3:		//(0x000011)JAL

            // Read address field
            strncpy(field_for_target, buffer + target_location, 26);
            field_for_target[26] = '\0';
            instr.r_t.target = fromBinary(field_for_target);

            break;

        // I-format
        case 0x9:		//(0x001001)ADDIU
        case 0xc:		//(0x001100)ANDI
        case 0xf:		//(0x001111)LUI
        case 0xd:		//(0x001101)ORI
        case 0xb:		//(0x001011)SLTIU
        case 0x23:		//(0x100011)LW
        case 0x2b:		//(0x101011)SW
        case 0x4:		//(0x000100)BEQ
        case 0x5:		//(0x000101)BNE

            // Read rs field
            strncpy(field_for_5bits, buffer + rs_location, 5);
            field_for_5bits[5] = '\0';
            instr.r_t.r_i.rs = fromBinary(field_for_5bits);

            // Read rt field
            strncpy(field_for_5bits, buffer + rt_location, 5);
            field_for_5bits[5] = '\0';
            instr.r_t.r_i.rt = fromBinary(field_for_5bits);

            // Read immediate field
            strncpy(field_for_imm, buffer + imm_location, 16);
            field_for_imm[16] = '\0';
            instr.r_t.r_i.r_i.imm = fromBinary(field_for_imm);

            break;

        default:
            printf("Not available instruction\n");
            assert(0);
    }

    char tmp_buffer[33];
    memcpy(tmp_buffer, buffer, 33);

    // Write the instruction in buffer into simulated memory
    mem_write_32(MEM_TEXT_START + index, fromBinary(tmp_buffer));

    // Deallocate memory
    free(field_for_5bits);
    free(field_for_6bits);
    free(field_for_target);
    free(field_for_imm);

    // 인스트럭션의 32 비트 바이너리 값
    instr.value = fromBinary(tmp_buffer);

    return instr;
}

void parsing_data(const char *buffer, const int index)
{
    char tmp_buffer[33];
    memcpy(tmp_buffer, buffer, 33);

    // Write the data in buffer into simulated memory
    mem_write_32(MEM_DATA_START + index, fromBinary(tmp_buffer));
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	    switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:		//(0x001001)ADDIU
            case 0xc:		//(0x001100)ANDI
            case 0xf:		//(0x001111)LUI	
            case 0xd:		//(0x001101)ORI
            case 0xb:		//(0x001011)SLTIU
            case 0x23:		//(0x100011)LW
            case 0x2b:		//(0x101011)SW
            case 0x4:		//(0x000100)BEQ
            case 0x5:		//(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:		//(0x000010)J
            case 0x3:		//(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
