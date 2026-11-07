#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <tomcrypt.h>

#define BUFFER_SIZE 1048576 // 1 МБ

int encrypt(char *buffer, const char *iv, const char *password) {
    return 0;
}

int decrypt(char *buffer, const char *iv, const char *password) {
    return 0;
}

int work(const char mode[2], const size_t filesize, const char *password, FILE *input_file, FILE *output_file) {
    int result = 1;
    char iv[16];
    char *buffer = NULL;

    if (strcmp(mode, "-e") == 0) {
        FILE *rndm_file = fopen("/dev/urandom", "rb");
        fread(iv, 1, 16, rndm_file);
        fclose(rndm_file);

        fwrite(iv, 1, 16, output_file);
    } else {
        fread(iv, 1, 16, input_file);
    }

    char *aes_key;

    if (filesize > BUFFER_SIZE) {
        buffer = malloc(BUFFER_SIZE);
        size_t bytes_read;

        while (
            (bytes_read = fread(buffer, 1, BUFFER_SIZE, input_file)) > 0
        ) {
            if (strcmp(mode, "-e") == 0) {
                if (encrypt(buffer, iv, aes_key) != 0) goto error;
            } else {
                if (decrypt(buffer, iv, aes_key) != 0) goto error;
            }
        }
    } else {
        buffer = malloc(filesize);
        fread(buffer, 1, filesize, input_file);

        if (strcmp(mode, "-e") == 0) {
            if (encrypt(buffer, iv, aes_key) != 0) goto error;
        } else {
            if (decrypt(buffer, iv, aes_key) != 0) goto error;
        }
    }

    result = 0;

    error:
        if (buffer != NULL) free(buffer);
        return result;

}

int main(int argc, char *argv[]) {
    int result = 1;

    if (argc != 5 || (strcmp(argv[1], "-e") != 0 && strcmp(argv[1], "-d") != 0)
    ) {
        printf("Неправильное использование программы");
        return 1;
    }

    FILE *input_file = fopen(argv[2], "rb");
    if (!input_file) {
        perror("Ошибка открытия входного файла");
        goto error;
    }

    FILE *output_file = fopen(argv[3], "wb");
    if (!output_file) {
        perror("Ошибка создания выходного файла");
        goto error;
    }

    size_t filesize;
    {
        struct stat st;
        if (stat(argv[2], &st) == 0) filesize = st.st_size;
    }

    if (
        work(argv[1], filesize, argv[4], input_file, output_file) != 0
    ) goto error;

    printf("Файл %s успешно %s и сохранен в %s", argv[2], strcmp(argv[1], "-e") == 0 ? "зашифрован" : "дешифрован", argv[3]);
    result = 0;

    error:
        if (input_file) fclose(input_file);
        if (output_file) fclose(output_file);
        return result;
}
