#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include "caesar.h"

#define PORT 8080

static size_t count = 0;

void print_system_load(size_t size_text) {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        double load = info.loads[0] / 65536.0; 
        long total_memory = info.totalram / 1024 / 1024; 
        long free_memory = info.freeram / 1024 / 1024;   
        printf("[%ld] System Load: %.2f, Total Memory: %ld MB, Free Memory: %ld MB, Memory String: %ld MB\n",
            count, load, total_memory, free_memory, size_text / 1024);
    } else {
        perror("Ошибка при получении информации о системе");
    }
} 

// Функция обработки HTTP-запроса
int request_handler(void *cls, struct MHD_Connection *connection, const char *url,
                    const char *method, const char *version, const char *upload_data,
                    size_t *upload_data_size, void **ptr) {
    count++;
    static int dummy;
    if (&dummy != *ptr) {
        *ptr = &dummy;
        return MHD_YES;
    }
    *ptr = NULL;

    if (strcmp(method, "GET") != 0) {
        return MHD_NO; // Разрешаем только GET запросы
    }

    const char *text_param = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "text");
    const char *shift_param = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "shift");

    if (!text_param || !shift_param) {
        const char *error_message = "Missing text or shift parameter\n";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_message),
                                                                        (void *)error_message, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
        MHD_destroy_response(response);
        return ret;
    }

    int shift = atoi(shift_param);

    size_t text_len = strlen(text_param);
    char *text = (char *)malloc(text_len + 1);
    if (text == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память.\n");
        return MHD_NO;
    }

    strncpy(text, text_param, text_len);
    text[text_len] = '\0';

    if (strcmp(url, "/encrypt") == 0) {
        caesar_encrypt(text, shift);
    } else if (strcmp(url, "/decrypt") == 0) {
        caesar_decrypt(text, shift);
    } else {
        const char *error_message = "Unknown endpoint\n";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_message),
                                                                        (void *)error_message, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
        MHD_destroy_response(response);
        free(text);  
        return ret;
    }

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(text), (void *)text, MHD_RESPMEM_MUST_COPY);
    free(text);  
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    print_system_load(strlen(text_param));
    return ret;
}

int main() {
    struct MHD_Daemon *daemon = MHD_start_daemon(
        MHD_USE_INTERNAL_POLLING_THREAD,
        PORT,
        NULL,
        NULL,
        &request_handler,
        NULL,
        MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)10,
        MHD_OPTION_THREAD_POOL_SIZE, (unsigned int)10,
        MHD_OPTION_CONNECTION_LIMIT, (unsigned int)100,
        MHD_OPTION_END
    );
    if (NULL == daemon) return 1;

    printf("Server running on http://localhost:%d\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return 0;
}

