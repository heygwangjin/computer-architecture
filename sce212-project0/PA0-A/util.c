#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"

int is_large(int num1, int num2)
{
	return num1 > num2;
}

int sum_x(int x1, int x2)
{
	int sum = 0;
	/* Fill this function */
	sum = x1 + x2;
	return sum;
}

void sub_y(int y1, int y2, int *sub)
{
	/* Fill this function */
	*sub = y1 - y2;
}

// You have to allocate memory for pointer members of "struct Point_ref"
// Hint: Use malloc()
void Point_val_to_Point_ref(struct Point_val *P1, struct Point_ref *P2)
{
	/* Fill this function */

	P2->x = (int *)malloc(sizeof(int));
	P2->y = (int *)malloc(sizeof(int));

	*P2->x = P1->x;
	*P2->y = P1->y;

	/** free(P2->x);
	  * free(P2->y); */
}

void Point_ref_to_Point_val(struct Point_ref *P1, struct Point_val *P2)
{
	/* Fill this function */
	P2->x = *P1->x;
	P2->y = *P1->y;
}

int calc_area1(struct Point_val *P1, struct Point_val *P2)
{
	/* Fill this function */
	int area = 0;

	int lengthOfX = P1->x - P2->x;
	int lengthOfY = P1->y - P2->y;

	area = abs(lengthOfX * lengthOfY);
	
	return area;
}

void calc_area2(struct Point_ref *P1, struct Point_ref *P2, int *area)
{
	/* Fill this function */
	int lengthOfX = *P1->x - *P2->x;
	int lengthOfY = *P1->y - *P2->y;

	*area = abs(lengthOfX * lengthOfY);
}

char* reverse(char *word)
{
	/* Fill this function */
	char *arr = (char *)malloc(sizeof(word));
	int i, new = 0;
	int j;

	while(*(word + i) != '\0') {
		*(arr + i) = *(word + i);
		i++;
    }

	*(arr + i) = '\0';

	for(j = i - 1; j >= 0; j--) {
		*(word + new) = *(arr + j);
		new++;
	}	

	*(word + new) = '\0';

	free(arr);

	return word;
}
