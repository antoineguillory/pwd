#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#define CURRENT_DIR "."
#define PRED_DIR    ".."
#define ROOT        "/"

typedef int file_descriptor_t;

ino_t  find_inode_currdir();
ino_t  find_inode        (char* name);
char * find_next_step    (ino_t inode_curr, char* pwd);
bool   is_root           (void);
char * concat_with_root  (char * str1, char* str2);

int main(void){
    char* pwd_str  = malloc(1);
    char* pwd_next = NULL;
    do {
        pwd_next = find_next_step(find_inode_currdir(), pwd_str);
        if(pwd_next == NULL) break;
        char* tmp_pwd = malloc(strlen(pwd_next) + 2); // "/" et le \0
        tmp_pwd = concat_with_root(pwd_str, pwd_next);
        free(pwd_str);
        pwd_str = malloc(strlen(tmp_pwd)+1);
        strcpy(pwd_str, tmp_pwd);
        chdir(PRED_DIR);
        free(tmp_pwd);
    } while(strcmp(pwd_str, pwd_next) != 0);
    printf("%s\n", pwd_str);
    return EXIT_SUCCESS;
}

char * concat_with_root (char * str1, char* str2) {
    char* tmp = malloc(strlen(str2) + 2); // "/" et le \0
    strcpy(tmp, str2);
    strcat(tmp, ROOT);
    strcat(tmp, str1);
    return tmp;
}

ino_t find_inode_currdir() {
    return find_inode(CURRENT_DIR);
}

ino_t find_inode(char* name) {
    struct stat fd_stat;
    int ret_val = stat(name, &fd_stat);
    if (ret_val < 0) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    return fd_stat.st_ino;
}

char * find_next_step(ino_t inode_curr, char* pwd){
    if(is_root()){
        return pwd;
    }
    DIR *dir;
    struct dirent *dp;
    if ((dir = opendir (PRED_DIR)) == NULL) {
        perror ("Cannot open ..");
        exit (EXIT_FAILURE);
    }

    while ((dp = readdir(dir)) != NULL) {
        if(dp->d_ino==inode_curr){
            closedir(dir);
            return dp->d_name;
        }
    }
    return NULL;
}


bool is_root(void){
    return (find_inode_currdir() == find_inode(PRED_DIR));
}
