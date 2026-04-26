#include "editor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

GapBuffer* gap_buffer_create(size_t initial_size) {
    if (initial_size == 0) initial_size = 1024;
    GapBuffer *gb = malloc(sizeof(GapBuffer));
    if (!gb) return NULL;
    
    gb->buffer = malloc(initial_size);
    if (!gb->buffer) {
        free(gb);
        return NULL;
    }
    
    gb->size = initial_size;
    gb->gap_start = 0;
    gb->gap_end = initial_size;
    return gb;
}

void gap_buffer_free(GapBuffer *gb) {
    if (gb) {
        if (gb->buffer) free(gb->buffer);
        free(gb);
    }
}

void gap_buffer_expand(GapBuffer *gb, size_t needed_space) {
    size_t current_gap_size = gb->gap_end - gb->gap_start;
    if (current_gap_size >= needed_space) return; // Hay espacio suficiente

    size_t new_size = gb->size * 2;
    // Asegurar que el nuevo tamaño alcance
    while (new_size - (gb->size - current_gap_size) < needed_space) {
        new_size *= 2;
    }

    char *new_buffer = malloc(new_size);
    if (!new_buffer) {
        perror("Error crítico: No se pudo expandir el Gap Buffer");
        exit(EXIT_FAILURE);
    }

    // Copiar la primera parte (antes del gap)
    memcpy(new_buffer, gb->buffer, gb->gap_start);
    
    // Copiar la segunda parte (después del gap)
    size_t after_gap_size = gb->size - gb->gap_end;
    size_t new_gap_end = new_size - after_gap_size;
    memcpy(new_buffer + new_gap_end, gb->buffer + gb->gap_end, after_gap_size);

    free(gb->buffer);
    gb->buffer = new_buffer;
    gb->gap_end = new_gap_end;
    gb->size = new_size;
}

void gap_buffer_move_cursor(GapBuffer *gb, size_t pos) {
    size_t logical_size = gb->size - (gb->gap_end - gb->gap_start);
    if (pos > logical_size) pos = logical_size; // Clampear al final

    if (pos < gb->gap_start) {
        // Mover a la izquierda
        size_t distance = gb->gap_start - pos;
        memmove(gb->buffer + gb->gap_end - distance, gb->buffer + pos, distance);
        gb->gap_start -= distance;
        gb->gap_end -= distance;
    } else if (pos > gb->gap_start) {
        // Mover a la derecha
        size_t distance = pos - gb->gap_start;
        memmove(gb->buffer + gb->gap_start, gb->buffer + gb->gap_end, distance);
        gb->gap_start += distance;
        gb->gap_end += distance;
    }
}

void gap_buffer_insert(GapBuffer *gb, const char *str) {
    size_t len = strlen(str);
    gap_buffer_expand(gb, len);

    // Escribir los datos en el gap
    memcpy(gb->buffer + gb->gap_start, str, len);
    gb->gap_start += len;
}

void gap_buffer_delete(GapBuffer *gb) {
    if (gb->gap_start > 0) {
        gb->gap_start--;
    }
}

char* gap_buffer_get_content(GapBuffer *gb, size_t *out_length) {
    size_t logical_size = gb->size - (gb->gap_end - gb->gap_start);
    char *content = malloc(logical_size + 1);
    if (!content) return NULL;

    // Juntar ambas mitades omitiendo el gap
    memcpy(content, gb->buffer, gb->gap_start);
    size_t after_gap_size = gb->size - gb->gap_end;
    memcpy(content + gb->gap_start, gb->buffer + gb->gap_end, after_gap_size);
    
    content[logical_size] = '\0';
    if (out_length) *out_length = logical_size;
    
    return content;
}
