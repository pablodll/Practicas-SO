if [ -x mytar ]; then

   if [ -d ./tmp ]; then
      rm -r ./tmp
   fi   
   
   mkdir tmp
   cd tmp
   
   touch file1.txt
   echo "Hello world!" > file1.txt
   echo "Creado file1"

   touch file2.txt
   head /etc/passwd > file2.txt
   echo "Creado file2"

   touch file3.dat
   head -c 1024 /dev/urandom > file3.dat
   echo "Creado file3"

   ../mytar -c -f filetar.mtar file1.txt file2.txt file3.dat
   
   mkdir out
   mv filetar.mtar out
   cd out
   ../../mytar -x -f filetar.mtar
   
   diff file1.txt ../file1.txt
   if [ $? -eq 0 ]; then
      
      diff file2.txt ../file2.txt
      if [ $? -eq 0 ]; then
         
         diff file3.dat ../file3.dat
         if [ $? -eq 0 ]; then
            cd ../..
            echo "Correct"
            exit 0
         else
            cd ../..
            echo "file3.dat is not equal"
            exit 1
         fi
         
      else
          cd ../..
          echo "file2.txt is not equal"
          exit 1
      fi
      
   else
      cd ../..
      echo "file1.txt is not equal"
      exit 1
   fi
   
else
   echo "mytar executable does not exist"
fi
