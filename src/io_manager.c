#include "io_manager.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#define CHUNK_SIZE 4096 // 4KB, alineado a la página de memoria en Linux

int save_file_to_disk(const char *filepath, const EditorFileHeader *header, const char *data, size_t size, IO_Strategy strategy) {
    // Abrimos con O_RDWR porque mmap requiere permisos de lectura para poder mapear en memoria compartida
    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error crítico abriendo el archivo en el Kernel");
        return -1;
    }

    size_t header_size = (header != NULL) ? sizeof(EditorFileHeader) : 0;
    size_t total_size = header_size + size;

    if (strategy == IO_MMAP) {
        // ESTRATEGIA: Mapeo directo en Memoria (Zero-Copy a nivel de usuario)
        // Expandir el archivo al tamaño final antes de mapear
        if (ftruncate(fd, total_size) == -1) {
            perror("Error truncando archivo para mmap");
            close(fd);
            return -1;
        }

        // Mapear memoria
        void *map = mmap(NULL, total_size, PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            perror("Error mapeando archivo (mmap)");
            close(fd);
            return -1;
        }

        // Copiar directo a la RAM virtual (El Kernel se encarga del disco)
        if (header != NULL) {
            memcpy(map, header, header_size);
        }
        memcpy((char*)map + header_size, data, size);

        // Forzar sincronización asíncrona
        msync(map, total_size, MS_SYNC);
        munmap(map, total_size);

    } else {
        // ESTRATEGIAS TRADICIONALES CON write()
        // Escribir la cabecera estructurada del archivo
        if (header != NULL) {
            if (write(fd, header, header_size) != (ssize_t)header_size) {
                perror("Error escribiendo la cabecera del archivo");
                close(fd);
                return -1;
            }
        }

        if (strategy == IO_LINE) {
            // ESTRATEGIA: Streaming (Línea por línea)
            size_t start = 0;
            for (size_t i = 0; i < size; i++) {
                if (data[i] == '\n' || i == size - 1) {
                    size_t len = i - start + 1;
                    if (write(fd, data + start, len) != (ssize_t)len) {
                        close(fd);
                        return -1;
                    }
                    start = i + 1;
                }
            }
        } 
        else if (strategy == IO_CHUNK) {
            // ESTRATEGIA: Batch en Bloques (Alineado a 4KB)
            size_t bytes_written = 0;
            while (bytes_written < size) {
                size_t bytes_to_write = (size - bytes_written < CHUNK_SIZE) ? (size - bytes_written) : CHUNK_SIZE;
                ssize_t result = write(fd, data + bytes_written, bytes_to_write);
                if (result < 0) {
                    close(fd);
                    return -1;
                }
                bytes_written += result;
            }
        } 
        else if (strategy == IO_FULL) {
            // ESTRATEGIA: Batch Completo
            if (write(fd, data, size) != (ssize_t)size) {
                close(fd);
                return -1;
            }
        }
    }

    close(fd);
    return 0;
}