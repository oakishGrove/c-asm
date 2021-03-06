#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>

/* 
 * Performs search of file or folder.
 *
 * TODO: mb
 *   [ ] word distance toleration
 *   [ ] exact keyword search flag
 *   [ ] change search root dir
 *   [ ] look for folders only
*/

char* findStringCase(const char* source, const char* contains) {
    if (contains != NULL && source != NULL ) {

        int index = 0;
        unsigned int containsLength = strlen(contains);
        int matchedChars = 0;
        int containsIndex = 0;
        while (source[index] != '\0') {
            if (tolower(source[index++]) == tolower(contains[containsIndex++])) {
                ++matchedChars;
            } else {
                matchedChars = 0;
                containsIndex = 0;
            }
            if (matchedChars == containsLength) {
                return source + index - containsLength;
            }
        }
    }

    return NULL;
}


void appendPath(char **string, int stringLen, int* stringSize, char *data) {
    if (string == NULL || *string == NULL || data == NULL)
        exit(0);

    unsigned long dataLen = strlen(data);
    if (dataLen + stringLen + 1 > *stringSize) {
        char *temp = (char *) malloc(*stringSize * 2);
        if (temp) {
            strcpy(temp, *string);
            free(*string);
            *string = temp;
            *stringSize *= 2;
        } else
            exit(1);
    }

    *(*string + stringLen++) = '/';
    for (int i = 0; i < dataLen; ++i) {
        *(*string + stringLen++) = *data++;
    }
}

void clearAppendedPath(char *string, int len, const int *pPathSize) {
    for ( int i = len; i < *pPathSize; ++i ) {
        string[i] = '\0';
    }
}

char** populateFileNames(int* fileNamesSize, int *fileNamesCount, char* dirPath) {

    char** fileNames = malloc(sizeof(char *) * *fileNamesSize);
    for(int i = 0; i < *fileNamesSize; ++i) {
        *(fileNames + i) = calloc(NAME_MAX, sizeof (char));
    }
    if (fileNames == NULL)
        exit(1);

    DIR *directory = NULL;
    int fileNamesIndex = 0;
    if ((directory = opendir(dirPath)) != NULL) {
        struct dirent *obj;
        while ((obj = readdir(directory)) != NULL) {
            if (strcmp(obj->d_name, ".") == 0
                || strcmp(obj->d_name, "..") == 0 )
                continue;
            if (fileNamesIndex >= *fileNamesSize) {
                *fileNamesSize *= 2;
                fileNames = realloc(fileNames, sizeof(char *) * *fileNamesSize);
                if (fileNames == NULL) {
                    exit(1);
                }
                for (int i = fileNamesIndex; i < *fileNamesSize; ++i) {
                    *(fileNames + i) = calloc(NAME_MAX, sizeof (char));
                }
            }
            *(*(fileNames+fileNamesIndex)+0) = obj->d_type;
            strcat((char *) fileNames[fileNamesIndex++]+1, obj->d_name);
        }
        closedir(directory);
    }
    *fileNamesCount = fileNamesIndex;
    return fileNames;
}

void freeFileNames(char **fileNames, int fileNamesCount) {
    for (int i = 0; i < fileNamesCount; ++i) {
        free(fileNames[i]);
    }
    free(fileNames);
}


// each string in array{fileNames} first byte means file type{struct dirent -> d_type}
void searchForDirHelper(char** path, char* target, int pathLen, int *pathSize) {
    static long counter = 0;
    int fileNamesSize = 32;
    int fileNamesCount = 0;
    char** fileNames = populateFileNames(&fileNamesSize, &fileNamesCount, *path);


    counter++;
    for (int i = 0; i < fileNamesCount; ++i) {
        if (findStringCase(fileNames[i], target)) {
            printf("{%d}||%s/%s\n", counter, *path, fileNames[i]+1);
        } else {
            if (*(*(fileNames+i)+0) == 4) {
                appendPath(path, pathLen, pathSize, *(fileNames+i)+1);
                searchForDirHelper(path, target, strlen(*path), pathSize);
                clearAppendedPath(*path, pathLen, pathSize);
            }
        }
    }
    freeFileNames(fileNames, fileNamesSize);
}


void searchForDir(char* path, char* target) {
    if (path && target) {
        unsigned long pathLen = strlen(path);
        int searchPathSize = 128 <= pathLen ? pathLen * 2 : 128;
        char *searchPath = malloc(searchPathSize);

        if (searchPath != NULL) {
            *searchPath = '\0';
            strcpy(searchPath, path);
            searchForDirHelper(&searchPath, target, pathLen, &searchPathSize);
            free(searchPath);
        }
    }
}

int main(int argc, char** argv) {

//    if (argc != 2) {
//        printf("2 params expected");
//        return 0;
//    }
    searchForDir("../../../..", "main.c");

//    searchForDir("..", argv[1]);
    //#include <sys/stat.h>
//    S_IFDIR
//    struct stat object_data;
//    memset(&object_data, 0xff, sizeof(object_data));
//    stat("../main.c", &object_data);
//    printf("%ld\n", object_data.st_ino);

//    if (S_ISDIR(object_data.st_mode))
//    if (S_ISREG(object_data.st_mode))
    return 0;
}
