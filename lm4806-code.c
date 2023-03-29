#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void count_letters(char *list, int num, int *count, int num_threads) {
    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        int local_count[4] = {0};

        int step = num / num_threads;
        int remainder = num % num_threads;
        int thread_id = omp_get_thread_num();
        int start = thread_id * step;
        int end = (thread_id == num_threads - 1) ? start + step + remainder : start + step;

        for (int i = start; i < end; i++) {
            int index = -1;

            switch (list[i]) {
                case 'a': index = 0; break;
                case 'b': index = 1; break;
                case 'c': index = 2; break;
                case 'd': index = 3; break;
            }

            if (index != -1) {
                local_count[index]++;
            }
        }

        #pragma omp critical
        {
            for (int i = 0; i < 4; i++) {
                count[i] += local_count[i];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <N> <num> <filename>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int num = atoi(argv[2]);
    char *filename = argv[3];

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char *list = (char *)malloc(num * sizeof(char));
    for (int i = 0; i < num; i++) {
        list[i] = fgetc(file);
    }
    fclose(file);

    int count[4] = {0}; // count[a, b, c, d]
    count_letters(list, num, count, num_threads);

    free(list);

    int max_index = 0;
    int max_count = count[0];
    for (int i = 1; i < 4; i++) {
        if (count[i] > max_count) {
            max_index = i;
            max_count = count[i];
        }
    }

    char max_char;
    switch (max_index) {
        case 0: max_char = 'a'; break;
        case 1: max_char = 'b'; break;
        case 2: max_char = 'c'; break;
        case 3: max_char = 'd'; break;
        default: max_char = '?'; break;
    }

    printf("%c occurred the most %d times of a total of %d characters.\n", max_char, max_count, num);
    return 0;
}

       

