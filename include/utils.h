#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
void prepare();
#else
#define prepare()
#endif

void print_title(const char *title);
void format_row(const char *key, const char *value);
void format_string(const char *key, const char *value, const char *colour);
char *colour_string(const char *colour, const char *text);

#endif // UTILS_H
