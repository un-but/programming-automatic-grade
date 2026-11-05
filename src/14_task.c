#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096 // 4 КБ

long get_file_size(FILE *file) {
    const long start_pos = ftell(file);

    fseek(file, 0, SEEK_END);
    const long size = ftell(file);

    fseek(file, start_pos, SEEK_SET);
    return size;
}

int copy_file(const char *src_path, const char *dst_path) {
    int result = 1; // По умолчанию программа завершилась с ошибкой
    FILE *src_file = NULL, *dst_file = NULL;

    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Ошибка выделения буфера");
        goto clean;
    }

    src_file = fopen(src_path, "rb");
    if (!src_file) {
        perror("Не получилось открыть исходный файл");
        goto clean;
    }

    dst_file = fopen(dst_path, "wb");
    if (!dst_file) {
        perror("Не получилось создать файл назначения");
        goto clean;
    }

    const long filesize = get_file_size(src_file);
    if (filesize == 0) {
        puts("Прогресс 100%");
        goto success;
    }

    size_t bytes_read, bytes_written;

    long total_progress = 0;
    short last_percent = 0;

    while (
        (bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file)) > 0
    ) {
        bytes_written = fwrite(buffer, 1, bytes_read, dst_file);

        if (bytes_read != bytes_written) {
            perror("Ошибка копирования");
            goto clean;
        }

        total_progress += bytes_read;
        const short current_percent = (total_progress * 100) / filesize;

        if (current_percent != last_percent) {
            last_percent = current_percent;
            printf("Прогресс %d%%\n", current_percent);
            fflush(stdout);
        }

    }

    success:
        puts("\nФайл скопирован");
        result = 0;

    clean:
        if (src_file) fclose(src_file);
        if (dst_file) fclose(dst_file);

        free(buffer);
        return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Некорректные аргументы\n");
        return 1;
    }

    return copy_file(argv[1], argv[2]);
}
