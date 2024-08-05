#define _GNU_SOURCE

#include "s21_grep.h"

bool write_template(FlagInfo *info, const int *count, char *argv) {
  int j = 0;
  for (char *i = argv; *i != '\0'; i++) {
    info->tem[*count][j] = argv[j];
    j++;
  }
  info->tem_len += 1;
  return true;
}

bool GrepParseArg(FlagInfo *info, int *count, int rez) {
  int flag = 0;
  switch (rez) {
    case 'e':
      info->e_flag = 1;
      if (optarg == NULL) {
        flag = 1;
        printf("s21_grep: option requires an argument -- %c\n", rez);
      } else {
        write_template(info, count, optarg);
        *count += 1;
      }
      break;
    case 'i':
      info->i_flag = 1;
      break;
    case 'v':
      info->v_flag = 1;
      break;
    case 'c':
      info->c_flag = 1;
      break;
    case 'l':
      info->l_flag = 1;
      break;
    case 'n':
      info->n_flag = 1;
      break;
    case 'h':
      info->h_flag = 1;
      break;
    case 's':
      info->s_flag = 1;
      break;
    case 'o':
      info->o_flag = 1;
      break;
    case 'f':
      info->f_flag = 1;
      if (optarg == NULL) {
        flag = 1;
        printf("s21_grep: option requires an argument -- %c\n", rez);
      } else {
        FILE *file;
        file = fopen(optarg, "r");
        if (file != 0) {
          char *s = NULL;
          size_t line_t = 0;
          while (getline(&s, &line_t, file) != -1) {
            if (strcmp(s, "\n") != 0) {
              if (s[strlen(s) - 1] == '\n') {
                s[strlen(s) - 1] = '\0';
              }
              write_template(info, count, s);
              *count += 1;
            } else {
              char *temp = ".*";
              write_template(info, count, temp);
              *count += 1;
            }
            if (s != NULL) {
              free(s);
              s = NULL;
            }
          }
          if (s != NULL) {
            free(s);
          }
        }
        fclose(file);
      }
      break;
  }

  return flag;
}

void reader(FILE **file, char *file_name, FlagInfo info) {
  if (info.v_flag == 1) {
    info.o_flag = 0;
  }

  char *curr_line = NULL;
  size_t line_t = 0;
  int count_c = 0, count_cv = 0, count_cc = 0;

  type_check check = {0};
  int quant = 0;
  while (getline(&curr_line, &line_t, *file) != -1) {
    quant++;
    check.line_count++;
    check.print = 0;
    check.v = 0;

    if (info.c_flag == 1 || info.l_flag == 1) {
      cl_work(info, curr_line, &count_c);
      if (info.v_flag == 1) {
        count_c == info.tem_len ? count_cv = count_cv + 1 : 0;
        count_c = 0;
      } else {
        if (count_c > 0 && strcmp(curr_line, "\n") != 0) {
          count_cc = count_cc + 1;
        }

        count_c = 0;
      }
      continue;
    }

    v_work(info, curr_line, 0, &check, file_name, quant);
    if (curr_line != NULL) {
      free(curr_line);
      curr_line = NULL;
    }
  }
  (curr_line != NULL) ? free(curr_line) : 0;

  info.v_flag == 0 ? print_cl(info, file_name, count_cc)
                   : print_cl(info, file_name, count_cv);
}

int reg_compare(char *curr_line, FlagInfo info, int i) {
  int res = 1;
  regex_t reg;
  if (info.i_flag == true) {  //  игнорирует различия регистра
    regcomp(&reg, info.tem[i], REG_EXTENDED | REG_ICASE);
  } else {
    regcomp(&reg, info.tem[i], REG_EXTENDED);
  }
  if (curr_line != NULL) {
    regmatch_t pmatch[1];
    size_t nmatch = 1;
    int flag = 0;

    int line_size = strlen(curr_line);
    if (curr_line[line_size - 1] == '\n') {
      curr_line[line_size - 1] = '\0';
      flag = 1;
    }
    res = regexec(&reg, curr_line, nmatch, pmatch, 0);
    if (flag == 1) {
      curr_line[line_size - 1] = '\n';
    }
  }
  regfree(&reg);
  return res;
}

void cl_work(FlagInfo info, char *curr_line, int *count_c) {
  for (int i = 0; i < info.tem_len; ++i) {
    if ((reg_compare(curr_line, info, i) == 0 && info.v_flag == 0) ||
        (reg_compare(curr_line, info, i) != 0 && info.v_flag == 1)) {
      *count_c += 1;
    }
  }
}

void print_cl(FlagInfo info, char *file_name, int count_c) {
  if (info.c_flag == 1 && info.l_flag == 0) {
    info.h_flag == 0 && info.check_multifile == 1
        ? printf("%s:%i\n", file_name, count_c)
        : printf("%i\n", count_c);
  }

  if (info.l_flag == 1 && info.c_flag == 0 && count_c > 0) {
    printf("%s\n", file_name);
  }

  if (info.c_flag == 1 && info.l_flag == 1) {
    if (info.check_multifile == 1) {
      if (info.h_flag == 1) {
        count_c > 0 ? printf("1\n%s\n", file_name) : printf("0\n");
      } else {
        count_c > 0 ? printf("%s:1\n%s\n", file_name, file_name)
                    : printf("%s:0\n", file_name);
      }
    } else {
      count_c > 0 ? printf("1\n%s\n", file_name) : printf("0\n");
    }
  }
}

void print_line(char *curr_line, char *file_name, type_check *check,
                FlagInfo info, int quant, int o_first) {
  if (info.check_multifile && info.h_flag == 0) {
    printf("%s:", file_name);
  }
  if (info.n_flag == 0) {
    if (info.v_flag == 1) {
      printf("%s", curr_line);
      if (curr_line[strlen(curr_line) - 1] != '\n') {
        putchar('\n');
      }
      check->print += 1;
    } else {
      printf("%s", curr_line);
      if (curr_line[strlen(curr_line) - 1] != '\n') {
        putchar('\n');
      }
      check->print += 1;
    }
  } else {
    if (o_first == 1) {
      printf("%d:", quant);
    }
    if (info.v_flag == 1) {
      printf("%s", curr_line);
      if (curr_line[strlen(curr_line) - 1] != '\n') {
        putchar('\n');
      }
      check->print += 1;
    } else {
      printf("%s", curr_line);
      if (curr_line[strlen(curr_line) - 1] != '\n') {
        putchar('\n');
      }
      check->print += 1;
    }
  }
}

// -v выводит строки, где нет шаблона
void v_work(FlagInfo info, char *curr_line, int i, type_check *check,
            char *file_name, int quant) {
  int v_flag = 0;
  for (i = 0; i < info.tem_len; ++i) {
    if (info.v_flag == 0) {
      if (reg_compare(curr_line, info, i) == 0 && check->print == 0) {
        if (info.o_flag == 1) {
          int count = 0;
          char *str = malloc(1000);
          str[0] = '\0';
          strcpy(str, info.tem[i]);
          while (reg_compare(curr_line, info, i) == 0) {
            count += 1;
            strcat(info.tem[i], ".*");
            strcat(info.tem[i], str);
          }
          strcpy(info.tem[i], str);
          for (int j = 0; j < count; ++j) {
            char a[1000];
            int q = 0;
            for (int k = 0; k <= (int)strlen(info.tem[i]); ++k) {
              // printf("%d\n", k);
              if (!(info.tem[i][k] == '^' || info.tem[i][k] == '$')) {
                a[k - q] = info.tem[i][k];
              } else {
                q += 1;
              }
            }
            print_line(a, file_name, check, info, quant, j + 1);
          }
          free(str);
        } else {
          print_line(curr_line, file_name, check, info, quant, 1);
        }
      }
    } else if (info.v_flag == 1) {
      if (reg_compare(curr_line, info, i) != 0 && check->print == 0) {
        v_flag++;
      }
    }
  }
  if (v_flag == info.tem_len) {
    if (info.o_flag == 1) {
      print_line(info.tem[i], file_name, check, info, quant, 1);
    } else {
      print_line(curr_line, file_name, check, info, quant, 1);
    }
  }
}
