echo "./mi_mkfs disco 100000"
./mi_mkfs disco 100000

echo "./mi_touch disco 6 /fichero"
./mi_touch disco 6 /fichero

echo "./mi_ls disco /"
./mi_ls disco /

echo "./mi_escribir  disco /fichero “hola que tal” 5120"
./mi_escribir  disco /fichero "hola que tal" 5120

echo " ./mi_ls disco /"
./mi_ls disco /

echo "./leer_sf disco"
./leer_sf disco

echo "./mi_chmod disco 4 /fichero"
./mi_chmod disco 4 /fichero

echo "./mi_escribir  disco /fichero “estoy estupendamente” 256000"
./mi_escribir  disco /fichero "estoy estupendamente" 256000

echo "./mi_ls disco /fichero"
./mi_ls disco /fichero

echo "./mi_mkdir disco 6 /dir1/"
./mi_mkdir disco 6 /dir1/

echo "./mi_touch disco 6 /dir1/fic1"
./mi_touch disco 6 /dir1/fic1

echo "./mi_escribir disco /dir1/fic1 hola1  256000"
./mi_escribir disco /dir1/fic1 hola1 256000

echo "./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1

echo "sleep 2"
sleep 2

echo "./mi_escribir disco /dir1/fic1 hola2 5120" 
./mi_escribir disco /dir1/fic1 hola2  5120

echo "./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1

echo "sleep 2"
sleep 2 

echo "./mi_escribir disco /dir1/fic1 hola3 5200"
./mi_escribir disco /dir1/fic1 hola3 5200

echo "./mi_stat disco /dir1/fic1\n"
./mi_stat disco /dir1/fic1

echo " sleep 2"
sleep 2

echo "./mi_escribir disco /dir1/fic1 hola4 256010"
./mi_escribir disco /dir1/fic1 hola4 256010

echo "./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1

echo "./mi_touch disco 6 /dir1/fic2"
./mi_touch disco 6 /dir1/fic2

echo "./mi_escribir disco /dir1/fic2 \"$(cat texto2.txt)\" 1000"
./mi_escribir disco /dir1/fic2 "$(cat texto2.txt)" 1000

echo "./mi_cat disco /dir1/fic2"
./mi_cat disco /dir1/fic2

echo "./mi_escribir disco /dir1/fic2 “******************************” 10000"
./mi_escribir disco /dir1/fic2 "******************************" 10000

echo "./mi_ls disco /dir1/"
./mi_ls disco /dir1/

echo "./mi_cat disco /dir1/fic2"
./mi_cat disco /dir1/fic2

echo "./mi_touch disco 6 /dir1/fic3"
./mi_touch disco 6 /dir1/fic3

echo "./mi_escribir_varios disco /dir1/fic3 “--texto repetido en 10 bloques--” 0"
./mi_escribir_varios disco /dir1/fic3 "--texto repetido en 10 bloques--" 0

echo "./mi_stat disco /dir1/fic3"
./mi_stat disco /dir1/fic3

echo "./mi_cat disco /dir1/fic3"
./mi_cat disco /dir1/fic3 

































