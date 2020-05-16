# File-System
SO2 Sistema de archivos

Autores: Félix Aguilar, Adrián Bennasar, ÁLvaro Bueno
Equipo: NANI

1) Mejoras realizadas:
   - Cuando se va a proceder a eliminar los bloques que están utilizados por el
     inodo, una vez que se ha eliminado el bloque de datos de un puntero y este 
     no contiene ningún otro puntero, no se realizan las iteraciones 
     pertenecientes a los bloques lógicos contenidos en este puntero. De esta
     forma, la ejecución evita iteraciones inútiles. 
     Ya que esta operación cambia el número de bloque lógico, se utiliza una 
     variable auxiliar para poder mostrar el número adecuado por pantalla.
    
