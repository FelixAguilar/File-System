# File-System
SO2 Sistema de archivos

Autores: Félix Aguilar, Adrián Bennasar, ÁLvaro Bueno
Equipo: NANI

1) Mejoras realizadas:

*ENTREGA 1:
   - Cuando se va a proceder a eliminar los bloques que están utilizados por el
     inodo, una vez que se ha eliminado el bloque de datos de un puntero y este 
     no contiene ningún otro puntero, no se realizan las iteraciones 
     pertenecientes a los bloques lógicos contenidos en este puntero. De esta
     forma, la ejecución evita iteraciones inútiles. 
     Ya que esta operación cambia el número de bloque lógico, se utiliza una 
     variable auxiliar para poder mostrar el número adecuado por pantalla.
    
*ENTREGA 2:
   - mi_touch: mejora que permite separar la creación de ficheros de la 
               creación de directorios.
  
   - mi_rmdir: permite separar la eliminación de archivos de la eliminación de 
               directorios.

   - mi_rm_r: permite la eliminación de elementos de forma recursiva.

   - mi_unlink_r: función que es llamada desde mi_rm_r.

   - mi_ls: permite la visualización de la infomación de un fichero, aparte
            de la de un directorio.
          
   - format_ls: subprograma para evitar redundancia de código.

   