#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <time.h>
#include <tomcrypt.h>

#define BUFFER_SIZE 1048576 // 1 МБ

void generate_key(const char *password,
                  unsigned char *key_out) {
    hash_state hash;

    sha256_init(&hash);
    sha256_process(&hash, (const unsigned char *)password, strlen(password));
    sha256_done(&hash, key_out);

    zeromem(&hash, sizeof(hash));
}

int encrypt(unsigned char *buffer,
            size_t buffer_len,
            FILE *output_file,
            symmetric_CBC *cbc,
            bool is_last) {
    // Добавляем padding
    if (is_last) {
        const size_t padding_len = 16 - (buffer_len % 16);
        memset(buffer + buffer_len, (int)padding_len, padding_len);
        buffer_len += padding_len;
    }

    if (cbc_encrypt(buffer, buffer, buffer_len, cbc) != CRYPT_OK)
        return 1;

    if (fwrite(buffer, 1, buffer_len, output_file) < buffer_len)
        return 1;

    return 0;
}

int decrypt(unsigned char *buffer,
            size_t buffer_len,
            FILE *output_file,
            symmetric_CBC *cbc,
            bool is_last) {
    if (cbc_decrypt(buffer, buffer, buffer_len, cbc) != CRYPT_OK)
        return 1;

    // Убираем padding
    if (is_last && buffer_len != 0) {
        if (buffer_len % 16 != 0)
            return 1;

        const size_t padding_len = buffer[buffer_len - 1];

        for (size_t i = 0; i < padding_len; i++) {
            if (buffer[buffer_len - i - 1] != padding_len)
                return 1;
        }

        buffer_len -= padding_len;
    }

    if (fwrite(buffer, 1, buffer_len, output_file) < buffer_len)
        return 1;

    return 0;
}

int work(const char mode[2],
         const size_t filesize,
         const char *password,
         FILE *input_file,
         FILE *output_file) {
    int result = 1;

    unsigned char iv[16];
    unsigned char *buffer = NULL;
    symmetric_CBC cbc;

    if (strcmp(mode, "-e") == 0) {
        FILE *rndm_file = fopen("/dev/urandom", "rb");
        fread(iv, 1, 16, rndm_file);
        fclose(rndm_file);

        fwrite(iv, 1, 16, output_file);
    } else {
        fread(iv, 1, 16, input_file);
    }

    unsigned char aes_key[32];
    generate_key(password, aes_key);

    if (cbc_start(find_cipher("aes"), iv, aes_key, 32, 0, &cbc) != CRYPT_OK)
        goto error;

    buffer = malloc(BUFFER_SIZE);
    size_t bytes_remained = filesize, bytes_read;

    while ((bytes_read = fread(buffer,
                               1,
                               BUFFER_SIZE > bytes_remained ? bytes_remained : BUFFER_SIZE,
                               input_file)) > 0) {
        bool is_last = (bytes_remained - bytes_read <= 0);

        if (strcmp(mode, "-e") == 0) {
            if (encrypt(buffer, bytes_read, output_file, &cbc, is_last) != 0)
                goto error;
        } else {
            if (decrypt(buffer, bytes_read, output_file, &cbc, is_last) != 0)
                goto error;
        }

        bytes_remained -= bytes_read;
    }

    result = 0;

error:
    zeromem(aes_key, sizeof(aes_key));
    cbc_done(&cbc);

    if (buffer != NULL) {
        zeromem(buffer, BUFFER_SIZE);
        free(buffer);
    }

    return result;
}

int main(int argc,
         char *argv[]) {
    int result = 1;

    if (argc != 5 || (strcmp(argv[1], "-e") != 0 && strcmp(argv[1], "-d") != 0)) {
        fprintf(stderr, "Неправильное использование программы\n");
        return 1;
    }

    FILE *input_file = NULL, *output_file = NULL;
    register_cipher(&aes_desc);

    input_file = fopen(argv[2], "rb");
    if (!input_file) {
        perror("Ошибка открытия входного файла");
        goto error;
    }

    output_file = fopen(argv[3], "wb");
    if (!output_file) {
        perror("Ошибка создания выходного файла");
        goto error;
    }

    size_t filesize;
    {
        struct stat st;
        if (stat(argv[2], &st) == 0) {
            filesize = (size_t)st.st_size;
        } else {
            fprintf(stderr, "Не удалось установить размер входного файла\n");
            goto error;
        }
    }

    if (work(argv[1], filesize, argv[4], input_file, output_file) != 0)
        goto error;

    printf("Файл %s успешно %s и сохранен в %s\n",
           argv[2],
           strcmp(argv[1], "-e") == 0 ? "зашифрован" : "дешифрован",
           argv[3]);
    result = 0;

error:
    if (input_file)
        fclose(input_file);
    if (output_file)
        fclose(output_file);
    return result;
}
