/**
 * utils.c - Utility Functions
 *
 * Provides formatting and display utilities for system information output.
 * Handles colour application and consistent text formatting across the
 * application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "colour.h"

/**
 * print_title - Display application title
 * @title: Title text to display
 *
 * Prints the application title with cyan colouring. Allocates memory
 * for the coloured string, which is freed after printing.
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
 * colour_string - Apply ANSI colour codes to text
 * @colour: ANSI colour code to apply
 * @text: Text to colourise
 *
 * Creates a new string with the specified colour code prepended and
 * reset code appended. Caller is responsible for freeing the returned
 * string.
 *
 * Return: Newly allocated coloured string, or NULL on allocation failure
 */
char *colour_string(const char *colour, const char *text)
{
    size_t length = strlen(colour) + strlen(text) + strlen(RESET) + 1;
    char *output = (char *)malloc(length);
    if (output == NULL)
    {
        return NULL;
    }

    snprintf(output, length, "%s%s%s", colour, text, RESET);
    return output;
}

/**
 * format_string - Format key-value pair with colour
 * @key: Label text
 * @value: Value text
 * @colour: ANSI colour code for the key
 *
 * Prints a formatted key-value pair with the key colourised and
 * emboldened. Uses fixed-width formatting for consistent alignment.
 */
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

/**
 * format_row - Format key-value pair with default colour
 * @key: Label text
 * @value: Value text
 *
 * Convenience wrapper for format_string() using cyan as the default
 * key colour.
 */
void format_row(const char *key, const char *value)
{
    format_string(key, value, CYAN);
}