echo "./mi_mkfs disco 100000\n"
./mi_mkfs disco 100000

echo "./mi_mkdir\n"
./mi_mkdir  

echo "./mi_mkdir disco 7 /\n" 
./mi_mkdir disco 7 /

echo "./mi_mkdir disco 6 dir1/\n"
./mi_mkdir disco 6 dir1/

echo "./mi_mkdir disco 6 /dir1/\n"  
./mi_mkdir disco 6 /dir1/ 

echo "./mi_mkdir disco 6 /dir1/dir2/\n"  
./mi_mkdir disco 6 /dir1/dir2/
 
echo "./mi_chmod\n" 
./mi_chmod 

echo "./mi_chmod disco 1 /dir1/dir2/\n" 
./mi_chmod disco 1 /dir1/dir2/ 
 
echo "./mi_touch disco 6 /dir1/dir2/fic1\n"  
./mi_touch disco 6 /dir1/dir2/fic1 

echo "./mi_chmod disco 2 /dir1/dir2/\n"  
./mi_chmod disco 2 /dir1/dir2/  

echo "./mi_touch disco 6 /dir1/dir2/fic1\n"  
./mi_touch disco 6 /dir1/dir2/fic1 

echo "./mi_chmod disco 6 /dir1/dir2/\n"
./mi_chmod disco 6 /dir1/dir2/
   
echo "./mi_touch disco 6 /dir1/dir2/fic2\n" 
./mi_touch disco 6 /dir1/dir2/fic2 

echo "./mi_touch disco 6 /dir1/dir2/fic1\n"
./mi_touch disco 6 /dir1/dir2/fic1

echo "./mi_ls disco /\n"
./mi_ls disco /

echo "./mi_stat disco /dir1/\n"
./mi_stat disco /dir1/

echo "./mi_ls disco /dir1/\n" 
./mi_ls disco /dir1/

echo "./mi_stat disco /dir1/dir2/\n"
./mi_stat disco /dir1/dir2/

echo "./mi_ls disco /dir1/dir2/\n"
./mi_ls disco /dir1/dir2/

echo "./mi_ls disco /dir1/dir3/\n"
./mi_ls disco /dir1/dir3/

echo "./mi_touch disco 6 /dir1/dir2/fic1\n"  
./mi_touch disco 6 /dir1/dir2/fic1 

echo "./mi_mkdir disco 6 /dir1/dir2/fic1/dir3/\n"
./mi_mkdir disco 6 /dir1/dir2/fic1/dir3/

echo "./mi_touch disco 6 /dir1/dir2/dir3/fic1\n" 
./mi_touch disco 6 /dir1/dir2/dir3/fic1 

echo "./mi_mkdir disco 9 /dir5/\n"
./mi_mkdir disco 9 /dir5/




