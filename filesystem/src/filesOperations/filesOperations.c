#include "filesOperations.h"

int existe_archivo(const char* nombre_archivo) {
    FILE* archivo = fopen(nombre_archivo, "r");  // Intenta abrir el archivo en modo lectura

    if (archivo != NULL) {  // Si fopen devuelve un puntero distinto de NULL, el archivo existe
        fclose(archivo);  // No olvides cerrar el archivo si existe
        return 1;  // El archivo existe
    }

    return 0;  // El archivo no existe
}


// Crear archivo en el sistema
int crear_archivo(const char* nombre_archivo, uint32_t tamaño_inicial, void* datos) {
    // Verificar si el archivo ya existe
    if (existe_archivo(nombre_archivo)) {
        log_error(logger, "El archivo ya existe: %s", nombre_archivo);
        return -1;
    }

    int bloque_indice = 0;
    if (tamaño_inicial <= 0 || asignar_bloques(tamaño_inicial, blocks_path, nombre_archivo, datos, &bloque_indice) == -1) {
        return -1;
    }

    char files_dir[256];
    snprintf(files_dir, sizeof(files_dir), "%s/files", mount_dir);

    if (access(files_dir, F_OK) == -1) {
    // Si el directorio no existe, intenta crearlo
        if (mkdir(files_dir, 0777) == -1) {
            log_error(logger, "Error creando directorio /files");
            exit(1);
        } else {
            log_debug(logger, "Directorio /files creado correctamente.\n");
        }
    }

    // Crear el contenido del archivo de metadatos
    char metadatos_path[256];
    snprintf(metadatos_path, sizeof(metadatos_path), "%s/files/%s", mount_dir, nombre_archivo);

    FILE* archivo_metadata = fopen(metadatos_path, "w");
    if (!archivo_metadata) {
        log_error(logger, "No se pudo crear el archivo de metadatos: %s\n", metadatos_path);
        marcar_bloque(bloque_indice, 0); // Revertir el bloque marcado como ocupado
        return -1;
    }


    fprintf(archivo_metadata, "SIZE=%u\n", tamaño_inicial);
    fprintf(archivo_metadata, "INDEX_BLOCK=%d\n", bloque_indice);
    fclose(archivo_metadata);
    return 0;
}
