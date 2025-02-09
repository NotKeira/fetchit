#ifndef UTILS_H
#define UTILS_H

char *get_command_output(const char *command);
void print_title(const char *title);
void format_row(const char *key, const char *value);
void format_string(const char *key, const char *value, const char *colour);
char *colour_string(const char *colour, const char *text);

#endif // UTILS_H
