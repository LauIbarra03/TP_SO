#!/bin/bash

# Verificar que se hayan pasado los argumentos necesarios
if [ "$#" -ne 2 ]; then
  echo "Uso: $0 <nombre_archivo> <tam_proceso>"
  exit 1
fi

# Asignar los argumentos a variables
nombre_archivo=$1
tam_proceso=$2

# Detener el script si ocurre algún error
set -e

# Directorio base donde se encuentran los módulos
directorio_base="./"

# Lista de módulos en el orden requerido
modules=("filesystem" "memoria" "cpu" "kernel")

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
  
  gnome-terminal -- bash -c "
    cd \"$module_name\" &&
    make clean &&
    make all &&
    valgrind -s --leak-check=full --track-origins=yes ./bin/$module_name $params; exec bash"
}

# Borrar los logs antes de iniciar
borrar_logs

# Iterar y ejecutar cada módulo en una nueva terminal
for module in "${modules[@]}"; do
  if [ "$module" == "kernel" ]; then
    # Pasar los parámetros al módulo kernel
    run_in_new_terminal "$module" "$nombre_archivo $tam_proceso"
  else
    run_in_new_terminal "$module"
  fi
  
  # Esperar 7 segundos antes de continuar con el siguiente módulo
  echo "=== Esperando 7 segundos antes de continuar con el siguiente módulo... ==="
  sleep 7
done

echo "=== Todos los módulos se están ejecutando en terminales separadas ==="dos los módulos se están ejecutando en terminales separadas ==="