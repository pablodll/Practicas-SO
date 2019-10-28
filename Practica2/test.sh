# Ela Shepherd Arevalo
# Pablo Daurell Marina

# Si existe temp, lo borramos
if [ -d temp/ ]; then
   rm -r temp/
fi

# Creamos temp y copiamos los archivos
mkdir temp
cp ./src/fuseLib.c mount-point
cp ./src/myFS.h mount-point
cp ./src/fuseLib.c temp
cp ./src/myFS.h temp

# Auditamos el disco y comprobamos que todo se ha copiado bien
./my-fsck virtual-disk

diff ./src/fuseLib.c ./mount-point/fuseLib.c
if [ $? -eq 0 ]; then
   echo "fuseLib.c copied correctly"   
else 
   echo "fuseLib.c copy Error"
fi

diff ./src/myFS.h ./mount-point/myFS.h
if [ $? -eq 0 ]; then
   echo "myFS.h copied correctly"   
else 
   echo "myFS.h copy Error"
fi

# Truncamos fuseLib.c (con un bloque menos), auditamos y comprobamos que todo ha ido bien
truncate -s -4096 ./temp/fuseLib.c
truncate -s -4096 ./mount-point/fuseLib.c

./my-fsck virtual-disk

diff ./temp/fuseLib.c ./mount-point/fuseLib.c
if [ $? -eq 0 ]; then
   echo "fuseLib.c truncated correctly"   
else 
   echo "fuseLib.c truncate Error"
fi

# Copiamos un nuevo fichero, auditamos y comprobamos que todo ha ido bien
cp ./src/MyFileSystem.c mount-point

./my-fsck virtual-disk

diff ./src/MyFileSystem.c ./mount-point/MyFileSystem.c
if [ $? -eq 0 ]; then
   echo "MyFileSystem.c copied correctly"   
else 
   echo "MyFileSystem.c copy Error"
fi

# Truncamos myFS.h (con un bloque mas), auditamos y comprobamos que todo ha ido bien
truncate -s +4096 ./temp/myFS.h
truncate -s +4096 ./mount-point/myFS.h

./my-fsck virtual-disk

diff ./temp/myFS.h ./mount-point/myFS.h
if [ $? -eq 0 ]; then
   echo "myFS.h truncated correctly"   
else 
   echo "myFS.h truncate Error"
fi
