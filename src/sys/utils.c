#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "colour.h"

/**
 * Prints a bold-coloured title.
 */
void print_title(const char *title)
{
    char *coloured_title = colour_string(CYAN, title);
    if (coloured_title)
    {
        printf("\n%s\n", coloured_title);
        free(coloured_title);
    }
}
/**
 * Returns a dynamically allocated coloured string.
 * The caller must free the allocated memory after use.
 */
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

/**
 * Formats a key-value pair with a coloured key.
 */
void format_string(const char *key, const char *value, const char *colour)
{
    char *coloured_key = colour_string(colour, key);
    if (coloured_key)
    {
        printf("%-25s %s\n", coloured_key, value);
        free(coloured_key);
    }
}

/**
 * Formats a key-value row using format_string for colouring (default: CYAN).
 */
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