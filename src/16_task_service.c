#include <stdio.h>
#include <string.h>

typedef struct {
    int id;
    char name[20];
    float price;
} Record;

Record records[] = {
    {1, "Яблоко", 1.10f},
    {2, "Апельсин", 1.20f},
    {3, "Мандарин", 1.30f},
    {4, "Банан", 1.40f},
    {5, "Дыня", 1.50f},
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Неправильные аргументы\n");
        return 1;
    }

    if (strncmp(argv[1], "init", 4) == 0) {
        FILE *file = fopen("files/16_task.bin", "wb");
        if (!file) {
            perror("Ошибка открытия файла");
            return 1;
        }

        fwrite(records, sizeof(Record), 5, file);
        fclose(file);
    } else if (strncmp(argv[1], "read", 4) == 0) {
        FILE *file = fopen("files/16_task.bin", "rb");
        if (!file) {
            perror("Ошибка открытия файла");
            return 1;
        }

        Record record;
        while (fread(&record, sizeof(Record), 1, file) == 1) {
            printf("ID: %d Name: %s Price: %f\n", record.id, record.name, record.price);
        }

        fclose(file);
    }
}
