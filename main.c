#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int width;
int height;
int **image;
int **negative_image;
int number_of_threads;


void read_line(char *buffer, FILE *input_file) {
    do {
        fgets(buffer, MAX_LINE_LENGTH, input_file);
    } while (buffer[0] == '#' || buffer[0] == '\n');
}

void load_image(char *input_filename) {
    FILE *input_file = fopen(input_filename, "r");

    char buffer[MAX_LINE_LENGTH + 1];

    // P2
    read_line(buffer, input_file);
    // P2

    read_line(buffer, input_file);
    width = atoi(strtok(buffer, " \n\t\r"));
    height = atoi(strtok(NULL, " \n\t\r"));

    image = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++) {
        image[i] = calloc(width, sizeof(int));
    }

    // M
    read_line(buffer, input_file);
    // M

    read_line(buffer, input_file);
    char *char_pixel_value = strtok(buffer, " \n\t\r");
    for(int h = 0; h < height; h++){
        for(int w = 0; w < width; w++){
            if (char_pixel_value == NULL) {
                read_line(buffer, input_file);
                char_pixel_value = strtok(buffer, " \n\t\r");
            }
            int pixel_value = atoi(char_pixel_value);
            image[h][w] = pixel_value;
            char_pixel_value = strtok(NULL, " \n\t\r");
        }
    }

    fclose(input_file);
}

void save_negative_image(char *output_filename){
    FILE *output_file = fopen(output_filename, "w+");

    fprintf(output_file, "P2\n");

    fprintf(output_file, "%d %d\n", width, height);

    fprintf(output_file, "255\n");

    int new_line = 0;
    for(int h = 0; h < height; h++){
        for(int w = 0; w < width; w++){
            new_line++;
            fprintf(output_file, "%d ", negative_image[h][w]);
            if(new_line == 10){
                new_line = 0;
                fprintf(output_file, "\n");
            }
        }
    }

    fclose(output_file);
}

int time_micro(struct timespec *start, struct timespec *end){
    return (1000000 * (end->tv_sec - start->tv_sec)) + ((end->tv_nsec - start->tv_nsec) / 1000);
}

void *numbers(void *argument){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int *int_argument_pointer = (int*)argument;
    int int_argument = *int_argument_pointer;

    int number_set_size = ceil((double)256 / (double)number_of_threads);
    for(int h = 0; h < height; h++) {
        for(int w = 0; w < width; w++) {
            if(image[h][w] / number_set_size == int_argument) negative_image[h][w] = 255 - image[h][w];
        }
    }

    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    int *thread_time = calloc(1, sizeof(int));
    *thread_time = time_micro(&start, &end);
    pthread_exit(thread_time);
    return NULL;
}

void *block(void *argument){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int *int_argument_pointer = (int*)argument;
    int int_argument = *int_argument_pointer;

    int x_set_size = ceil((double)width / (double)number_of_threads);
    for (int x = int_argument * x_set_size; x < (int_argument + 1) * x_set_size; x++) {
        if(x < width){
            for (int y = 0; y < height; y++) negative_image[y][x] = 255 - image[y][x];
        }
    }

    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    int *thread_time = calloc(1, sizeof(int));
    *thread_time = time_micro(&start, &end);
    pthread_exit(thread_time);
    return NULL;
}


int main(int argc, char *argv[]){
    if (argc != 5){
        printf("Wrong number of arguments!");
        exit(-1);
    }
    number_of_threads = atoi(argv[1]);
    char *mode = argv[2];
    char *input_filename = argv[3];
    char *output_filename = argv[4];

    load_image(input_filename);

    negative_image = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++) {
        negative_image[i] = calloc(width, sizeof(int));
    }

    int **thread_times = calloc(number_of_threads, sizeof(int*));

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t *threads = calloc(number_of_threads, sizeof(pthread_t));
    int *arguments = calloc(number_of_threads, sizeof(int));

    for (int i = 0; i < number_of_threads; i++) arguments[i] = i;
    if (strcmp(mode, "numbers") == 0) {
        for (int i = 0; i < number_of_threads; i++) {
            pthread_create(&threads[i], NULL, &numbers, &arguments[i]);
        }
    }
    else if (strcmp(mode, "block") == 0) {
        for (int i = 0; i < number_of_threads; i++) {
            pthread_create(&threads[i], NULL, &block, &arguments[i]);
        }
    }

    for (int i = 0; i < number_of_threads; i++) pthread_join(threads[i], (void**)&(thread_times[i]));

    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    int total_time = time_micro(&start, &end);

    save_negative_image(output_filename);

    for (int i = 0; i < number_of_threads; i++){
        printf("Thread %d time: %d microseconds\n", i+1, *thread_times[i]);
    }
    printf("Total time: %d microseconds\n", total_time);

    return 0;
}