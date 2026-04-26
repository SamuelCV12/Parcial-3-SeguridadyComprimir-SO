#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>

// Estructura fundamental para el manejo de texto eficiente.
// Permite inserciones y borrados en O(1) amortizado cerca del cursor.
typedef struct {
    char *buffer;      // Puntero a la memoria total alocada
    size_t size;       // Capacidad máxima de memoria reservada
    size_t gap_start;  // Índice donde inicia el hueco (cursor lógico)
    size_t gap_end;    // Índice donde finaliza el hueco
} GapBuffer;

// Inicializa el Gap Buffer con una capacidad inicial
GapBuffer* gap_buffer_create(size_t initial_size);

// Libera exhaustivamente toda la memoria para evitar leaks (Crash Test)
void gap_buffer_free(GapBuffer *gb);

// Inserta texto dinámicamente en la posición del cursor (gap_start)
void gap_buffer_insert(GapBuffer *gb, const char *str);

// Borra el carácter justo detrás del cursor (equivalente a Backspace)
void gap_buffer_delete(GapBuffer *gb);

// Expande dinámicamente el buffer si falta espacio (uso interno o de carga)
void gap_buffer_expand(GapBuffer *gb, size_t needed_space);

// Mueve el cursor dentro del documento, trasladando el gap
void gap_buffer_move_cursor(GapBuffer *gb, size_t pos);

// Recupera el texto contiguo para guardarlo en disco
// Retorna un buffer nuevo que DEBE ser liberado por el usuario
char* gap_buffer_get_content(GapBuffer *gb, size_t *out_length);

#endif
