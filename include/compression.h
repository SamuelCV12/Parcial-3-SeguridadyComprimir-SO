#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stddef.h>

// Comprime un string usando Run-Length Encoding (RLE).
// Retorna el tamaño del nuevo buffer comprimido.
// Ojo: 'output' será apuntado a un nuevo buffer dinámico que debe ser liberado.
size_t compress_rle(const char *input, size_t input_size, char **output);

// Descomprime un buffer usando Run-Length Encoding (RLE).
// Retorna 1 si fue exitoso, 0 en error.
// 'output' apuntará al texto inflado.
int decompress_rle(const char *input, size_t input_size, size_t original_size, char **output);

#endif // COMPRESSION_H