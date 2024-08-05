#include "s21_grep.h"

const struct option long_options[] = {{"e", required_argument, NULL, 'e'},
                                      {"f", required_argument, NULL, 'f'},
                                      {NULL, 0, NULL, 0}};

int main(int argc, char *argv[]) {
  FlagInfo info = {0};
  int ch, count = 0, flag = 0;
  while ((ch = getopt_long(argc, argv, "e:ivclnhsf:o", long_options, NULL)) !=
         -1) {
    GrepParseArg(&info, &count, ch);
  }
  if (optind != argc && count == 0) {
    info.tem_len = 0;
    for (int i = 0; i < (int)strlen(argv[optind]); i++) {
      info.tem[count][i] = argv[optind][i];
      info.tem_len++;
    }
    info.tem_len = 1;
    count++;
    flag = 1;
  }
  // открываем файл
  int curr_arg = optind + flag;
  FILE *file;
  if (curr_arg + 1 < argc) {
    info.check_multifile = 1;
  }

  while (curr_arg < argc) {
    char *file_name = argv[curr_arg];
    file = fopen(file_name, "r");
    // если все ок и все открылось
    if (file != 0) {
      reader(&file, file_name, info);
    }

    curr_arg++;
    fclose(file);
  }
}
