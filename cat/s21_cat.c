#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUF_LEN 4096  // 4Кбайт

typedef struct {
  bool b_flag;  // нумерует только непустые строки - ok
  bool e_flag;  // также отображает символы конца строки как $ - ok
  bool n_flag;  // нумерует все выходные строки - ok
  bool s_flag;  // сжимает несколько смежных пустых строк - ok
  bool t_flag;  // также отображает табы как ^I
  bool v_flag;  // отображает непечатные символы
  bool T_flag;
  bool E_flag;
} CatInfo;

bool CatFlags(CatInfo *info, char *file);
// cat no arg
void ReadFromStd(int fd);

void PrintLine(CatInfo info, char *ptr, const char *end, bool *empty_line,
               int *count);

bool CatParseArg(CatInfo *info, char *argv) {
  argv++;
  bool flag = true;
  if (*argv == '-') {
    argv++;
    if (strcmp(argv, "-number-nonblank")) {
      info->b_flag = true;
    } else if (strcmp(argv, "-number")) {
      info->n_flag = true;
    } else if (strcmp(argv, "-squeeze-blank")) {
      info->s_flag = true;
    }
  } else {
    for (char *p = argv; *p != '\0'; p++) {
      switch (*p) {
        case 'b':
          info->b_flag = 1;
          break;
        case 'e':
          info->e_flag = 1;
          break;
        case 'E':
          info->E_flag = 1;
          break;
        case 'n':
          info->n_flag = 1;
          break;
        case 's':
          info->s_flag = 1;
          break;
        case 't':
          info->t_flag = 1;
          break;
        case 'T':
          info->T_flag = 1;
          break;
        case 'v':
          info->v_flag = 1;
          break;
        default:
          flag = false;
      }
    }
  }
  return flag;
}

// cat with arg
bool CatArgs(int argc, char *argv[]) {
  bool flag = true;
  CatInfo info = {0};
  for (int i = 1; i != argc; i++) {
    if (*argv[i] == '-')
      if (!CatParseArg(&info, argv[i])) flag = false;
  }
  if (!(info.b_flag || info.e_flag || info.n_flag || info.s_flag ||
        info.t_flag || info.v_flag || info.T_flag ||
        info.E_flag)) {  // если флагов нет, то открываем несколько файлов
    for (int i = 1; i != argc; i++) {  // а несколько файлов открывается тут
      if (*argv[i] != '-') ReadFromStd(open(argv[1], O_RDONLY));
    }
  } else {
    for (int i = 1; i != argc; i++) {
      if (*argv[i] != '-') CatFlags(&info, argv[i]);
    }
  }
  return flag;
}

int main(int argc, char *argv[]) {
  (void)argv;
  if (argc == 1) {
    ReadFromStd(STDIN_FILENO);
  } else {
    if (CatArgs(argc, argv)) return 1;
  }
  return 0;
}

// cat no arg
void ReadFromStd(int fd) {
  char buf[BUF_LEN];
  // int fd = STDIN_FILENO;
  int size_read = read(
      fd, buf, BUF_LEN);  // read(читаем из консоли, записать в буфер, размер
  // буфера) -- возрящает, сколько мы прочитали
  while (size_read > 0) {
    if (!EOF)
      printf("%.*s\n", size_read, buf);
    else
      printf("%.*s", size_read, buf);
    size_read = read(fd, buf, BUF_LEN);
  }
}

bool CatFlags(CatInfo *info, char *file) {
  bool flag = true, empty_line = false;
  char buf[BUF_LEN];
  int fd = open(file, O_RDONLY);

  if (fd == -1) {  // если ошибка при открытии, то
    flag = false;
  }

  int size_read = read(fd, buf, BUF_LEN - 1);
  int i = 1;
  while (size_read > 0) {
    char *end, *ptr;
    buf[size_read] = 0;
    ptr = buf;

    while (1) {
      end = strchr(ptr, '\n');  // поиск подстроки в строке
      PrintLine(*info, ptr, end, &empty_line, &i);
      if (end == 0) {
        break;
      }
      // PrintLine(*info, ptr, end, &empty_line, &i);
      ptr = end + 1;
      i++;
    }
    size_read = read(fd, buf, BUF_LEN - 1);
  }
  return flag;
}

void PrintLine(CatInfo info, char *ptr, const char *end, bool *empty_line,
               int *count) {
  if (info.t_flag || info.e_flag) {
    info.v_flag = true;
  }

  char curr_char = ' ';
  if (end) {
    for (char *p = ptr; p <= end; p++) {
      if (info.s_flag) {
        if (end == ptr) {
          (*count)--;
          if (!*empty_line) {
            if (info.n_flag) {
              (*count)++;
              printf("%6d\t\n", *count);
            } else {
              putchar('\n');
            }
          }
          *empty_line = true;
          return;
        } else {
          *empty_line = false;
        }
        curr_char = *p;
      }

      if (info.b_flag) {
        if (*ptr != '\n') {
          if (*(p - 1) == '\n' || p == ptr) printf("%6d\t", *count);

          curr_char = *p;
        }
        if (*ptr == '\n') {
          (*count)--;
          putchar('\n');
          continue;
        }

      } else if (info.n_flag) {  // нумерация всех строк
        if (*(p - 1) == '\n' || p == ptr) printf("%6d\t", *count);

        curr_char = *p;
      }

      if (info.e_flag) {  // выводит в конце строки $
        if (*p == '\n') {
          putchar('$');
          curr_char = *p;
        } else {
          curr_char = *p;
        }
      }

      if (info.E_flag) {  // выводит в конце строки $
        if (*p == '\n') {
          putchar('$');
          curr_char = *p;
        } else {
          curr_char = *p;
        }
      }

      if (info.T_flag) {
        if (*p == '\t') {
          printf("^I");
        } else {
          curr_char = *p;
          putchar(curr_char);
        }
      }

      if (info.v_flag && !info.t_flag) {
        int a = (int)(*p);
        int alt = 64;
        a > -1 && a < 9 ? printf("^") : 0;
        a > -1 && a < 9 ? printf("%c", (char)(alt + a)) : 0;
        a > 10 && a < 32 ? printf("^") : 0;
        a > 10 && a < 32 ? printf("%c", (char)(alt + a)) : 0;
        a == 127 ? printf("^?") : 0;
        a >= 32 && a != 127 ? printf("%c", *p) : 0;
        a == 10 ? printf("%c", *p) : 0;
        a == 9 ? printf("%c", (char)(9)) : 0;
      }
      if (info.t_flag == true) info.v_flag = true;

      if (info.v_flag && info.t_flag) {
        int a = (int)(*p);
        int alt = 64;
        a > -1 && a < 9 ? printf("^") : 0;
        a > -1 && a < 9 ? printf("%c", (char)(alt + a)) : 0;
        a > 10 && a < 32 ? printf("^") : 0;
        a > 10 && a < 32 ? printf("%c", (char)(alt + a)) : 0;
        a == 127 ? printf("хуй") : 0;
        a >= 32 && a != 127 ? printf("%c", *p) : 0;
        a == 10 ? printf("%c", *p) : 0;
        a == 9 ? printf("^I") : 0;
      }

      if (!info.v_flag && !info.T_flag) putchar(curr_char);
    }
  } else {
    for (char *p = ptr; *p != '\0'; p++) {
      if (info.s_flag) {
        if (end == ptr) {
          (*count)--;
          if (!*empty_line) {
            if (info.n_flag) {
              (*count)++;
              printf("%6d\t\n", *count);
            } else {
              putchar('\n');
            }
          }
          *empty_line = true;
          return;
        } else {
          *empty_line = false;
        }
        if (!end)
          curr_char = *p;
        else
          curr_char = *p;
      }

      if (info.b_flag) {
        if (*ptr != '\n') {
          if (*(p - 1) == '\n' || p == ptr) printf("%6d\t", *count);

          curr_char = *p;
        }
        if (*ptr == '\n') {
          (*count)--;
          putchar('\n');
          continue;
        }
      } else if (info.n_flag) {  // нумерация всех строк
        if (*(p - 1) == '\n' || p == ptr) printf("%6d\t", *count);
        curr_char = *p;
      }

      if (info.e_flag) {  // выводит в конце строки $
        if (*p == '\n') {
          putchar('$');
          curr_char = *p;
        } else {
          curr_char = *p;
        }
      }

      if (info.E_flag) {  // выводит в конце строки $
        if (*p == '\n') {
          putchar('$');
          curr_char = *p;
        } else {
          curr_char = *p;
        }
      }

      if (info.T_flag) {
        if (*p == '\t') {
          printf("^I");
        } else {
          curr_char = *p;
          putchar(curr_char);
        }
      }

      if (info.v_flag && !info.t_flag) {
        int a = (int)(*p);
        int alt = 64;
        a > -1 && a < 9 ? printf("^") : 0;
        a > -1 && a < 9 ? printf("%c", (char)(alt + a)) : 0;
        a > 10 && a < 32 ? printf("^") : 0;
        a > 10 && a < 32 ? printf("%c", (char)(alt + a)) : 0;
        a == 127 ? printf("^?") : 0;
        a >= 32 && a != 127 ? printf("%c", *p) : 0;
        a == 10 ? printf("%c", *p) : 0;
        a == 9 ? printf("%c", (char)(9)) : 0;
      }
      if (info.t_flag == true) info.v_flag = true;

      if (info.v_flag && info.t_flag) {
        int a = (int)(*p);
        int alt = 64;
        a > -1 && a < 9 ? printf("^") : 0;
        a > -1 && a < 9 ? printf("%c", (char)(alt + a)) : 0;
        a > 10 && a < 32 ? printf("^") : 0;
        a > 10 && a < 32 ? printf("%c", (char)(alt + a)) : 0;
        a == 127 ? printf("^?") : 0;
        a >= 32 && a != 127 ? printf("%c", *p) : 0;
        a == 10 ? printf("%c", *p) : 0;
        a == 9 ? printf("^I") : 0;
      }

      if (!info.v_flag && !info.T_flag) putchar(curr_char);
    }
  }
}
