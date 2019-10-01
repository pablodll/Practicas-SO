#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	int i = 0;
	int c;
	
	c = fgetc(origin);
	
	while(i < nBytes && c != EOF){
	   if(fputc(c, destination) == EOF){
	      return -1;
	   }
	   c = fgetc(origin);
	   i++;
	}

	if(c == EOF) {
	   fputc(c, destination);
	}
	
	return i;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	char* string = NULL;
	char c;
	int i = 0;
	
	string = malloc(PATH_MAX);
	
	c = fgetc(file);
	
	while(c != '\0'){
	
	   string[i] = c;
	   i++;
	   c = fgetc(file);
	}
	
	string[i] = '\0';
	
	return string;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry * header = NULL;
	
	if(fread(nFiles, sizeof(int), 1, tarFile) == 0) return NULL;
	
	header = malloc(sizeof(stHeaderEntry) * (*nFiles));
	
	for(int i = 0; i < *nFiles; i++){
	   if((header[i].name = loadstr(tarFile)) == NULL){
	      free(header); 
	      return NULL;
	   }
	   fread(&header[i].size, sizeof(int), 1, tarFile);
	}
	
	return header;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
   FILE * tarFile = NULL;
   FILE * inputFile = NULL;
   stHeaderEntry * header = NULL;
   
   // Abrimos fichero tar
	if((tarFile = fopen(tarName, "wb")) == NULL) return EXIT_FAILURE;
   
   // Reservamos memoria para header
   header = malloc(sizeof(stHeaderEntry) * nFiles);
   
   // Calculamos los bits de desplazamiento
   int offData = sizeof(int) + nFiles * sizeof(unsigned int);
   
   // Inicializamos los nombres del header
   for(int i = 0; i < nFiles; i++){
      header[i].name = malloc(strlen(fileNames[i]) + 1);
      strcpy(header[i].name, fileNames[i]);
      offData += strlen(fileNames[i]) + 1; // Añadimos el tamaño del header al desplazamiento
   }
   
   // Nos posicionamos en la region de datos
   fseek(tarFile, offData, SEEK_CUR);
   
   // Copiamos los ficheros en el tar
   for(int i = 0; i < nFiles; i++){
      if((inputFile = fopen(fileNames[i], "rb")) == NULL){
         free(header);
         fclose(tarFile);
         return EXIT_FAILURE;
      }
      
      if((header[i].size = copynFile(inputFile, tarFile, INT_MAX)) == -1){
         free(header);
         fclose(tarFile);
         fclose(inputFile);
         return EXIT_FAILURE;
      }
      
      fclose(inputFile);
   }
   
   // Volvemos al byte 0 del tar
   rewind(tarFile);
   
   // Escribimos el numero de ficheros
   fwrite(&nFiles, sizeof(int), 1, tarFile);
   
   // Escribimos el header
   for(int i = 0; i < nFiles; i++){
      fwrite(header[i].name, strlen(header[i].name) + 1, 1, tarFile);
      fwrite(&header[i].size, sizeof(int), 1, tarFile);
   }
   
   free(header);
   fclose(tarFile);
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
   FILE * tarFile = NULL;
   FILE * outputFile = NULL;
   stHeaderEntry * header = NULL;
   int nFiles;
   
   if((tarFile = fopen(tarName, "rb")) == NULL) return EXIT_FAILURE;
   
   if((header = readHeader(tarFile, &nFiles)) == NULL) return EXIT_FAILURE;
   
   for(int i = 0; i < nFiles; i++){
      if((outputFile = fopen(header[i].name, "wb")) == NULL){
         free(header);   
         return EXIT_FAILURE;
      }
      copynFile(tarFile, outputFile, header[i].size);
      fclose(outputFile);
   }
	
	
	free(header);
	fclose(tarFile);
	return EXIT_SUCCESS;
}
