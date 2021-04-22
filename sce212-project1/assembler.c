#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

/*
 * For debug option. If you want to debug, set 1.
 * If not, set 0.
 */
#define DEBUG 0

#define MAX_SYMBOL_TABLE_SIZE   1024
#define MEM_TEXT_START          0x00400000
#define MEM_DATA_START          0x10000000
#define BYTES_PER_WORD          4
#define INST_LIST_LEN           20

/******************************************************
 * Structure Declaration
 *******************************************************/

typedef struct inst_struct {
    char *name;
    char *op;
    char type;
    char *funct;
} inst_t;

typedef struct symbol_struct {
    char name[32];
    uint32_t address;
} symbol_t;

enum section { 
    DATA = 0,
    TEXT,
    MAX_SIZE
};

/******************************************************
 * Global Variable Declaration
 *******************************************************/

inst_t inst_list[INST_LIST_LEN] = {       //  idx
    {"addiu",   "001001", 'I', ""},       //    0
    {"addu",    "000000", 'R', "100001"}, //    1
    {"and",     "000000", 'R', "100100"}, //    2
    {"andi",    "001100", 'I', ""},       //    3
    {"beq",     "000100", 'I', ""},       //    4
    {"bne",     "000101", 'I', ""},       //    5
    {"j",       "000010", 'J', ""},       //    6
    {"jal",     "000011", 'J', ""},       //    7
    {"jr",      "000000", 'R', "001000"}, //    8
    {"lui",     "001111", 'I', ""},       //    9
    {"lw",      "100011", 'I', ""},       //   10
    {"nor",     "000000", 'R', "100111"}, //   11
    {"or",      "000000", 'R', "100101"}, //   12
    {"ori",     "001101", 'I', ""},       //   13
    {"sltiu",   "001011", 'I', ""},       //   14
    {"sltu",    "000000", 'R', "101011"}, //   15
    {"sll",     "000000", 'R', "000000"}, //   16
    {"srl",     "000000", 'R', "000010"}, //   17
    {"sw",      "101011", 'I', ""},       //   18
    {"subu",    "000000", 'R', "100011"}  //   19
};

symbol_t SYMBOL_TABLE[MAX_SYMBOL_TABLE_SIZE]; // Global Symbol Table

uint32_t symbol_table_cur_index = 0; // For indexing of symbol table

/* Temporary file stream pointers */
FILE *data_seg;
FILE *text_seg;

/* Size of each section */
uint32_t data_section_size = 0;
uint32_t text_section_size = 0;

/******************************************************
 * Function Declaration
 *******************************************************/

/* Change file extension from ".s" to ".o" */
char* change_file_ext(char *str) {
    char *dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0))
        return NULL;

    str[strlen(str) - 1] = 'o';
    return "";
}

/* Add symbol to global symbol table */
void symbol_table_add_entry(symbol_t symbol)
{
    SYMBOL_TABLE[symbol_table_cur_index++] = symbol;
#if DEBUG
    printf("%s: 0x%08x\n", symbol.name, symbol.address);
#endif
}

/* Convert integer number to binary string */
char* num_to_bits(unsigned int num, int len) 
{
    char* bits = (char *) malloc(len+1);
    int idx = len-1, i;
    while (num > 0 && idx >= 0) {
        if (num % 2 == 1) {
            bits[idx--] = '1';
        } else {
            bits[idx--] = '0';
        }
        num /= 2;
    }
    for (i = idx; i >= 0; i--){
        bits[i] = '0';
    }
    bits[len] = '\0';
    return bits;
}

/* Record .text section to output file */
void record_text_section(FILE *output) 
{
    uint32_t cur_addr = MEM_TEXT_START;
    char line[1024];

    /* Point to text_seg stream */
    rewind(text_seg);

    /* Print .text section */
    while (fgets(line, 1024, text_seg) != NULL) {
        char *rs_bits, *rt_bits, *rd_bits, *shamt_bits, *imm_bits, *addr_bits;
        char op[32] = {0};
        char type = '0';
        int idx = 0;
        int rs, rt, rd, imm, shamt;
        int addr;

        rs = rt = rd = imm = shamt = addr = 0;
#if DEBUG
        printf("0x%08x: ", cur_addr);
#endif
        /* Find the instruction type that matches the line */
        /* blank */
        char* token = strtok(line, " \t");
        for (; idx < INST_LIST_LEN; ++idx) {
            if (!strcmp(token, inst_list[idx].name)) break;
        }

        strcpy(op, inst_list[idx].op);
        type = inst_list[idx].type;

        switch (type) {
            case 'R':
                /* blank */
                if (!strcmp("sll", inst_list[idx].name) || !strcmp("srl", inst_list[idx].name)) {
                    token = strtok(NULL, ", ");
                    rd = atoi(token + 1);
                    token = strtok(NULL, ", ");
                    rt = atoi(token + 1);
                    token = strtok(NULL, " \n");
                    shamt = atoi(token);
                    rs = 0;

                    rd_bits = num_to_bits(rd, 5);
                    rt_bits = num_to_bits(rt, 5);
                    shamt_bits = num_to_bits(shamt, 5);
                    rs_bits = num_to_bits(rs, 5);
                }
                else if (!strcmp("jr", inst_list[idx].name)) {
                    token = strtok(NULL, " \n");
                    rs = atoi(token + 1);
                    rd = 0;
                    rt = 0;
                    shamt = 0;

                    rd_bits = num_to_bits(rd, 5);
                    rs_bits = num_to_bits(rs, 5);
                    rt_bits = num_to_bits(rt, 5);
                    shamt_bits = num_to_bits(shamt, 5);
                }
                else {
                    token = strtok(NULL, ", ");
                    rd = atoi(token + 1);
                    token = strtok(NULL, ", ");
                    rs = atoi(token + 1);
                    token = strtok(NULL, " \n");
                    rt = atoi(token + 1);
                    shamt = 0;

                    rd_bits = num_to_bits(rd, 5);
                    rs_bits = num_to_bits(rs, 5);
                    rt_bits = num_to_bits(rt, 5);
                    shamt_bits = num_to_bits(shamt, 5);

                }
                fputs(op, output);
                fputs(rs_bits, output);
                fputs(rt_bits, output);
                fputs(rd_bits, output);
                fputs(shamt_bits, output);
                fputs(inst_list[idx].funct, output);

#if DEBUG
                printf("op:%s rs:$%d rt:$%d rd:$%d shamt:%d funct:%s\n",
                        op, rs, rt, rd, shamt, inst_list[idx].funct);
#endif
                break;

            case 'I':
                /* blank */
                if (!strcmp("lui", inst_list[idx].name)) {
                    token = strtok(NULL, " ,");
                    rt = atoi(token + 1);
                    token = strtok(NULL, " \n");
                    if (strchr(token, 'x')) {
                        imm = strtol(token, NULL, 16);
                    } else {
                        imm = atoi(token);
                    }
                    rs = 0;

                    rs_bits = num_to_bits(rs, 5);
                    rt_bits = num_to_bits(rt, 5);
                    imm_bits = num_to_bits(imm, 16);
                }
                else if (!strcmp("beq", inst_list[idx].name) || !strcmp("bne", inst_list[idx].name)) {
                    token = strtok(NULL, " ,");
                    rs = atoi(token + 1);
                    token = strtok(NULL, " ,");
                    rt = atoi(token + 1);
                    token = strtok(NULL, " \n");

                    int i = 0;
                    while (strcmp(token, SYMBOL_TABLE[i].name)) {
                        i++;
                    }

                    imm = SYMBOL_TABLE[i].address;
                    imm = (imm - cur_addr)/4 - 1;

                    rs_bits = num_to_bits(rs, 5);
                    rt_bits = num_to_bits(rt, 5);
                    imm_bits = num_to_bits(imm, 16);

                }
                else if (!strcmp("lw", inst_list[idx].name) || !strcmp("sw", inst_list[idx].name)) {
                    token = strtok(NULL, " ,");
                    rt = atoi(token + 1);
                    token = strtok(NULL, " (");
                    imm = atoi(token);
                    token = strtok(NULL, " )");
                    rs = atoi(token + 1);

                    rs_bits = num_to_bits(rs, 5);
                    rt_bits = num_to_bits(rt, 5);
                    imm_bits = num_to_bits(imm, 16);
                }
                else {
                    token = strtok(NULL, " ,");
                    rt = atoi(token + 1);
                    token = strtok(NULL, " ,");
                    rs = atoi(token + 1);
                    token = strtok(NULL, " \n");
                    if (strchr(token, 'x')) {
                        imm = strtol(token, NULL, 16);
                    } else {
                        imm = atoi(token);
                    }

                    rs_bits = num_to_bits(rs, 5);
                    rt_bits = num_to_bits(rt, 5);
                    imm_bits = num_to_bits(imm, 16);
                }

                fputs(op, output);
                fputs(rs_bits, output);
                fputs(rt_bits, output);
                fputs(imm_bits, output);
#if DEBUG
                printf("op:%s rs:$%d rt:$%d imm:0x%x\n",
                        op, rs, rt, imm);
#endif
                break;

            case 'J':
                /* blank */
                symbol_table_cur_index = 0;

                token = strtok(NULL, " \n");

                while (strcmp(token, SYMBOL_TABLE[symbol_table_cur_index].name)) {
                    symbol_table_cur_index++;
                }

                addr = SYMBOL_TABLE[symbol_table_cur_index].address / 4;
                addr_bits = num_to_bits(addr, 26);

                fputs(op, output);
                fputs(addr_bits, output);
#if DEBUG
                printf("op:%s addr:%i\n", op, addr);
#endif
                break;

            default:
                break;
        }
        fprintf(output, "\n");

        cur_addr += BYTES_PER_WORD;
    }
}

/* Record .data section to output file */
void record_data_section(FILE *output)
{
    uint32_t cur_addr = MEM_DATA_START;
    char line[1024];
    char *data;
    int decimal = 0;

    /* Point to data segment stream */
    rewind(data_seg);

    /* Print .data section */
    while (fgets(line, 1024, data_seg) != NULL) {
        /* blank */
        char _line[1024] = {0};
        strcpy(_line, line);
        data = strtok(line, " \t");
        data = strtok(NULL, " \n");

        // strtol 함수는 특정 진법으로 표기된 문자열을 long 타입으로 변환하는 함수이다.
        if (strchr(data, 'x')) decimal = strtol(data, NULL, 16);
        else decimal = strtol(data, NULL, 10);

        fputs(num_to_bits(decimal,32), output);
        fputc(10, output);

#if DEBUG
        printf("0x%08x: ", cur_addr);
        printf("%s", _line);
#endif
        cur_addr += BYTES_PER_WORD;
    }
}

/* Fill the blanks */
void make_binary_file(FILE *output)
{
#if DEBUG
    char line[1024] = {0};
    rewind(text_seg);
    /* Print line of text segment */
    while (fgets(line, 1024, text_seg) != NULL) {
        printf("%s",line);
    }
    printf("text section size: %d, data section size: %d\n",
            text_section_size, data_section_size);
#endif

    /* Print text section size and data section size */
    /* blank */
    fputs(num_to_bits(text_section_size, 32), output);
    fputc(10, output);
    fputs(num_to_bits(data_section_size, 32), output);
    fputc(10, output);

    /* Print .text section */
    record_text_section(output);

    /* Print .data section */
    record_data_section(output);
}

/* Fill the blanks */
void make_symbol_table(FILE *input)
{
    char line[1024] = {0};
    uint32_t address = 0;
    symbol_t label;
    enum section cur_section = MAX_SIZE; // MAX_SIZE = 2

    /* Read each section and put the stream */
    // fgets 함수는 스트림에서 문자열을 받아서 (num - 1) 개의 문자를 입력 받을 때 혹은 개행 문자나 EOF 에 도달할 때 까지 입력 받아서 C 형식 문자열로 저장
    // 개행 문자까지 문자열에 저장하고 NULL 문자는 자동적으로 마지막으로 입력받은 문자 뒤에 포함
    while (fgets(line, 1024, input) != NULL) {
        char *temp;
        char _line[1024] = {0};
        strcpy(_line, line);
        temp = strtok(_line, "\t\n");

        /* Check section type */
        if (!strcmp(temp, ".data")) {
            data_seg = tmpfile();

            if (data_seg == NULL) {
                perror("ERROR");
                exit(EXIT_FAILURE);
            }

            cur_section = DATA;
            address = MEM_DATA_START;
            continue;
        }
        else if (!strcmp(temp, ".text")) {
            text_seg = tmpfile();

            if (text_seg == NULL) {
                perror("ERROR");
                exit(EXIT_FAILURE);
            }

            cur_section = TEXT;
            address = MEM_TEXT_START;
            continue;
        }

        /* Put the line into each segment stream */
        if (cur_section == DATA) {
            if (strchr(temp, ':')) {
                char temp_label[32] = {0};
                strncpy(temp_label, temp, strlen(temp)-1);

                strcpy(label.name, temp_label);
                label.address = address;
                symbol_table_add_entry(label);
            }

            temp = strstr(line, ".");
            fputs(temp, data_seg); // 개행문자까지 포함해서 넣음
            data_section_size += BYTES_PER_WORD;
        }
        else if (cur_section == TEXT) {
            if (strchr(temp, ':')) {
                char temp_label[32] = {0};
                strncpy(temp_label, temp, strlen(temp)-1);

                strcpy(label.name, temp_label);
                label.address = address;
                symbol_table_add_entry(label);
                continue;
            } 
            if (!strcmp(temp, "la")) {
                char rgtr[32] = {0};
                temp = strtok(NULL, " ,");
                /** printf("temp: %s\n", temp); */
                strcpy(rgtr, temp);
                fputs("\tlui\t", text_seg);
                fputs(rgtr, text_seg);
                /** printf("register success\n"); */
                temp = strtok(NULL, " \n");
                /** printf("VAR temp: %s\n", temp); */
                int i = 0;
                while (strcmp(temp, SYMBOL_TABLE[i].name)) {
                    /** printf("different temp: %s, ST.name: %s\n",temp, SYMBOL_TABLE[i].name); */
                    i++;
                }
                if (SYMBOL_TABLE[i].address == MEM_DATA_START) {
                    char addr[32];
                    strcpy(addr, "0x1000\n");
                    fputs(", ", text_seg);
                    fputs(addr, text_seg);
                    text_section_size += BYTES_PER_WORD;
                }
                else {
                    char addr[32];
                    strcpy(addr, "0x1000\n");
                    fputs(", ", text_seg);
                    fputs(addr, text_seg);
                    address += BYTES_PER_WORD;
                    text_section_size += BYTES_PER_WORD;

                    fputs("\tori\t", text_seg);
                    fputs(rgtr, text_seg);
                    fputs(", ", text_seg);
                    fputs(rgtr, text_seg);
                    fputs(", ", text_seg);
                    int diff = SYMBOL_TABLE[i].address - MEM_DATA_START;
                    if (diff == 12) {
                        fputs("0x000c\n", text_seg);
                    }
                    else if (diff == 8){
                        fputs("0x0008\n", text_seg);
                    }
                    else{
                        fputs("0x0004\n", text_seg);
                    }
                    text_section_size += BYTES_PER_WORD;
                }
            }
            else {
                fputs(line, text_seg);
                text_section_size += BYTES_PER_WORD;
            }
        }
        address += BYTES_PER_WORD;
    }
}

/******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/

int main(int argc, char* argv[])
{
    FILE *input, *output;
    char *filename;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Read the input file */
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    /* Create the output file (*.o) */
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if(change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }

    output = fopen(filename, "w");
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    /******************************************************
     *  Let's complete the below functions!
     *
     *  make_symbol_table(FILE *input)
     *  make_binary_file(FILE *output)
     *  ├── record_text_section(FILE *output)
     *  └── record_data_section(FILE *output)
     *
     *******************************************************/
    make_symbol_table(input);
    make_binary_file(output);

    fclose(input);
    fclose(output);

    return 0;
}
