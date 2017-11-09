#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pwd.h>
#include <dirent.h>
#include <grp.h>
#include <libgen.h>


#define CURRENT_DIR "."
#define PRED_DIR ".."

typedef int file_descriptor_t;

ino_t find_inode_currdir();
ino_t find_inode(char* name);
char * find_next_step(ino_t inode_curr, char* pwd);
bool is_root(void);

int main(void){
    char* pwd_str = malloc(2);
    char* pwd_next = malloc(2);
    do{
        pwd_next = find_next_step(find_inode_currdir(), pwd_str);
        char* tmp_pwd = malloc(strlen(pwd_next)+2); // "/" et le \0
        strcpy(tmp_pwd, pwd_next);
        strcat(tmp_pwd, "/");
        strcat(pwd_str, tmp_pwd);
        chdir(PRED_DIR);
        printf("DEBUG : NEXT DIR IS %s\n", pwd_next);
        printf("DEBUG : PWD STATE : %s\n", pwd_str);
        free(tmp_pwd);
    }while(strcmp(pwd_str,pwd_next)!=0);
    printf("%s\n", pwd_str);
    
    return EXIT_SUCCESS;
}

ino_t find_inode_currdir() {
    return find_inode(CURRENT_DIR);
}

ino_t find_inode(char* name) {
    struct stat fd_stat;
    
    file_descriptor_t fd = open(name, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    int ret_val = fstat(fd, &fd_stat);
    if (ret_val < 0) {  
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    
    ret_val = close(fd);
    if (ret_val < 0) {  
        perror("close");
        exit(EXIT_FAILURE);
    }
    
    return (ino_t) fd_stat.st_ino;
}

char * find_next_step(ino_t inode_curr, char* pwd){
    if(is_root()){
        return pwd;
    }
    DIR *dir;
    struct dirent *dp;
    if ((dir = opendir (PRED_DIR) == NULL) {
        perror ("Cannot open ..");
        exit (EXIT_FAILURE);
    }

    while ((dp = readdir(dir)) != NULL) {
        if((ino_t)dp->d_ino==inode_curr){
            closedir(dir);
            printf("DEBUG : dp is : %s\n", dp->d_name);
            return dp->d_name;
        }
    }
    
    exit(EXIT_FAILURE);
}

bool is_root(void){
    if (find_inode_currdir() == find_inode(PRED_DIR)){
        return true;
    }
    return false;
}
