/*
 *  mypwd.c
 *  
 *
 *  Created by Mattics on 5/2/11.
 *
 */

#include "mypwd.h"

#define LENGTH 2048

void printPath(int numDirectories);
void otherMethods(struct stat root);
struct stat getCurrentDirectory();
struct stat getParentDirectory();
int scanFiles(DIR *directory, struct stat currentDirectory, int numDirectories);

static char **pathname;

/* ================================================================ */

int main(void)
{
    struct stat root;
    int i;
    
    /* Initializing the table to NULL */
    pathname = malloc(sizeof(*pathname) * (LENGTH + 1));
    for (i = 0; i < LENGTH; i++) {
        pathname[i] = NULL;
    }
    
    /* Initializes the root */
    if (stat("/home", &root)) {
        perror("mypwd\n");
        exit(0);
    }
    
    otherMethods(root);
    
    return 0;
}

/* ================================================================ */

void otherMethods(struct stat root)
{
    struct stat currentDirectory, parentDirectory;
    DIR *directory;
    int numDirectories = 0;
    
    while ((directory = opendir("..")) != NULL) {
        
        currentDirectory = getCurrentDirectory();
        
        /* Changes directory to parent */
        if (chdir("..")) {
            perror("mypwd\n");
            exit(0);
        }
        
        parentDirectory = getParentDirectory();
        
        numDirectories = scanFiles(directory, currentDirectory, numDirectories);
        
        /* Checks to see if parent directory is root */
        if ((root.st_dev == parentDirectory.st_dev) &&
            (root.st_ino == parentDirectory.st_ino)) {
            break;
        }
    }
    
    closedir(directory);
    printPath(numDirectories);
}

/* ================================================================ */

struct stat getCurrentDirectory()
{
    struct stat current;
    
    if (stat(".", &current)) {
        perror("mypwd\n");
        exit(0);
    }
    
    return current;
}

/* ================================================================ */

struct stat getParentDirectory()
{
    struct stat parent;
    
    if (stat(".", &parent)) {
        perror("mypwd\n");
        exit(0);
    }
    
    return parent;
}

/* ================================================================ */

int scanFiles(DIR *directory, struct stat currentDirectory, int numDirectories)
{
    struct dirent *file;
    struct stat direct;
    
    /* Scan in each file in directory to find child */
    while ((file = readdir(directory)) != NULL) {
        if (stat(file->d_name, &direct)) {
            perror("mypwd\n");
        }
        
        if (direct.st_ino == currentDirectory.st_ino &&
            direct.st_dev == currentDirectory.st_dev) {
            if (strcmp(file->d_name, "..") != 0 && 
                strcmp(file->d_name, ".") != 0) {
                pathname[numDirectories] = file->d_name;
                numDirectories++;
            }
        }
    }
    
    return numDirectories;
}

/* ================================================================ */

void printPath(int numDirectories)
{
    /* Prints out the path */
    if (numDirectories == 0) {
        printf("/\n");
    } else if (numDirectories > LENGTH) {
        printf("Path too long.\n");
        exit(0);
    }else {
        for (numDirectories -= 1; numDirectories >= 0; numDirectories--) {
            printf("/%s", pathname[numDirectories]);
        }
        printf("\n");
    }
}