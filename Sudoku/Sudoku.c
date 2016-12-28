#include <stdio.h>
#include <math.h>
#include <memory.h>

//A function that count bit
#if defined(_WIN32)	
#define popcnt __popcnt
#elif defined(linux)
#define popcnt __builtin_popcount
#endif

#define BIT9 0b111111111

typedef struct {
	int v;	//Value
	int p;	//Possibility, represent by bit
} num;

void insertNum(num sudoku[9][9], int x, int y, int v);
int checkNum(num sudoku[9][9]);

int main(int argc, char* args[]) {
	if (argc < 2) {
		printf("Give me a path of sudoku file");
		return -1;
	}

	num ***backup[8];
	int backupi=0;
	num sudoku[9][9];

	memset(sudoku, ~0, sizeof(sudoku));	//All bit is setted to 1
	
	//Read input from file
	FILE *fp;
	fopen_s(&fp, args[1], "rt");
	for (int x = 0; x < 9; x++)
		for (int y = 0; y < 9; y++) {
			fscanf_s(fp, "%d", &sudoku[x][y].v);
			sudoku[x][y].p &= sudoku[x][y].v ? 0 : BIT9;
			insertNum(sudoku, x, y, ~((1 << sudoku[x][y].v) >> 1));
		}
	fclose(fp);

	int flag;
	while (flag = checkNum(sudoku)) {
		if (flag == -1) {	// There are multiple number to choice, so backup and choose one
			backup[backupi] = malloc(sizeof(sudoku));
			for(int i=0; i<9*9; i++)
				if (!~(*(sudoku) + i)->v) {	//Choose a num which have no value
					for (int t = 0; t < 9; t++)
						if ((*(sudoku) + i)->p & 1 << t) {	//Choose a number which possible
							(*(sudoku) + i)->p &= ~(1 << t);	//Prevent to choose same number
							memcpy_s(backup[backupi++], sizeof(sudoku), sudoku, sizeof(sudoku));
							insertNum(sudoku, i / 9, i % 9, ~(1 << t));
							break;
						}
					break;
				}
		} else if (flag == -2) { //Can't continue, so recover backup
			if (!backupi) {
				printf("There is no correct answer");
				return -2;
			}
			memcpy_s(sudoku, sizeof(sudoku), backup[--backupi], sizeof(sudoku));
			free(backup[backupi]);
		}
	}

	//Print result
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++)
			if (sudoku[x][y].v != ~0)
				printf("%d ", (int)log2(~sudoku[x][y].v) + 1);
			else
				printf("%d ", 0);
		puts("");
	}

	return 0;
}

//Set value and remove possibility from related num
void insertNum(num sudoku[9][9], int x, int y, int v) {
	sudoku[x][y].v = v; //Set value
	if (v != ~0) {
		sudoku[x][y].p &= v;	//Remove this value from possibility
		for (int i = 0; i < 9; i++)	//Remove this value from possibility in horizontally
			sudoku[x][i].p &= v;
		for (int i = 0; i < 9; i++)	//Remove this value from possibility vertically
			sudoku[i][y].p &= v;
		for (int xi = 0; xi < 3; xi++)	//Remove this value from possibility in the 3 by 3 box
			for (int yi = 0; yi < 3; yi++)
				sudoku[x / 3 * 3 + xi][y / 3 * 3 + yi].p &= v;
	}
}

//Check current state
int checkNum(num sudoku[9][9]) {
	int check = 0;
	for (int x = 0; x < 9; x++)
		for (int y = 0; y < 9; y++)
			if (!~sudoku[x][y].v)
				if (!sudoku[x][y].p) //Can't continue
					return -2;
				 else if (popcnt(sudoku[x][y].p) == 1) {
					insertNum(sudoku, x, y, ~sudoku[x][y].p);
					check = 1;
				} else if (!check && !~sudoku[x][y].v)	//There are multiple number to choice
					check = -1;
				
	return check;
}