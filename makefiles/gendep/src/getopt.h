#ifndef SPI_GETOPT_H
#define SPI_GETOPT_H

#ifdef __cplusplus
extern "C"
{
#endif

extern int opterr;    /* if error message should be printed */
extern int optind;    /* index into parent argv vector */
extern int optopt;    /* character checked for validity */
extern char *optarg;  /* argument associated with option */

int getopt(
    int nargc,
    char **nargv,
    char *ostr);

#ifdef __cplusplus
}
#endif

#endif /* SPI_GETOPT_H */
