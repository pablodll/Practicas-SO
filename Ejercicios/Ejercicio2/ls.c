//Autor: Pablo Daurell Marina
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]){
   
   DIR *dir;
   struct dirent *dp;
   struct stat st;
   char buf[PATH_MAX];
   
   if(argc == 1){ // Directorio actual
      if(getcwd(buf, PATH_MAX) == NULL){
         return -1;
      }
      dir = opendir(buf);
      printf("%s\n", buf);
   }
   else{ // Directorio externo
      dir = opendir(argv[1]);
      chdir(argv[1]);
   }
   
   if(dir == NULL){
      fprintf(stderr, "Error al abrir %s\n", buf);
   }
   else{
      
      while((dp = readdir(dir)) != NULL){
      
         if(stat(dp->d_name, &st) == -1){
            fprintf(stderr, "Error in %s\n", dp->d_name);
         }
         else{
            printf( (S_ISDIR(st.st_mode)) ? "d" : "-");
            printf( (st.st_mode & S_IRUSR) ? "r" : "-");
            printf( (st.st_mode & S_IWUSR) ? "w" : "-");
            printf( (st.st_mode & S_IXUSR) ? "x" : "-");
            printf( (st.st_mode & S_IRGRP) ? "r" : "-");
            printf( (st.st_mode & S_IWGRP) ? "w" : "-");
            printf( (st.st_mode & S_IXGRP) ? "x" : "-");
            printf( (st.st_mode & S_IROTH) ? "r" : "-");
            printf( (st.st_mode & S_IWOTH) ? "w" : "-");
            printf( (st.st_mode & S_IXOTH) ? "x" : "-");
            printf(" NLinks: %lu ", st.st_nlink);
            printf("User: %u ", st.st_uid);
            printf("Group: %u ", st.st_gid);
            printf("Tam: %lu ", st.st_size);
            printf("i-node: %lu ", st.st_ino);
            printf("%s ", dp->d_name);
            printf("%s", ctime(&st.st_mtime));
         }
      }
   }
   
   return 0;
}
