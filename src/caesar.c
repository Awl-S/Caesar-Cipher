#include <caesar.h>
#include <string.h>

void caesar_encrypt(char *text, int shift) {
    int len = strlen(text);

    for (int i = 0; i < len; i++) {
        char c = text[i];

        int is_upper = (c >= 'A') & (c <= 'Z');  // 1 для заглавных
        int is_lower = (c >= 'a') & (c <= 'z');  // 1 для строчных
        int is_alpha = is_upper | is_lower;      // 1 для букв

        char offset = 'A' * is_upper + 'a' * is_lower;

        text[i] = is_alpha * ((c - offset + shift) % 26 + offset) + (1 - is_alpha) * c;
    }
}

void caesar_decrypt(char *text, int shift) {
    int len = strlen(text);

    for (int i = 0; i < len; i++) {
        char c = text[i];

        int is_upper = (c >= 'A') & (c <= 'Z');  // 1 для заглавных букв
        int is_lower = (c >= 'a') & (c <= 'z');  // 1 для строчных букв
        int is_alpha = is_upper | is_lower;      // 1 для букв

        char offset = 'A' * is_upper + 'a' * is_lower;

        text[i] = is_alpha * ((c - offset - shift + 26) % 26 + offset) + (1 - is_alpha) * c;
    }
}