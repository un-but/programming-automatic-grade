#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 106496 // 1 КБ

const long get_file_size(FILE *file) {
    const long start_pos = ftell(file);

    fseek(file, 0, SEEK_END);
    const long size = ftell(file);

    fseek(file, start_pos, SEEK_SET);
    return size;
}

int copy_file(const char *src_path, const char *dst_path) {
    FILE *src_file = fopen(src_path, "rb");
    if (!src_file) {
        perror("Не получилось открыть исходный файл");
        return 1;
    }

    FILE *dst_file = fopen(dst_path, "w");
    if (!dst_file) {
        perror("Не получилось создать файл назначения");
        return 1;
    }

    const char filesize = get_file_size(src_file);

    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Ошибка выделения буфера");
        goto clean;
    }

    long step = filesize / 100;
    long percent = 0;
    long total_progress = 0, step_progress = 0;

    clean:
        fclose(src_file);
        fclose(dst_file);
        return 1;

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        perror("Некорректные аргументы");
        return 1;
    }

    return copy_file(argv[1], argv[2]);
}
