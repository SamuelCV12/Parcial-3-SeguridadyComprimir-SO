#ifndef FILE_FORMAT_H
#define FILE_FORMAT_H

#include <stdint.h>

// "EDTX" en formato Little-Endian (Editor Texto)
#define EDITOR_MAGIC_NUMBER 0x58544445 

// Estructura de cabecera de archivo.
// Utilizamos __attribute__((packed)) para evitar el "padding" (relleno de bytes) 
// que el compilador suele hacer por temas de alineación. Así, el struct medirá 
// exactamente la suma de los bytes de sus tipos (4 + 1 + 1 + 1 + 1 + 8 = 16 bytes).
typedef struct __attribute__((packed)) {
    uint32_t magic_number;  // 4 bytes: Identificador de nuestro formato propietario
    uint8_t  version;       // 1 byte: Versión del formato de archivo (ej. 1)
    uint8_t  is_compressed; // 1 byte: Bandera booleana (1 si es RLE, 0 plano)
    uint8_t  text_color;    // 1 byte: Metadato - color del texto
    uint8_t  bg_color;      // 1 byte: Metadato - color del fondo
    uint64_t original_size; // 8 bytes: Tamaño original antes de comprimir (para RLE)
} EditorFileHeader;

#endif
