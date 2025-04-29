#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "colour.h"

void print_title(const char *title)
{
    char *coloured_title = colour_string(CYAN, title);
    if (coloured_title)
    {
        printf("\n%s\n", coloured_title);
        free(coloured_title);
    }
}
char *colour_string(const char *colour, const char *text)
{
    size_t length = strlen(colour) + strlen(text) + strlen(RESET) + 1;
    char *output = (char *)malloc(length);

    if (output == NULL)
    {
        perror("malloc");
        return NULL;
    }

    snprintf(output, length, "%s%s%s", colour, text, RESET);
    return output;
}

void format_string(const char *key, const char *value, const char *colour)
{
    char *coloured_key = colour_string(colour, key);
    char *bold_key = colour_string(BOLD, coloured_key);
    free(coloured_key);

    if (bold_key)
    {
        printf("%-35s %s\n", bold_key, value);
        free(bold_key);
    }
}

void format_row(const char *key, const char *value)
{
    format_string(key, value, CYAN);
}

void trim_leading_spaces(char *str)
{
    while (isspace((unsigned char)*str))
    {
        str++;
    }
}

char *get_command_output(const char *command)
{
    FILE *fp;
    char *result = malloc(256);
    fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("popen");
        free(result);
        return NULL;
    }
    fgets(result, 256, fp);
    pclose(fp);
    result[strcspn(result, "\n")] = '\0';
    return result;
}