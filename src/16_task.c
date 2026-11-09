#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[20];
    float price;
} Record;

_Static_assert(sizeof(Record) == 28,
               "Ошибка компиляции: размер одной записи должен быть равен 28 байтам");

int rewrite_by_id(const char *filename,
                  const int id,
                  const char *name,
                  const float price) {
    int result = 1; // По умолчанию программа завершилась с ошибкой

    FILE *file = fopen(filename, "rb+");
    if (!file) {
        perror("Не удалось открыть файл");
        goto clean;
    }

    Record current_record;
    while (fread(&current_record, sizeof(Record), 1, file) == 1) {
        if (current_record.id == id) {
            current_record.price = price;
            memset(current_record.name, 0, 20);
            strncpy(current_record.name, name, 19);

            // Перемещение указателя назад на одну запись и обновление данных
            fseek(file, -(long)sizeof(Record), SEEK_CUR);
            fwrite(&current_record, sizeof(Record), 1, file);

            goto success;
        }
    }

    puts("Запись с указанным id не найдена");
    goto clean;

success:
    puts("Запись обновлена");
    result = 0;

clean:
    if (file)
        fclose(file);
    return result;
}

int main(int argc,
         char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Некорректные аргументы\n");
        return 1;
    }

    return rewrite_by_id(argv[1], atoi(argv[2]), argv[3], atof(argv[4]));
}
