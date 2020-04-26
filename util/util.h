#ifndef UTIL_H
#define UTIL_H

#define UNUSED(X) (void)(X)

char *make_string(const char *str);
void debug_puts(char *str);
void string_replace_single_quote_with_double(char *str);

#endif /* UTIL_H */
