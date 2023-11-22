#include "s21_grep.h"

int main(int argc, char* argv[]) {
  Flags fl = {0};
  parcer(argc, argv, &fl);
  if (!fl.error) {
    grep_files(&fl);
  }
  return fl.error;
}

void parcer(int argc, char* argv[], Flags* fl) {
  FILE* f_files = fopen("f_files.txt", "w");
  FILE* f_patterns = fopen("f_patterns.txt", "w");
  int stop = 0;
  for (int i = 1; i < argc && !(*fl).error; i++) {
    if (argv[i][0] == '-' && argv[i][1] != '-') {
      for (size_t j = 1; j < strlen(argv[i]) && !(*fl).error; j++) {
        switch (argv[i][j]) {
          case 'e':
            (*fl).e = 1;
            break;
          case 'i':
            (*fl).i = 1;
            break;
          case 'v':
            (*fl).v = 1;
            break;
          case 'c':
            (*fl).c = 1;
            break;
          case 'l':
            (*fl).l = 1;
            break;
          case 'n':
            (*fl).n = 1;
            break;
          case 'h':
            (*fl).h = 1;
            break;
          case 's':
            (*fl).s = 1;
            break;
          case 'f':
            (*fl).f = 1;
            break;
          case 'o':
            (*fl).o = 1;
            break;
          default:
            printf("./s21_grep: illegal option -- %c\n", argv[i][j]);
            (*fl).error = 1;
            break;
        }
      }
    } else if ((*fl).f && !stop) {
      strcpy((*fl).regexfile, argv[i]);
      stop = 1;
    } else if (!(*fl).num_patterns ||
               ((strlen(argv[i - 1]) == 2) && (argv[i - 1][0] == '-') &&
                (strchr("e", argv[i - 1][1]) != NULL))) {
      FILE* fp = NULL;
      if ((fp = fopen(argv[i], "r")) != NULL && strcmp(argv[i], ".")) {
        fclose(fp);
        check_file(argv[i], fl, f_files);
      } else {
        if ((*fl).num_patterns) fputs("\n", f_patterns);
        fputs(argv[i], f_patterns);
        (*fl).num_patterns++;
      }
    } else {
      check_file(argv[i], fl, f_files);
    }
  }
  if ((*fl).f) regex_files(fl, f_patterns);
  fclose(f_files);
  fclose(f_patterns);
}

void check_file(char* file_name, Flags* fl, FILE* f_files) {
  FILE* fp = fopen(file_name, "r");
  if (fp == NULL) {
    if (!(*fl).s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", file_name);
    }
    (*fl).error = 1;
  } else {
    if ((*fl).num_files) fputs("\n", f_files);
    fputs(file_name, f_files);
    (*fl).num_files++;
    fclose(fp);
  }
}

void regex_files(Flags* fl, FILE* f_patterns) {
  FILE* fp = fopen((*fl).regexfile, "r");
  if (fp == NULL) {
    if (!(*fl).s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", (*fl).regexfile);
    }
    (*fl).error = 1;
  } else {
    size_t len = 0;
    ssize_t str_len = 0;
    char* pattern = NULL;
    while ((str_len = getline(&pattern, &len, fp)) != -1) {
      if ((*fl).num_patterns) fputs("\n", f_patterns);
      fputs(pattern, f_patterns);
      (*fl).num_patterns++;
    }
    free(pattern);
    fclose(fp);
  }
}

int search_pattern(char* pattern, Flags* fl, char* line, int print) {
  int count = 0, flag = 0, stop = 0;
  regex_t re;

  if ((*fl).i)
    flag = REG_ICASE;
  else if ((*fl).e || (*fl).f)
    flag = REG_EXTENDED;

  regcomp(&re, pattern, flag);
  int line_position = 0;
  regmatch_t regmatch;
  while (regexec(&re, &line[line_position], 1, &regmatch, 0) == 0 && !stop) {
    if ((*fl).o && !(*fl).v && !(*fl).c && !(*fl).l && print) {
      while (regmatch.rm_so < regmatch.rm_eo) {
        printf("%c", line[line_position + regmatch.rm_so]);
        regmatch.rm_so++;
      }
      printf("\n");
    }
    if (line[0] != '\n') count++;
    line_position += regmatch.rm_eo;
  }

  regfree(&re);
  return count;
}

void grep_files(Flags* fl) {
  int not_first_file = 0;
  char* file = NULL;
  size_t file_len = 0;
  ssize_t r = 0;

  FILE* f_files = fopen("f_files.txt", "r");

  while ((r = getline(&file, &file_len, f_files)) != -1) {
    if (file[strlen(file) - 1] == '\n') file[strlen(file) - 1] = '\0';
    FILE* fp;
    if ((fp = fopen(file, "r")) != NULL) {
      if (feof(fp)) (*fl).error = 1;
      char* line = NULL;
      size_t len = 0;
      ssize_t read = 0;
      int count = 0, line_num = 1;

      while ((read = getline(&line, &len, fp)) != -1) {
        int printed = 0;
        char* pattern = NULL;
        size_t pattern_len = 0;
        ssize_t p = 0;
        FILE* f_patterns = fopen("f_patterns.txt", "r");

        while ((p = getline(&pattern, &pattern_len, f_patterns)) != -1) {
          if (pattern[strlen(pattern) - 1] == '\n')
            pattern[strlen(pattern) - 1] = '\0';
          int searching = search_pattern(pattern, fl, line, 0);

          if (!(*fl).v && searching) {
            if (!printed) count++;
            if ((!(*fl).l) && (!(*fl).c)) {
              if (!printed) prev_output(fl, file, printed, line_num);
              if ((*fl).o)
                search_pattern(pattern, fl, line, 1);
              else if (!printed)
                standart_output(fl, line);
            }
            printed = 1;
          } else if (!searching && (*fl).v) {
            count++;
            if (!printed && !searching && (!(*fl).l) && (!(*fl).c)) {
              prev_output(fl, file, printed, line_num);
              standart_output(fl, line);
              printed = 1;
            }
          }
        }

        if (pattern != NULL) free(pattern);
        line_num++;
        fclose(f_patterns);
      }
      if ((*fl).l || (*fl).c) past_output(fl, file, count);

      if (line != NULL) free(line);
      fclose(fp);
    }
    not_first_file++;
  }
  if (file != NULL) free(file);
  fclose(f_files);
}

void prev_output(Flags* fl, char* file, int printed, int line_num) {
  if (!(*fl).l && (*fl).num_files > 1 && !(*fl).h) printf("%s:", file);
  if (!(*fl).c && !(*fl).l)
    if ((*fl).n && !printed) printf("%d:", line_num);
}

void standart_output(Flags* fl, char* line) {
  if (!(*fl).c && !(*fl).l) {
    printf("%s", line);
    if (strlen(line) > 0)
      if (line[strlen(line) - 1] != '\n') printf("\n");
  }
}
void past_output(Flags* fl, char* file, int count) {
  if ((!(*fl).l || (*fl).c) && (*fl).num_files > 1 && !(*fl).h)
    printf("%s:", file);

  if ((*fl).c) {
    ((*fl).l && count) ? printf("1\n") : printf("%d\n", count);
  }
  if (count && (*fl).l) printf("%s\n", file);
}