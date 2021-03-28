#include "dir_file.h"
#include "utils.h"

int open_file(char* fname, FILE** input) {
    if (access(fname, F_OK) == -1) {
        ERR_PRINT("The '%s' file does not exists\n", fname);
        return -1;
    }

    *input = fopen(fname, "r");
    if (input == NULL) {
        ERR_PRINT("Failed open '%s' file\n", fname);
        return -1;
    }

    return 1;
}

// This parse_str_to_list() split string to the tokens, and put the tokens in token_list.
// The value of return is the number of tokens.
int parse_str_to_list(const char* str, char** token_list) {
    /* Fill this function */
	int num_token = 0;
	char copy[MAX_BUFFER_SIZE], *token;

	strcpy(copy, str);
	
	// strtok 함수는 문자열에서 찾은 토큰의 첫 번째 주소값을 리턴한다. 토큰이 더 이상 없다면 NULL을 리턴한다.
	// strtok 함수는 첫 번째 구분 문자들을 null 문자로 덮어쓴다. 그래서 무조건 원본 문자열에 변경이 가해진다.
	token = strtok(copy, DELIM);
	while (token != NULL) {
		// *(token_list + num_token) = (char*)malloc(sizeof(char) * strlen(token) + 1); // strlen 함수는 널 문자를 제외한 길이
		*(token_list + num_token) = (char*)malloc(sizeof(char) * strlen(token)); // token 은 null 문자를 포함하지 않은 문자열이기 때문에, strlen 에 + 1 이 필요 없다.
		strcpy(*(token_list + num_token), token);
		/** printf("|||%s|||", *(token_list + num_token)); */
		token = strtok(NULL, DELIM);
		num_token++;
		/** printf("%d|||\n", num_token); */
	}

	return num_token;
}

// 여기서 token_list[index] 에서 사용한 메모리를 해제 하기 때문에, 위의 함수에서 메모리 해제를 하면 안된다.
void free_token_list(char** token_list, int num_token) {
    int index;

    for (index = 0; index < num_token; index++) {
        free(token_list[index]);
    }

    free(token_list);
}
