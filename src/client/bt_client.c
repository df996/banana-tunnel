//
// Created by cheng on 2023/5/30.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    char *str = "127.0.0.1:3306";
    char *val = strtok(str, ":");
    while (val != NULL) {
        printf("val: %s\n", val);
        val = strtok(NULL, ":");
    }

    // if (argc <= 2) {
    //     printf("Usage: bt-client server-ip:port ports...\n");
    //     exit(1);
    // }

    // int index = 0;
    // char **arg = argv;
    // while (*arg != NULL) {
    //     index++;
    //     *arg++;
    //     if (index <= 2) {
    //         continue;
    //     }
    //     printf("%llu\n", strlen(*arg));
    //     printf("%s, %p\n", *arg, arg);
    // }

    // int count = 0; 
    // while(argv[++count] != NULL);
    // printf("argv length: %d\n", count);
    // printf("argv length: %d\n", sizeof(argv) / sizeof(*argv));

    // char **ports;
    // ports = malloc(sizeof(char *) * )

    // int index = 0;
    // char **arg = NULL;
    // for (arg = argv; *arg != NULL; arg++) {
    //     index++;
    //     if (index < 2) {
    //         continue;
    //     }
    //     printf("string: %s length: %d pinter: %llu\n", *arg, strlen(*arg), arg);
    // }

    return 0;
}