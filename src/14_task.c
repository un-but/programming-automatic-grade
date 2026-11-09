#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096 // 4 КБ

int copy_file(const char *src_path,
              const char *dst_path) {
    int result = 1; // По умолчанию программа завершилась с ошибкой
    FILE *src_file = NULL, *dst_file = NULL;

    char *buffer[20];

    src_file = fopen(src_path, "rb");
    if (!src_file) {
        perror("Не получилось открыть исходный файл");
        goto error;
    }

    dst_file = fopen(dst_path, "wb");
    if (!dst_file) {
        perror("Не получилось создать файл назначения");
        goto error;
    }

    long filesize;
    {
        struct stat st;
        if (stat(src_path, &st) == 0) {
            filesize = st.st_size;
        } else {
            fprintf(stderr, "Не удалось установить размер входного файла");
            goto error;
        }
    }

    if (filesize == 0) {
        puts("Прогресс 100%");
        goto success;
    }

    size_t bytes_read, bytes_written;

    long total_progress = 0;
    int last_percent = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, dst_file);

        if (bytes_read != bytes_written) {
            perror("Ошибка копирования");
            goto error;
        }

        total_progress += bytes_read;
        const int current_percent = (int)((total_progress * 100) / filesize);

        if (current_percent != last_percent) {
            last_percent = current_percent;
            printf("Прогресс %d%%\n", current_percent);
            fflush(stdout);
        }
    }

success:
    puts("\nФайл скопирован");
    result = 0;

error:
    if (src_file)
        fclose(src_file);
    if (dst_file)
        fclose(dst_file);
    return result;
}

int main(int argc,
         char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Некорректные аргументы\n");
        return 1;
    }

    return copy_file(argv[1], argv[2]);
}
