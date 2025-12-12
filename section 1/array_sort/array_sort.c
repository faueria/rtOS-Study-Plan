#include <stdio.h>

void bubble_sort(int input_array[], int size) {
	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size - i - 1; j++) {
			if (input_array[j] > input_array[j+1]) {
				int temp = input_array[j];
				input_array[j] = input_array[j+1];
				input_array[j+1] = temp;
			}
		}
	}
}

int main() {
	int array[10] = {2, 6, 4, 9, 7, 8, 2, 4, 1, 5};
	int size = 10;
	for (int i = 0; i < size - 1; i++) {
		printf("%d", array[i]);
	}
	bubble_sort(array, size);
	printf("\n");
	for (int i = 0; i < size - 1; i++) {
		printf("%d", array[i]);
	}
	printf("\n");
	return 0;
}
