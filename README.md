# EDTX | Editor de Texto de Bajo Nivel con I/O Optimizado
![Lenguaje](https://img.shields.io/badge/Lenguaje-C-blue)
![SO](https://img.shields.io/badge/SO-Linux-orange)

## 📌 Descripción General
**EDTX** es un editor de texto interactivo para terminal desarrollado en **C nativo**, diseñado como un proyecto integrador de **Sistemas Operativos**. A diferencia de un editor convencional, EDTX se enfoca en la manipulación directa de la memoria, la reducción de *context switches* y la optimización del bus de Entrada/Salida (I/O).

El sistema implementa un pipeline donde la información nunca viaja al disco en texto claro, forzando una compresión en el **User Space** antes de delegar el control al Kernel de Linux.

---

## 🚀 Objetivos Técnicos
*   **Gestión de Memoria Eficiente:** Uso de un *Gap Buffer* para ediciones en tiempo real con complejidad $O(1)$ amortizado.
*   **Optimización de I/O:** Implementación de estrategias de escritura por bloques alineados (4KB) y mapeo de memoria directa (`mmap`).
*   **Compresión de Datos:** Integración obligatoria de un algoritmo *Run-Length Encoding* (RLE) para minimizar la carga en el bus de datos.
*   **Seguridad y Robustez:** Validación empírica de integridad de memoria mediante *Address Sanitizer*.

---

## 📁 Estructura del Proyecto
```text
.
├── include/           # Cabeceras (.h)
│   ├── editor.h       # Lógica del Gap Buffer
│   ├── compression.h  # Algoritmos RLE
│   ├── io_manager.h   # Estrategias de persistencia
│   └── file_format.h  # Especificación del formato binario EDTX
├── src/               # Código Fuente (.c)
│   ├── main.c         # Ciclo REPL y manejo de Raw Mode
│   ├── editor.c       # Implementación del Gap Buffer
│   ├── compression.c  # Codificador/Decodificador RLE
│   └── io_manager.c   # Implementación de Syscalls (write/mmap)
├── Makefile           # Automatización de compilación
└── README.md          # Documentación del proyecto
```

---

## 🛠️ Compilación y Ejecución

### Requisitos
*   Compilador `gcc`
*   Entorno Linux (Debian/Ubuntu recomendado)
*   Librería estándar de C

### Instalación
1.  Clona el repositorio:
    ```bash
    git clone https://github.com/SamuelCV12/Parcial-3-SeguridadyComprimir-SO.git
    cd Parcial-3-SeguridadyComprimir-SO
    ```
2.  Compila el proyecto usando el Makefile:
    ```bash
    make
    ```

### Uso
Para iniciar el editor con un archivo nuevo o existente:
```bash
./build/editor nombre_archivo
```

#### Banderas de Optimización
*   `--mmap`: Activa la estrategia de guardado mediante mapeo de memoria virtual (Zero-Copy).
*   `--raw`: (Solo para pruebas) Desactiva la compresión para comparar rendimiento.

---

## ⌨️ Controles del Editor
| Tecla | Acción |
| :--- | :--- |
| `Ctrl + S` | **Guardar:** Comprime y persiste en disco (.edtx) |
| `Ctrl + X` | **Salir:** Libera memoria y cierra el programa |
| `Flechas` | Navegación del cursor por el documento |
| `Backspace` | Borrado de caracteres en O(1) |

---

## 📊 Especificación del Formato .edtx
Los archivos generados por este editor utilizan un formato binario propietario con una cabecera empaquetada de **16 bytes**:
*   **Magic Number:** `EDTX` (Firma de validación).
*   **Metadata:** Versión, banderas de compresión y metadatos de color.
*   **Size:** Tamaño original para gestión precisa de `malloc` en descompresión.

---

## 📊 Profiling & Benchmarking
Para validar empíricamente la eficiencia del diseño, se pueden realizar pruebas de rendimiento comparando el **Enfoque Clásico** (sin optimización) vs el **Enfoque Propuesto** (EDTX).

### 1. Generar archivo de prueba (50 MB)
```bash
head -c 50000000 /dev/zero | tr '\0' 'A' > test_50mb.txt
```

### 2. Prueba de Enfoque Clásico (RAW)
Mide el tiempo y las llamadas al sistema sin compresión:
```bash
# Medir tiempos (User, Sys, Real)
time ./build/editor test_50mb.txt --raw

# Contar llamadas al sistema (Syscalls)
strace -c ./build/editor test_50mb.txt --raw
```

### 3. Prueba de Enfoque Propuesto (EDTX + RLE)
Mide la eficiencia con el pipeline de compresión e I/O optimizado:
```bash
# Medir tiempos
time ./build/editor test_50mb.txt

# Contar llamadas al sistema
strace -c ./build/editor test_50mb.txt
```

### 4. Resultados Esperados
*   **Reducción de Syscalls:** El número de llamadas a `write()` debe disminuir significativamente (~70% menos).
*   **Carga del Kernel:** El `sys time` debe ser menor debido a la reducción de interrupciones.
*   **Ahorro en Disco:** El archivo final debe ocupar una fracción del tamaño original (validar con `ls -lh`).

---
**Desarrollado para la asignatura de Sistemas Operativos.**  
*Enfoque en eficiencia, syscalls y gestión de recursos del Kernel.*
