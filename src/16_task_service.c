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

int main() {
    FILE *file = fopen("files/16_task.bin", "wb");

    // Расширение имени до 20 символов (последний всегда \0)
    char *current_name;
    for (int i = 0; i < 5; i++) {
        current_name = records[i].name;
        memset(records[i].name, 0, 20);
        strncpy(records[i].name, current_name, 19);
    }

    fwrite(records, sizeof(Record), 5, file);
    fclose(file);
}
