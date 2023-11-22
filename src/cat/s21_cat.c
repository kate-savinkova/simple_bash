#include "s21_cat.h"

int main(int argc, char *argv[]) {
  Flags fl = parcer(argc, argv);
  if (fl.b || fl.e || fl.n || fl.s || fl.t || fl.v) {
    scan_files(argc, argv, fl);
  } else {
    int i = 1;
    while (i < argc) {
      FILE *fp;
      if ((fp = fopen(argv[i], "r")) != NULL) {
        if (feof(fp)) break;
        char ch;
        while ((ch = getc(fp)) != EOF) {
          printf("%c", ch);
        }
        fclose(fp);
      }
      i++;
    }
  }
  return 0;
}

Flags parcer(int argc, char *argv[]) {
  Flags fl = {0, 0, 0, 0, 0, 0};
  const char *short_options = "benstv";
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"E", no_argument, NULL, 'e'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {"T", no_argument, NULL, 't'},
      {"show-nonprinting", no_argument, NULL, 'v'},
      {NULL, 0, NULL, 0}};

  int option_index = -1;
  int rez = 0;
  while ((rez = getopt_long(argc, argv, short_options, long_options,
                            &option_index)) != -1) {
    switch (rez) {
      case 'b':
        fl.b = fl.n = 1;
        break;
      case 'e':
        fl.e = fl.v = 1;
        break;
      case 'n':
        fl.n = 1;
        break;
      case 's':
        fl.s = 1;
        break;
      case 't':
        fl.t = fl.v = 1;
        break;
      case 'v':
        fl.v = 1;
        break;
      case '?':
        break;
    }
    option_index = -1;
  }
  return fl;
}

void scan_files(int argc, char *argv[], Flags fl) {
  int i = 1;
  while (i < argc) {
    FILE *fp;
    if ((fp = fopen(argv[i], "r")) != NULL) {
      if (feof(fp)) break;
      cat_files(fp, fl);
      fclose(fp);
    }
    i++;
  }
}

void cat_files(FILE *fp, Flags fl) {
  int ch, enter, line, flag = 1;
  line = enter = 0;
  for (int prev = '\n'; (ch = getc(fp)) != EOF && flag; prev = ch) {
    if (prev == '\n') {
      if (fl.s) {
        if (ch == '\n') {
          if (enter) continue;
          enter = 1;
        } else
          enter = 0;
      }
      if (fl.n && (!fl.b || ch != '\n')) {
        fprintf(stdout, "%6d\t", ++line);
      }
    }
    if (ch == '\n') {
      if (fl.e && putchar('$') == EOF) flag = 0;
    } else if (ch == '\t') {
      if (fl.t) {
        if (putchar('^') == EOF || putchar('I') == EOF) break;
        continue;
      }
    } else if (fl.v) {
      char ch1 = ch;
      if (!isascii(ch1) && !isprint(ch1)) {
        ch1 = toascii(ch1);
      }
      if (iscntrl(ch1)) {
        if (putchar('^') == EOF ||
            putchar((int)ch1 == 127 ? '?' : ch + 64) == EOF)
          flag = 0;
        continue;
      }
    }
    if (putchar(ch) == EOF) flag = 0;
  }
}