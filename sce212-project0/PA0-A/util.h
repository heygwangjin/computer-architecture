
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*        DO NOT MODIFY THIS FILE!                             */
/*        You should only modify the util.c file!              */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

// 전처리기는 컴파일 하기 전에 #이 붙은 코드를 지정해놓은 코드로 변경하는 일을 한다.
#ifndef __UTIL_H__
#define __UTIL_H__

struct Point_val
{
	int x;
	int y;
};

struct Point_ref
{
	int *x;
	int *y;
};

int is_large(int num1, int num2);
int sum_x(int x1, int x2);
void sub_y(int y1, int y2, int *sub);
void Point_val_to_Point_ref(struct Point_val *P1, struct Point_ref *P2);
void Point_ref_to_Point_val(struct Point_ref *P1, struct Point_val *P2);
int calc_area1(struct Point_val *P1, struct Point_val *P2);
void calc_area2(struct Point_ref *P1, struct Point_ref *P2, int *area);
char* reverse(char *word);

#endif
