#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct
{
    char **keys;
    char **values;
} args;

args *initARGS(int argc)
{
    args *tmp = (args *)malloc(sizeof(args));
    tmp->keys = (char **)malloc(sizeof(char *) * (argc - 2));
    tmp->values = (char **)malloc(sizeof(char *) * (argc - 2));
    for (int i = 0; i < argc - 2; i++)
    {
        tmp->keys[i] = (char *)malloc(sizeof(char) * 16);
        tmp->values[i] = (char *)malloc(sizeof(char) * 16);
    }
    return tmp;
}

void initArgSValue(int argc, char **argv, args *arg)
{
    for (int i = 2, j = 0; i < argc; ++i, ++j)
    {
        char *tmp = (char *)malloc(sizeof(char) * argc);
        assert(tmp != NULL);
        strcat(tmp, argv[i]);
        char *token = strtok(tmp, "=");
        if (token)
        {
            strcpy(arg->keys[j], token);
            token = strtok(NULL, "=");
            if (token)
            {
                strcpy(arg->values[j], token);
            }
        }
    }
}

void replaceVariables(FILE *fd, FILE *fd2, args *res, int argc)
{
    char buffer[1024];

    while (fgets(buffer, 1024, fd) != NULL)
    {
        int bufLenght = strlen(buffer);
        int i = 0;

        while (i < bufLenght)
        {
            if (isalnum(buffer[i]) || buffer[i] == '_')
            {
                int start = i;

                while (i < bufLenght && (isalnum(buffer[i]) || buffer[i] == '_'))
                {
                    i++;
                }

                char token[50];
                strncpy(token, buffer + start, i - start);
                token[i - start] = '\0';

                int flag = 0;
                for (int j = 0; j < argc - 2; j++)
                {
                    if (strcmp(token, res->keys[j]) == 0)
                    {
                        fwrite(res->values[j], sizeof(char), strlen(res->values[j]), fd2);
                        flag = 1;
                        break;
                    }
                }

                if (!flag)
                {
                    fwrite(token, sizeof(char), strlen(token), fd2);
                }
            }
            else
            {
                fwrite(&buffer[i], sizeof(char), 1, fd2);
                i++;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *fd = fopen(argv[1], "r");

    FILE *fd2 = fopen("modify.c", "w");

    args *res = initARGS(argc);

    initArgSValue(argc, argv, res);
    replaceVariables(fd, fd2, res, argc);

    fclose(fd);
    fclose(fd2);

    for (int i = 0; i < argc - 2; i++)
    {
        free(res->keys[i]);
        free(res->values[i]);
    }
    free(res->keys);
    free(res->values);
    free(res);

    return 0;
}

