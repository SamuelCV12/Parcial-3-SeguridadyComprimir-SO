#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <stddef.h> // Para size_t
#include "file_format.h"

typedef enum {
    IO_LINE,   // Streaming: Escribe cada vez que encuentra un '\n'
    IO_CHUNK,  // Batch (Bloques): Escribe alineado a 4KB (Page Size)
    IO_FULL,   // Batch (Total): Escribe todo el buffer de un solo golpe
    IO_MMAP    // OS: Mapea memoria virtual directa a bloques del disco
} IO_Strategy;

// Firma de la función
int save_file_to_disk(const char *filepath, const EditorFileHeader *header, const char *data, size_t size, IO_Strategy strategy);

#endif