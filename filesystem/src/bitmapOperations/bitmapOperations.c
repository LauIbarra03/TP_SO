#include "bitmapOperations.h"

uint8_t* bitmap = NULL; // Declaración del puntero para el bitmap

void leer_primer_byte(const char* bitmap_path) {
    FILE* archivo = fopen(bitmap_path, "rb"); // Abrir archivo en modo binario
    if (!archivo) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    unsigned char byte; // Variable para almacenar el primer byte

    // Leer el primer byte del archivo
    if (fread(&byte, sizeof(unsigned char), 1, archivo) != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        exit(1);
    }

    // Mostrar el byte como bits
    printf("Primer byte (8 bits): ");
    for (int i = 7; i >= 0; i--) {
        printf("%u", (byte >> i) & 1); // Desplazar y aislar cada bit
    }
    printf("\n");

    fclose(archivo); // Cerrar el archivo
}

void cargar_bitmap() {
    // Proteger acceso al archivo y al bitmap con mutex
    pthread_mutex_lock(&bitmap_mutex);
    pthread_mutex_lock(&file_bitmap_mutex);
    bitmap = malloc(bitmap_size); 
    if (!bitmap) {
        perror("Error al asignar memoria para bitmap");
        exit(1);
    }
    // Inicializar el bitmap con ceros (opcional, pero recomendado)
    memset(bitmap, 0, bitmap_size);
    // Abrir el archivo bitmap.dat en modo binario
    FILE* bitmap_file = fopen(bitmap_path, "rb");
    if (!bitmap_file) {
        perror("Error al abrir bitmap.dat");
        pthread_mutex_unlock(&file_bitmap_mutex);
        pthread_mutex_unlock(&bitmap_mutex);
        exit(EXIT_FAILURE);
    }
    // Validar el tamaño del archivo para asegurarse de que coincide con bitmap_size
    fseek(bitmap_file, 0, SEEK_END);
    long file_size = ftell(bitmap_file);
    rewind(bitmap_file);
    
    if (file_size != bitmap_size) {
        fprintf(stderr, "Tamaño inesperado del archivo bitmap: %ld bytes (esperado: %d bytes)\n", file_size, bitmap_size);
        fclose(bitmap_file);
        pthread_mutex_unlock(&file_bitmap_mutex);
        pthread_mutex_unlock(&bitmap_mutex);
        exit(EXIT_FAILURE);
    }
    // Leer el contenido del archivo en el arreglo bitmap
    size_t bytes_leidos = fread(bitmap, 1, bitmap_size, bitmap_file);
    if (bytes_leidos != bitmap_size) {
        fprintf(stderr, "Error al leer el bitmap: se leyeron %zu bytes en lugar de %d\n", bytes_leidos, bitmap_size);
        fclose(bitmap_file);
        pthread_mutex_unlock(&file_bitmap_mutex);
        pthread_mutex_unlock(&bitmap_mutex);
        exit(EXIT_FAILURE);
    }
    // Cerrar el archivo después de leer
    fclose(bitmap_file);
    // Liberar los mutex
    pthread_mutex_unlock(&file_bitmap_mutex);
    pthread_mutex_unlock(&bitmap_mutex);
}
// Persistir el estado actual del bitmap
void persistir_bitmap() {
    pthread_mutex_lock(&bitmap_mutex);
    pthread_mutex_lock(&file_bitmap_mutex);

    FILE* bitmap_file = fopen(bitmap_path, "wb");
    if (!bitmap_file) {
        perror("Error al guardar bitmap.dat");
        pthread_mutex_unlock(&bitmap_mutex);
        pthread_mutex_unlock(&file_bitmap_mutex);
        exit(1);  // O maneja el error de forma más controlada
    }

    size_t bytes_escritos = fwrite(bitmap, 1, bitmap_size, bitmap_file);
    if (bytes_escritos != bitmap_size) {
        fprintf(stderr, "Error al escribir el bitmap\n");
        fclose(bitmap_file);
        pthread_mutex_unlock(&bitmap_mutex);
        pthread_mutex_unlock(&file_bitmap_mutex);
        exit(1);  // Manejo de error
    }

    fclose(bitmap_file);

    //leer_primer_byte(bitmap_path);

    pthread_mutex_unlock(&bitmap_mutex);
    pthread_mutex_unlock(&file_bitmap_mutex);
}


// Buscar el siguiente bloque libre en el bitmap
int buscar_bloque_libre(uint32_t* byte_index, uint32_t* bit_position) {
    uint32_t bloque_libre = (uint32_t)-1;

    int retardo_acceso_bloque = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");

    usleep(retardo_acceso_bloque * 1000);

    pthread_mutex_lock(&bitmap_mutex); // Bloqueo el acceso al bitmap

    for (int byte = 0; byte < bitmap_size; byte++) {
        for (int bit = 0; bit < 8; bit++) {
            if (!(bitmap[byte] & (1 << bit))) { // Si el bit está libre
                *byte_index = byte;     // Índice del byte encontrado
                *bit_position = bit;    // Posición del bit dentro del byte
                uint32_t bloque_libre = (uint32_t)(byte * 8 + bit); // Índice del bloque
                pthread_mutex_unlock(&bitmap_mutex); // Desbloqueo y salgo
                return bloque_libre;
            }
        }
    }

    pthread_mutex_unlock(&bitmap_mutex); // Desbloqueo si no encontré bloque libre
    return -1; // No hay bloques libres
}

// Marcar un bloque como ocupado o libre
void marcar_bloque(uint32_t bloque, int ocupado) {
    int byte = bloque / 8;
    int bit = bloque % 8;

    pthread_mutex_lock(&bitmap_mutex); // Bloqueo el acceso al bitmap
    if (ocupado) {
        bitmap[byte] |= (1 << bit); // Marcar bit como 1
    } else {
        bitmap[byte] &= ~(1 << bit); // Marcar bit como 0
    }
    pthread_mutex_unlock(&bitmap_mutex); // Desbloqueo el bitmap
}
// Inicializar un archivo y sus metadatos

// Asignar bloques a un archivo y actualizar su bloque índice
int asignar_bloques(uint32_t size, const char* blocks_path, const char* new_file_path, void* datos, int* index_block) { 
    uint32_t bloques_necesarios = (size + block_size - 1) / block_size;  // Número de bloques necesarios
    log_info(logger, "cantidad de bloques necesarios: %d\n", bloques_necesarios + 1);

    int tiempo_retardo_peticiones = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");

    size_t cantidad_bloques_libres = contar_bloques_libres();

    if(cantidad_bloques_libres < bloques_necesarios + 1) {
        log_error(logger, "No hay bloques disponibles para crear el archivo");
        return -1;
    }

    uint32_t byte_index, bit_position;
    int bloque_indice = buscar_bloque_libre(&byte_index, &bit_position);

    *index_block = bloque_indice;
    
    if (bloque_indice == -1) {
        log_error(logger, "No hay bloques disponibles para crear el archivo.");
        return -1;
    }

    // Marcar el bloque índice como ocupado
    marcar_bloque(bloque_indice, 1);

    cantidad_bloques_libres = contar_bloques_libres();

    log_info(logger, "## Archivo Creado: <%s> - Tamaño: <%d>\n", new_file_path, size);

    log_info(logger, "## Bloque asignado: <%d> - Archivo: <%s> - Bloques Libres: <%d>", bloque_indice, new_file_path, cantidad_bloques_libres);
    log_info(logger, "## Acceso Bloque - Archivo: <%s> - Tipo Bloque: <ÍNDICE> - Bloque File System <%d>", new_file_path, bloque_indice);
    usleep(tiempo_retardo_peticiones * 1000);

    pthread_mutex_lock(&file_bloques_mutex); // Bloquear acceso a bloques.dat
    FILE* blocks_file = fopen(blocks_path, "rb+");
    if (!blocks_file) {
        perror("Error al abrir bloques.dat");
        pthread_mutex_unlock(&file_bloques_mutex);
        exit(1);
    }

    // Reservar espacio para almacenar los punteros a los bloques de datos
    uint32_t* punteros_bloques = calloc(bloques_necesarios, sizeof(uint32_t));
    if (!punteros_bloques) {
        perror("Error al asignar memoria para punteros a bloques");
        fclose(blocks_file);
        pthread_mutex_unlock(&file_bloques_mutex);
        exit(1);
    }

    uint32_t* data_ptr = (uint32_t*) datos; // Cast del puntero de datos a uint32_t*
    uint32_t remaining_words = size / sizeof(uint32_t); // Cantidad total de palabras a escribir

    for (uint32_t i = 0; i < bloques_necesarios; i++) {
        uint32_t byte_index, bit_position;
        int bloque_libre = buscar_bloque_libre(&byte_index, &bit_position);
        if (bloque_libre == -1) {
            free(punteros_bloques);
            fclose(blocks_file);
            pthread_mutex_unlock(&file_bloques_mutex);
            return -1;
        }

        marcar_bloque(bloque_libre, 1); // Marcar el bloque como ocupado
        punteros_bloques[i] = bloque_libre; // Registrar el puntero al bloque asignado

        // Calcular cuántos datos escribir en este bloque
        size_t words_to_write = remaining_words < block_size / sizeof(uint32_t) ? remaining_words : block_size / sizeof(uint32_t);

        cantidad_bloques_libres = contar_bloques_libres();
    
        log_info(logger, "## Bloque asignado: <%d> - Archivo: <%s> - Bloques Libres: <%d>", bloque_libre, new_file_path, cantidad_bloques_libres);

        log_info(logger, "## Acceso Bloque - Archivo: <%s> - Tipo Bloque: <DATOS> - Bloque File System <%d>", new_file_path, bloque_libre);
        usleep(tiempo_retardo_peticiones * 1000);
        
        // Escribir los datos en el bloque
        fseek(blocks_file, bloque_libre * block_size, SEEK_SET);
        if (fwrite(data_ptr, sizeof(uint32_t), words_to_write, blocks_file) != words_to_write) {
            perror("Error al escribir datos en bloques.dat");
            free(punteros_bloques);
            fclose(blocks_file);
            pthread_mutex_unlock(&file_bloques_mutex);
            exit(1);
        }

        // Actualizar puntero de datos y contador de palabras restantes
        data_ptr += words_to_write;
        remaining_words -= words_to_write;
    }


    // Escribir los punteros al bloque índice
    fseek(blocks_file, bloque_indice * block_size, SEEK_SET);
    if (fwrite(punteros_bloques, sizeof(uint32_t), bloques_necesarios, blocks_file) != bloques_necesarios) {
        perror("Error al escribir punteros en el bloque índice");
        free(punteros_bloques);
        fclose(blocks_file);
        pthread_mutex_unlock(&file_bloques_mutex);
        exit(1); 
    }

    // Liberar recursos
    free(punteros_bloques);
    fclose(blocks_file);
    pthread_mutex_unlock(&file_bloques_mutex);
    persistir_bitmap();
    log_info(logger, "Asignación completada para el archivo %s\n", new_file_path); 
    return 1;
}



size_t contar_bloques_libres() {
    size_t bloques_libres = 0;
    
    // Recorremos cada byte del bitmap
    for (size_t i = 0; i < bitmap_size; i++) {
        uint8_t byte = bitmap[i];
        
        // Recorremos cada bit del byte
        for (int j = 0; j < 8; j++) {
            // Comprobamos si el bit en la posición j del byte es 0 (bloque libre)
            if ((byte & (1 << j)) == 0) {
                bloques_libres++;
            }
        }
    }

    return bloques_libres;
}
