#!/bin/bash

# Verificar que se hayan pasado los argumentos necesarios
if [ "$#" -lt 2 ]; then
  echo "Uso: $0 <nombre_archivo> <tam_proceso> [config_opcional]"
  exit 1
fi

# Asignar los argumentos a variables
nombre_archivo=$1
tam_proceso=$2
config_opcional=${3:-default} # Tercer parámetro opcional con valor por defecto

# Detener el script si ocurre algún error
set -e

# Directorio base donde se encuentran los módulos
directorio_base="./"

# Lista de módulos en el orden requerido
modules=("memoria" "cpu" "kernel")

# Función para modificar configuraciones según el archivo recibido y el caso opcional
modificar_config() {
  local archivo=$1
  local caso=$2

   case $archivo in
    "PLANI_PROC")
      case $caso in
        "FIFO")
          sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=FIFO/' kernel/kernel.config
          ;;
        "PRIORIDADES")
          sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=PRIORIDADES/' kernel/kernel.config
          ;;
        "CMN")
          sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=CMN/' kernel/kernel.config
          ;;
        *)
          echo "Caso no reconocido, parametros posibles: FIFO | PRIORIDADES | CMN"
          exit 1
          ;;
      esac
      sed -i 's/^QUANTUM=.*/QUANTUM=875/' kernel/kernel.config
      sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=1024/' memoria/memoria.config
      sed -i 's/^RETARDO_RESPUESTA=.*/RETARDO_RESPUESTA=500/' memoria/memoria.config
      sed -i 's/^ESQUEMA=.*/ESQUEMA=FIJAS/' memoria/memoria.config
      sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=FIRST/' memoria/memoria.config
      sed -i 's/^PARTICIONES=.*/PARTICIONES=[32,32,32,32,32,32,32,32]/' memoria/memoria.config
      sed -i 's/^BLOCK_SIZE=.*/BLOCK_SIZE=16/' filesystem/filesystem.config
      sed -i 's/^BLOCK_COUNT=.*/BLOCK_COUNT=1024/' filesystem/filesystem.config
      sed -i 's/^RETARDO_ACCESO_BLOQUE=.*/RETARDO_ACCESO_BLOQUE=2500/' filesystem/filesystem.config
      ;;

    "RECURSOS_MUTEX_PROC")
      case $caso in
        "750")
          sed -i 's/^QUANTUM=.*/QUANTUM=750/' kernel/kernel.config
          ;;
        "150")
          sed -i 's/^QUANTUM=.*/QUANTUM=150/' kernel/kernel.config
          ;;
        *)
          echo "Caso no reconocido, parametros posibles: 750 | 150"
          exit 1
          ;;
      esac
      sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=CMN/' kernel/kernel.config
      sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=1024/' memoria/memoria.config
      sed -i 's/^RETARDO_RESPUESTA=.*/RETARDO_RESPUESTA=20/' memoria/memoria.config
      sed -i 's/^ESQUEMA=.*/ESQUEMA=FIJAS/' memoria/memoria.config
      sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=FIRST/' memoria/memoria.config
      sed -i 's/^PARTICIONES=.*/PARTICIONES=[32,32,32,32,32,32,32,32]/' memoria/memoria.config
      sed -i 's/^BLOCK_SIZE=.*/BLOCK_SIZE=16/' filesystem/filesystem.config
      sed -i 's/^BLOCK_COUNT=.*/BLOCK_COUNT=1024/' filesystem/filesystem.config
      sed -i 's/^RETARDO_ACCESO_BLOQUE=.*/RETARDO_ACCESO_BLOQUE=2500/' filesystem/filesystem.config
      ;;

    "MEM_FIJA_BASE")
      case $caso in
        "FIRST")
          sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=FIRST/' memoria/memoria.config
          ;;
        "BEST")
          sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=BEST/' memoria/memoria.config
          ;;
        "WORST")
          sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=WORST/' memoria/memoria.config
          ;;
        *)
          echo "Caso: '$caso' no reconocido, parametros posibles: FIRST | BEST | WORST"
          exit 1
          ;;
      esac
      sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=CMN/' kernel/kernel.config
      sed -i 's/^QUANTUM=.*/QUANTUM=500/' kernel/kernel.config
      sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=256/' memoria/memoria.config
      sed -i 's/^RETARDO_RESPUESTA=.*/RETARDO_RESPUESTA=200/' memoria/memoria.config
      sed -i 's/^ESQUEMA=.*/ESQUEMA=FIJAS/' memoria/memoria.config
      sed -i 's/^PARTICIONES=.*/PARTICIONES=[32,16,64,128,16]/' memoria/memoria.config
      sed -i 's/^BLOCK_SIZE=.*/BLOCK_SIZE=16/' filesystem/filesystem.config
      sed -i 's/^BLOCK_COUNT=.*/BLOCK_COUNT=1024/' filesystem/filesystem.config
      sed -i 's/^RETARDO_ACCESO_BLOQUE=.*/RETARDO_ACCESO_BLOQUE=2500/' filesystem/filesystem.config
      ;;

    "MEM_DINAMICA_BASE")
      sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=CMN/' kernel/kernel.config
      sed -i 's/^QUANTUM=.*/QUANTUM=500/' kernel/kernel.config
      sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=1024/' memoria/memoria.config
      sed -i 's/^RETARDO_RESPUESTA=.*/RETARDO_RESPUESTA=200/' memoria/memoria.config
      sed -i 's/^ESQUEMA=.*/ESQUEMA=DINAMICAS/' memoria/memoria.config
      sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=BEST/' memoria/memoria.config
      sed -i 's/^BLOCK_SIZE=.*/BLOCK_SIZE=32/' filesystem/filesystem.config
      sed -i 's/^BLOCK_COUNT=.*/BLOCK_COUNT=4096/' filesystem/filesystem.config
      sed -i 's/^RETARDO_ACCESO_BLOQUE=.*/RETARDO_ACCESO_BLOQUE=2500/' filesystem/filesystem.config
      ;;
    
    "PRUEBA_FS")
      sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=CMN/' kernel/kernel.config
      sed -i 's/^QUANTUM=.*/QUANTUM=25/' kernel/kernel.config
      sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=2048/' memoria/memoria.config
      sed -i 's/^RETARDO_RESPUESTA=.*/RETARDO_RESPUESTA=10/' memoria/memoria.config
      sed -i 's/^ESQUEMA=.*/ESQUEMA=DINAMICAS/' memoria/memoria.config
      sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=BEST/' memoria/memoria.config
      sed -i 's/^BLOCK_SIZE=.*/BLOCK_SIZE=32/' filesystem/filesystem.config
      sed -i 's/^BLOCK_COUNT=.*/BLOCK_COUNT=200/' filesystem/filesystem.config
      sed -i 's/^RETARDO_ACCESO_BLOQUE=.*/RETARDO_ACCESO_BLOQUE=25/' filesystem/filesystem.config
      ;;
    
    "THE_EMPTINESS_MACHINE")
      sed -i 's/^ALGORITMO_PLANIFICACION=.*/ALGORITMO_PLANIFICACION=CMN/' kernel/kernel.config
      sed -i 's/^QUANTUM=.*/QUANTUM=125/' kernel/kernel.config
      sed -i 's/^TAM_MEMORIA=.*/TAM_MEMORIA=8192/' memoria/memoria.config
      sed -i 's/^RETARDO_RESPUESTA=.*/RETARDO_RESPUESTA=50/' memoria/memoria.config
      sed -i 's/^ESQUEMA=.*/ESQUEMA=DINAMICAS/' memoria/memoria.config
      sed -i 's/^ALGORITMO_BUSQUEDA=.*/ALGORITMO_BUSQUEDA=BEST/' memoria/memoria.config
      sed -i 's/^BLOCK_SIZE=.*/BLOCK_SIZE=64/' filesystem/filesystem.config
      sed -i 's/^BLOCK_COUNT=.*/BLOCK_COUNT=1024/' filesystem/filesystem.config
      sed -i 's/^RETARDO_ACCESO_BLOQUE=.*/RETARDO_ACCESO_BLOQUE=100/' filesystem/filesystem.config
      ;;
    
    *)
      echo "Archivo: $archivo es distintos a los test, se mantiene el config actual"
      ;;
  esac
}

# Modificar configuraciones
modificar_config "$nombre_archivo" "$config_opcional"

# Función para borrar logs
borrar_logs() {
  echo "=== Eliminando logs de módulos ==="
  local directorio_actual=$(pwd)

  for module in "${modules[@]}"; do
    local ruta_modulo="${directorio_base}${module}"
    
    if [ -d "$ruta_modulo" ]; then
      cd "$ruta_modulo" || exit

      local archivo_log="${module}.log"
      if [ -f "$archivo_log" ]; then
        echo "Eliminando $archivo_log..."
        rm "$archivo_log"
      else
        echo "Archivo $archivo_log no encontrado. Nada que eliminar."
      fi

      cd "$directorio_actual" || exit
    else
      echo "El directorio $ruta_modulo no existe. Saltando..."
    fi
  done
  echo "=== Logs eliminados ==="
}

# Función para abrir una nueva terminal y ejecutar un módulo con Valgrind
run_in_new_terminal() {
  local module_name=$1
  local params=$2
  echo "=== Abriendo terminal para el módulo: $module_name ==="
  
  xfce4-terminal --hold --command "bash -c '
  cd \"$module_name\" && 
  make clean          && 
  make all            && 
  valgrind -s --leak-check=full --track-origins=yes ./bin/$module_name $params; 
  exec bash'" &
}


# Borrar los logs antes de iniciar
borrar_logs

levantar_htop() {
  echo "=== Abriendo HTOP ==="
  
  xfce4-terminal --hold --command "bash -c '
  htop; 
  exec bash'" &
}

levantar_htop

SLEEP_TIME=11
# Iterar y ejecutar cada módulo en una nueva terminal
for module in "${modules[@]}"; do
  if [ "$module" == "kernel" ]; then
    # Pasar los parámetros al módulo kernel
    run_in_new_terminal "$module" "$nombre_archivo $tam_proceso"
  else
    run_in_new_terminal "$module"
  fi
  
  # Esperar $valor_sleep segundos antes de continuar con el siguiente módulo
  echo "=== Esperando $SLEEP_TIME segundos antes de continuar con el siguiente módulo... ==="
  sleep $SLEEP_TIME
done

echo "=== Todos los módulos se están ejecutando en terminales separadas ==="





