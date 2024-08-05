#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 100

typedef struct {
  bool e_flag;  // Шаблон
  bool i_flag;  // Игнорирует различия регистра.
  bool v_flag;  // Инвертирует смысл поиска соответствий. выводит строки, где
                // нет шаблона
  bool c_flag;  // Выводит только количество совпадающих строк.
  bool l_flag;  // Выводит только совпадающие файлы.
  bool
      n_flag;  // Предваряет каждую строку вывода номером строки из файла ввода.

  bool h_flag;  // Выводит совпадающие строки, не предваряя их именами файлов.
  bool s_flag;  // Подавляет сообщения об ошибках о несуществующих или
                // нечитаемых файлах.
  bool f_flag;  // Получает регулярные выражения из файла.
  bool o_flag;  // Печатает только совпадающие (непустые)

  char tem[N][N];
  int count_files;
  int tem_len;
  bool check_multifile;
} FlagInfo;

typedef struct check {
  bool multifile;
  bool print;
  bool v;
  int line_count;
} type_check;

// парсер
bool GrepParseArg(FlagInfo* info, int* count, int rez);

// запись шаблона в массив с шаблонами
bool write_template(FlagInfo* info, const int* count, char* argv);

void reader(FILE** file, char* file_name, FlagInfo info);

void print_cl(FlagInfo info, char* path_file, int count_c);

void cl_work(FlagInfo info, char* curr_line, int* count_c);

// печатаем строку, если n==0 && v==1
void print_line(char* curr_line, char* path_file, type_check* check,
                FlagInfo info, int quant, int o_first);

// ok
void v_work(FlagInfo info, char* curr_line, int i, type_check* check,
            char* path_file, int quant);
#endif  // SRC_GREP_S21_GREP_H_
