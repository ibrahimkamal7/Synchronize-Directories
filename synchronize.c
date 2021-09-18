#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>

const int MAXLINE = 1024;

//counts of the number of files in direactory a and b
int count_A = 0;
int count_B = 0;

int search(char *fileName, char files[MAXLINE][MAXLINE], int size);
void replicateFile(char *source, char *destination);
char* buildPath(char *directoryName, char *fileName);

int main (int argc, char *argv[]){
     
     //error message if the number of arguments provided are not correct
     if(argc != 3){
         printf("\nInvalid number of arguments. The arguments given to the program should be the two directories you want to sync.\n");
         exit(0);
     }

     //storing all the file names in drectory a in a 2-d array
     char filesA[MAXLINE][MAXLINE];
     DIR *directory_A;
     struct dirent *de_A;

     if(!(directory_A = opendir(argv[1]))){
         printf("\nUnable to open directory %s\n", argv[1]);
         exit(0);
     }

     while(0 != (de_A = readdir(directory_A))){
         strcpy(filesA[count_A], de_A->d_name);
         count_A++;
     }

     //storing all the file names in drectory b in a 2-d array
     char filesB[MAXLINE][MAXLINE];
     DIR *directory_B;
     struct dirent *de_B;

     if(!(directory_B = opendir(argv[2]))){
         printf("\nUnable to open directory %s\n", argv[2]);
         exit(0);
     }

     while(0 != (de_B = readdir(directory_B))){
         strcpy(filesB[count_B], de_B->d_name);
         count_B++;
     }

     for(int i = 0; i < count_A; i++){

         //checking whether a file in directory a exists in directory b or not
         int found = search(filesA[i], filesB, count_B);

         //building the path of the file
         char *sourceFile = buildPath(argv[1], filesA[i]);
         char *destinationFile = buildPath(argv[2], filesA[i]);

         struct stat stat_A, stat_B;
         stat(sourceFile, &stat_A);
         stat(destinationFile, &stat_B);

         //replcating files in directory b which are present in directory a and not in b
         if(found == 0 && S_ISREG(stat_A.st_mode)){
             replicateFile(sourceFile, destinationFile);
         }

         //if a file is present in both the directories, replicating the latest modifed file in the other directory
         else if(found == 1 && S_ISREG(stat_A.st_mode)){
            double seconds = difftime(stat_A.st_mtime, stat_B.st_mtime);
            if(seconds < 0){
                replicateFile(destinationFile, sourceFile);
            }

            else{
                replicateFile(sourceFile, destinationFile);
            }	
         }

         //freeing up the allocated pointers
         free(sourceFile);
         free(destinationFile);
     }

     for(int i = 0; i < count_B; i++){

         //checking whether the file in directory b exists in directory a or not
         int found = search(filesB[i], filesA, count_A);

         //building the path for the file
         char *sourceFile = buildPath(argv[2], filesB[i]);

         struct stat stat_B;
         stat(sourceFile, &stat_B);
         
         //removing the file from directory b if it is not present in directory a
         if(found == 0 && S_ISREG(stat_B.st_mode)){
             printf("\nRemoving file %s\n", sourceFile);
             //removing the file
             remove(sourceFile);
         }
         
         //freeing up the allocated memory
         free(sourceFile);
     }

    //closing the directories
    closedir(directory_A);
    closedir(directory_B);

    return 0;
 }

 //searches for a filename in the given directory, returns 1 if found else 0.
 int search(char *fileName, char files[MAXLINE][MAXLINE], int size){
     for(int i = 0; i < size; i++){
         if(strcmp(fileName, files[i]) == 0)
            return 1;
     }
     return 0;
 }

 //replicates a file from the source directory in the destination directory
 void replicateFile(char *source, char *destination){
     printf("\nReplicating %s in %s.\n", source, destination);

     int fd1, fd2, retval1, retval2, n;
     char buf[8192];

     //opening the source file for reading its content
     if ((fd1 = open(source, O_RDONLY, 0)) == -1){
          printf("\nUnable to open source file %s\n", source);
          exit(10);
    }

    //opening the destination file to write the content
    if ((fd2 = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1){
          printf("\nUnable to open destination file %s\n", destination);
          exit(10);
      }
    
    //reading from the source file until EOF and writing the content to the destination file
    while ((n = read(fd1, buf, MAXLINE) ) > 0){
        write ( fd2, buf, n);
      }

    //closing both the files
    if ((retval1 = close(fd1)) < 0) {
        printf("\nError while closing the source file %s\n", source);
        exit(10);
      }
    
    if ((retval2 = close(fd2)) < 0) {
        printf("\nError while closing the destination file %s\n", destination);
        exit(10);
    }

 }

 //build the path from for the given file using the directory name provided. The path would be directoryname/filename
 char* buildPath(char* directoryName, char* fileName){
     int size = strlen(directoryName) + strlen("/") + strlen(fileName) + 1;
     char *filePath = (char *) malloc(size * sizeof(char));

     if (filePath == NULL){
         printf("\nUnable to allocate memory\n");
         exit(10);
     }

     strcpy(filePath, directoryName);
     strcat(filePath, "/");
     strcat(filePath, fileName);

    return filePath;
 }