#ifndef __ANSI_H__
#define __ANSI_H__

#define ANSI_RESET "\e[0m"
#define ANSI_BOLD "\e[1m"
#define ANSI_BOLD_OFF "\e[22m"
#define ANSI_DIM "\e[2m"
#define ANSI_DIM_OFF "\e[22m"
#define ANSI_ITALIC "\e[3m"
#define ANSI_ITALIC_OFF "\e[23m"
#define ANSI_UNDERLINE "\e[4m"
#define ANSI_UNDERLINE_OFF "\e[24m"
#define ANSI_INVERSE "\e[7m"
#define ANSI_INVERSE_OFF "\e[27m"
#define ANSI_HIDDEN "\e[8m"
#define ANSI_HIDDEN_OFF "\e[28m"
#define ANSI_STRIKETHROUGH "\e[9m"
#define ANSI_STRIKETHROUGH_OFF "\e[29m"

#define ANSI_RED "\e[31m"
#define ANSI_GREEN "\e[32m"
#define ANSI_YELLOW "\e[33m"
#define ANSI_BLUE "\e[34m"
#define ANSI_MAGENTA "\e[35m"
#define ANSI_CYAN "\e[36m"
#define ANSI_WHITE "\e[37m"
#define ANSI_GRAY "\e[80m"

#define S_NOPE "\e[2m[\e[0;31m\u2718\e[0;2m]\e[0m"
#define S_GOOD "\e[2m[\e[0;32m\u2714\e[0;2m]\e[0m"

#define BOLD(s) ANSI_BOLD, s, ANSI_RESET

#endif
