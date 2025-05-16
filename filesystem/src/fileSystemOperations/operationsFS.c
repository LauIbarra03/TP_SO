#include "fileSystemOperations/operationsFS.h"
#include "filesystem.h"
#include <time.h>
#include <sys/time.h> 
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h> // Asegúrate de incluir las commons

#define MAX_PATH_LENGTH 256

uint32_t block_size;
uint32_t block_count;
const char* mount_dir;
size_t bitmap_size;
pthread_mutex_t bitmap_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_bitmap_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_bloques_mutex = PTHREAD_MUTEX_INITIALIZER;
extern t_log *logger;

char bitmap_path[MAX_PATH_LENGTH], blocks_path[MAX_PATH_LENGTH], files_dir[MAX_PATH_LENGTH];

// Inicializar el sistema de archivos
void inicializar_fs() {
    block_size = config_get_int_value(config, "BLOCK_SIZE");
    block_count = config_get_int_value(config, "BLOCK_COUNT");
    mount_dir = config_get_string_value(config, "MOUNT_DIR");
    log_info(logger, "path del config: %s", mount_dir);
    bitmap_size  = (block_count + 7) / 8;

    snprintf(bitmap_path, sizeof(bitmap_path), "%s/bitmap.dat", mount_dir);
    log_info(logger, "Path del bitmap: %s", bitmap_path);
    snprintf(blocks_path, sizeof(blocks_path), "%s/bloques.dat", mount_dir);
    snprintf(files_dir, sizeof(files_dir), "%s/files", mount_dir);

    DIR* dir = opendir(mount_dir);
    
    if (dir) {
        // Si el directorio existe, lo cerramos
        closedir(dir);
        log_info(logger, "El directorio ya existe: %s", mount_dir);
    } else {
        // Si no existe, lo creamos
        if (mkdir(mount_dir, 0777) == 0) {
            printf("Directorio creado: %s\n", mount_dir);
        } else {
            perror("Error creando el directorio");
            exit(1);
        }
    }

    // Crear y cargar bitmap
    crear_archivo_si_no_existe(bitmap_path, bitmap_size, 0x00);
    cargar_bitmap();

    // Crear bloques.dat
    crear_archivo_si_no_existe(blocks_path, block_count * block_size, 0x00);

    if(crear_directorio_recursivo(files_dir) != 0) {
        exit(1);
    }
}

int crear_directorio_recursivo(const char* ruta) {
    char* ruta_copy = strdup(ruta);  // Copia dinámica para evitar límites
    if (ruta_copy == NULL) {
        perror("Error duplicando la ruta");
        return -1;
    }

    for (char* p = ruta_copy + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';  // Termina la ruta aquí temporalmente
            if (access(ruta_copy, F_OK) == -1) {  // Verifica si existe
                if (mkdir(ruta_copy, 0777) == -1 && errno != EEXIST) {
                    perror("Error creando directorio intermedio");
                    free(ruta_copy);
                    return -1;
                }
            }
            *p = '/';  // Restaura el carácter original
        }
    }

    // Crear el directorio final
    if (access(ruta_copy, F_OK) == -1) {
        if (mkdir(ruta_copy, 0777) == -1 && errno != EEXIST) {
            perror("Error creando directorio final");
            free(ruta_copy);
            return -1;
        }
    }

    free(ruta_copy);
    return 0;
}

void crear_archivo_si_no_existe(const char* path, size_t size, char fill_byte) {
    struct stat st;
    
    // Verifica si el archivo existe
    if (stat(path, &st) == 0) {
        if ((size_t)st.st_size != size) {
            log_warning(logger, "El archivo %s existe pero tiene un tamaño inesperado (%ld bytes en lugar de %zu). Será recreado.", path, st.st_size, size);
            if (remove(path) != 0) {
                log_error(logger, "No se pudo eliminar el archivo %s. No se continuará.", path);
                return;
            }
        } else {
            log_info(logger, "El archivo ya existe con el tamaño esperado: %s. No se hará nada.", path);
            return;
        }
    }

    // Crear y rellenar el archivo con el byte especificado
    FILE* file = fopen(path, "wb");
    if (!file) {
        log_error(logger, "Error creando o abriendo el archivo: %s", path);
        return;
    }

    char buffer[1024];
    memset(buffer, fill_byte, sizeof(buffer));

    for (size_t i = 0; i < size; i += sizeof(buffer)) {
        size_t bytes_to_write = (size - i < sizeof(buffer)) ? size - i : sizeof(buffer);
        if (fwrite(buffer, 1, bytes_to_write, file) != bytes_to_write) {
            log_error(logger, "Error escribiendo en el archivo: %s", path);
            fclose(file);
            return;
        }
    }

    log_info(logger, "Archivo creado e inicializado con byte de relleno: %s", path);

    if (fclose(file) != 0) {
        log_error(logger, "Error cerrando el archivo: %s", path);
        return;
    }
}


op_code execute_dump_memory(t_dump_context* contexto) {
    char nombre_archivo[MAX_PATH_LENGTH];
    generar_nombre_archivo(contexto->pid, contexto->tid, nombre_archivo);
    log_info(logger, "Nombre del archivo a crear: %s", nombre_archivo);

    uint32_t tamanio = contexto->tamanio_proceso;
    void* datos = contexto->datos;

    int retorno_creacion_archivo = crear_archivo(nombre_archivo, tamanio, datos);

    if(retorno_creacion_archivo != 0) {
        log_error(logger, "Error en retorno creacion archivo");
        return ERROR;
    }
    log_info(logger, "## Fin de solicitud - Archivo: <%s>\n", nombre_archivo);

    return OK;    
}

void generar_nombre_archivo(uint32_t pid, uint32_t tid, char nombre[MAX_PATH_LENGTH]) {
    // Obtener el timestamp actual
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Obtener segundos y formatear con strftime
    struct tm *tiempo_local = localtime(&tv.tv_sec);

    char timestamp[64];
    if (strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tiempo_local) == 0) {
        fprintf(stderr, "Error formateando la hora.\n");
        return;
    }

    // Agregar los milisegundos al timestamp
    char timestamp_completo[64];
    snprintf(timestamp_completo, sizeof(timestamp_completo), "%s:%03ld", timestamp, tv.tv_usec / 1000);

    // Crear el nombre del archivo concatenando pid, tid y timestamp
    snprintf(nombre, MAX_PATH_LENGTH, "%d-%d-%s.dmp", pid, tid, timestamp_completo);
}
