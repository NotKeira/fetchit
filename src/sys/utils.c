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

#ifdef _WIN32
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

/**
 * prepare - Prepares fetchit
 *
 * On Windows, it configures stdout to support ANSI escape codes.
 *
 * On Linux, it is no-op.
 */
void prepare() {
    HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (stdout_handle == NULL || stdout_handle == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD current_stdout_mode = 0;

    if (GetConsoleMode(stdout_handle, &current_stdout_mode) == TRUE) {
        current_stdout_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        SetConsoleMode(stdout_handle, current_stdout_mode);
    }
}
#endif

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
    if (coloured_title != NULL)
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
    if (output != NULL)
    {
        snprintf(output, length, "%s%s%s", colour, text, RESET);
    }

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

    if (bold_key != NULL)
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