# Computer Architecture Assignments

과목코드 | 개설전공 | 교수 | 수강시기 |
--------|---------|--------|---------|
SCE212 | 소프트웨어 | 안정섭 | 2021년 3학년 1학기 |
* * *

- [x] sce212-project0
- [ ] sce212-project1
- [ ] sce212-project2
- [ ] sce212-project3
* * *

## sce212-project0
- 컴퓨터구조 과제 업로드 레포지토리이다.
- 포인터, 구조체 등과 같은 C 언어의 여러 개념이 부족하여, [모두의코드](https://modoocode.com/)에서 공부하면서 과제를 수행 했다. 지금까지 본 C 언어 개념 설명 중 단연 최고라고 생각한다.
- 리눅스 원격 서버에 있는 디렉토리를 scp 명령어를 통해 가져오고 싶을 때, -r 옵션을 줘야한다. -r 은 __recursive__ 를 의미한다. scp -r [user@]host:[source_path] [target_path]와 같은 포맷이다.

## sce212-project1
### 1. Overview
- MIPS ISA assembler 구현
- Goal : MIPS ISA instruction set 이해, assemblers 원리 이해
- 하나의 assemble file 이 전체 프로그램이 된다.
- 주어진 instruction set 의 subset 을 변환하는 어셈블러를 구현해야 한다.
- 반드시 jump/branch target 을 위한 레이블과 static data section 을 위한 레이블을 다뤄야 한다(?)
### 2. Instruction Set
- Only instructions for unsigned operations need to be implemented
- the immediate fields for certain instructions are sign extended to allow negative numbers (?)
- Only loads and stores with `4B` word need to be implemendted
- The assembler must support decimal and hexadecimal numbers (0x) for the immediate field, and .data section
- The register name is always `$n`, n is from 0 to 31
- la (load address) is a pseudo instruction; it `should be converted to one or two assembly instructions`.
- lui $register, upper 16bit address
- ori $register, lower 16bit address
### 2.1 Directives
**.text**
- indicates that following items are stored in the `user text segment`, typically instructions
- It always starts from `0x400000`

**.data**
- indicates that following data items are stored in the `data segment`
- It always starts from `0x10000000`

**.word**
- store n 32-bit quantities in successive memory words

1. You can assume that the `.data` and `.text` directives appear only once  
2. `.data` must appear before `.text` directive  
3. Assume that each word in the data section is initialized  

### 3. TIL
- fgets() 함수는 스트림에서 문자열을 받아서 (num - 1) 개의 문자를 입력 받을 때 혹은 개행 문자나 EOF 에 도달할 때 까지 입력 받아서 C 형식 문자열로 저장한다. 개행 문자까지 문자열에 저장하고 NULL 문자는 자동적으로 마지막으로 입력받은 문자 뒤에 포함
- strcmp() 함수는 비교하는 두 문자열의 아스키코드 값이 같으면 0 을 반환한다.
- strtok() 함수는 원본 문자열의 변형시킨다. 두 번째 인장 해당하는 값을 만나면 그 곳의 값을 NULL 문자로 변형시킨다.
- fputs() 함수는 str 이 가리키는 문자열을 NULL 문자에 도달할 때 까지 스트림에 복사하는데, 이 때 NULL 문자는 스트림에 복사되지 않는다.
