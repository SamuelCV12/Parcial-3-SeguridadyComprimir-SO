#include "compression.h"
#include <stdlib.h>
#include <stdio.h>

size_t compress_rle(const char *input, size_t input_size, char **output) {
    if (input_size == 0) return 0;

    // En el peor de los casos (texto sin repeticiones continuas), RLE puede duplicar 
    // el tamaño original (ej. "ABC" -> "1A1B1C"). Aseguramos el buffer máximo.
    *output = malloc(input_size * 2);
    if (!*output) {
        perror("Error crítico: Fallo de asignación de memoria en compresión");
        return 0;
    }

    size_t out_pos = 0;
    for (size_t i = 0; i < input_size; i++) {
        unsigned char count = 1;
        
        // Contar coincidencias consecutivas (máximo 255 porque usamos 1 byte para el conteo)
        while (i + 1 < input_size && input[i] == input[i+1] && count < 255) {
            count++;
            i++;
        }
        
        // Escribir la cantidad y luego el carácter
        (*output)[out_pos++] = count;
        (*output)[out_pos++] = input[i];
    }

    return out_pos; // Retornamos el nuevo tamaño, que idealmente será mucho menor al original
}

int decompress_rle(const char *input, size_t input_size, size_t original_size, char **output) {
    if (input_size == 0) return 0;
    
    *output = malloc(original_size + 1);
    if (!*output) {
        perror("Error crítico: Fallo de asignación de memoria en descompresión");
        return 0;
    }

    size_t out_pos = 0;
    // Iteramos de a 2 bytes: [cantidad][caracter]
    for (size_t i = 0; i < input_size - 1 && out_pos < original_size; i += 2) {
        unsigned char count = input[i];
        char c = input[i+1];
        
        for (unsigned char j = 0; j < count; j++) {
            if (out_pos < original_size) {
                (*output)[out_pos++] = c;
            }
        }
    }
    
    return 1;
}