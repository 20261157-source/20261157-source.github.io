#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXTOKEN 1000

enum { NAME, PARENS, BRACKETS };

void dcl(void);
void dirdcl(void);
int gettoken(void);
void ungettoken(void);
int is_type(char *s);

int tokentype;
char token[MAXTOKEN];
char name[MAXTOKEN];
char datatype[MAXTOKEN];
char out[10000];

int has_buf = 0;
int buf_tokentype;
char buf_token[MAXTOKEN];

void ungettoken(void) {
    has_buf = 1;
    buf_tokentype = tokentype;
    strcpy(buf_token, token);
}

int gettoken(void) {
    int c;
    char *p = token;

    if (has_buf) {
        has_buf = 0;
        tokentype = buf_tokentype;
        strcpy(token, buf_token);
        return tokentype;
    }

    while ((c = getchar()) == ' ' || c == '\t')
        ;

    if (c == '(') {
        if ((c = getchar()) == ')') {
            strcpy(token, "()");
            return tokentype = PARENS;
        } else {
            ungetc(c, stdin);
            strcpy(token, "(");
            return tokentype = '(';
        }
    } else if (c == '[') {
        *p++ = c;
        while ((c = getchar()) != ']' && c != '\n' && c != EOF) {
            *p++ = c;
        }
        if (c == '\n' || c == EOF) {
            printf("error: missing ]\n");
            ungetc(c, stdin);
        }
        *p++ = ']';
        *p = '\0';
        return tokentype = BRACKETS;
    } else if (isalpha(c)) {
        for (*p++ = c; isalnum(c = getchar()); )
            *p++ = c;
        *p = '\0';
        ungetc(c, stdin);
        return tokentype = NAME;
    } else {
        return tokentype = c;
    }
}

int is_type(char *s) {
    return strcmp(s, "char") == 0 || strcmp(s, "int") == 0 ||
           strcmp(s, "void") == 0 || strcmp(s, "float") == 0 ||
           strcmp(s, "double") == 0 || strcmp(s, "const") == 0 ||
           strcmp(s, "volatile") == 0;
}

void dcl(void) {
    int ns = 0;
    char quals[50][128];
    int i;

    for (i = 0; i < 50; i++) quals[i][0] = '\0';

    while (gettoken() == '*') {
        while (gettoken() == NAME && 
              (strcmp(token, "const") == 0 || strcmp(token, "volatile") == 0)) {
            if (quals[ns][0] != '\0') strcat(quals[ns], " ");
            strcat(quals[ns], token);
        }
        ungettoken();
        ns++;
    }
    dirdcl();
    while (ns-- > 0) {
        strcat(out, " ");
        if (quals[ns][0] != '\0') {
            strcat(out, quals[ns]);
            strcat(out, " ");
        }
        strcat(out, "pointer to");
    }
}

void dirdcl(void) {
    int type;

    if (tokentype == '(') {
        dcl();
        if (tokentype != ')') {
            printf("error: missing )\n");
        }
    } else if (tokentype == NAME) {
        strcpy(name, token);
    }

    while ((type = gettoken()) == PARENS || type == BRACKETS || type == '(') {
        if (type == PARENS) {
            strcat(out, " function returning");
        } else if (type == '(') {
            strcat(out, " function returning");
            while ((type = gettoken()) != ')' && type != '\n' && type != EOF)
                ;
            if (type != ')') {
                printf("error: missing )\n");
                if (type == '\n' || type == EOF) {
                    ungetc(type, stdin);
                    tokentype = '\n';
                    break;
                }
            }
        } else if (type == BRACKETS) {
            strcat(out, " array");
            strcat(out, token);
            strcat(out, " of");
        }
    }
}

int main(int argc, char *argv[]) {
    int type;
    char temp[12000];

    if (argc < 2) return 1;

    if (strcmp(argv[1], "-dcl") == 0) {
        while (gettoken() != EOF) {
            if (tokentype == '\n') continue;

            datatype[0] = '\0';
            while (tokentype == NAME && is_type(token)) {
                if (datatype[0] != '\0') strcat(datatype, " ");
                strcat(datatype, token);
                gettoken();
            }
            ungettoken();

            out[0] = '\0';
            name[0] = '\0';

            dcl();

            if (tokentype != '\n' && tokentype != EOF) {
                printf("syntax error\n");
                while (tokentype != '\n' && tokentype != EOF) {
                    gettoken();
                }
            } else {
                printf("%s: %s %s\n", name, out, datatype);
            }
        }
    } else if (strcmp(argv[1], "-undcl") == 0) {
        while (gettoken() != EOF) {
            if (tokentype == '\n') continue;

            strcpy(out, token);
            while ((type = gettoken()) != '\n' && type != EOF) {
                if (type == PARENS || type == BRACKETS) {
                    strcat(out, token);
                } else if (type == '*') {
                    int next_type = gettoken();
                    ungettoken();
                    if (next_type == PARENS || next_type == BRACKETS) {
                        sprintf(temp, "(*%s)", out);
                    } else {
                        sprintf(temp, "*%s", out);
                    }
                    strcpy(out, temp);
                } else if (type == NAME) {
                    sprintf(temp, "%s %s", token, out);
                    strcpy(out, temp);
                } else {
                    printf("invalid input at %s\n", token);
                }
            }
            printf("%s\n", out);
        }
    }
    return 0;
}