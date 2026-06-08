#include <stdio.h>
#include <stdlib.h>  /* for atof() */
#include <ctype.h>
#include <string.h>
#include <math.h>    /* for mathematical functions and macros */

#define MAXOP 100     /* max size of operand or operator */
#define NUMBER '0'    /* signal that a number was found */
#define MAXVAL 100    /* maximum depth of val stack */

int top = 0;          /* next free stack position */
double stack[MAXVAL]; /* value stack */

int getop(char []);
void push(double);
double pop(void);
int getch(void);
void ungetch(int);

int main(void)
{
    int type;
    double op2;
    char s[MAXOP];
    double saved[26]; /* array to store variables */
    int i;
    double temp;      /* temporary variable for swap */

    /* initialize variables to zero */
    for (i = 0; i < 26; i++) {
        saved[i] = 0.0;
    }

    while ((type = getop(s)) != EOF) {
        switch (type) {
            case '\n':
                if (top > 0) {
                    printf("\t%.8g\n", pop());
                }
                break;
            case '+':
                push(pop() + pop());
                break;
            case '*':
                push(pop() * pop());
                break;
            case '-':
                op2 = pop();
                push(pop() - op2);
                break;
            case '/':
                op2 = pop();
                if (op2 != 0.0) {
                    push(pop() / op2);
                } else {
                    printf("error: zero divisor\n");
                }
                break;
            case '%':
                op2 = pop();
                if (op2 != 0.0) {
                    push(fmod(pop(), op2));
                } else {
                    printf("error: zero divisor in modulus\n");
                }
                break;
            case NUMBER:
                push(atof(s));
                break;
            case 'v':                               /* push variable value onto stack */
                push(saved[s[0] - 'a']);
                break;
            case '=':
                if (islower((unsigned char)s[0])) {
                    if (top > 0) {
                        saved[s[0] - 'a'] = pop();  /* pop and assign variable */
                    } else {
                        printf("error: stack empty\n");
                    }
                } else {
                    printf("error: invalid variable name\n");
                }
                break;
            case 'f':                               /* handle math functions and stack operations */
                if (strcmp(s, "clear") == 0) {
                    top = 0;
                } else if (strcmp(s, "print") == 0) {
                    if (top > 0) {
                        printf("\t%.8g\n", stack[top - 1]);
                    } else {
                        printf("stack empty\n");
                    }
                } else if (strcmp(s, "dup") == 0) {
                    if (top > 0) {
                        push(stack[top - 1]);
                    } else {
                        printf("stack empty, cannot duplicate\n");
                    }
                } else if (strcmp(s, "swap") == 0) {
                    if (top >= 2) {
                        temp = stack[top - 1];
                        stack[top - 1] = stack[top - 2];
                        stack[top - 2] = temp;
                    } else {
                        printf("stack has fewer than 2 elements\n");
                    }
                } else if (strcmp(s, "pow") == 0) {
                    op2 = pop();
                    push(pow(pop(), op2));
                } else if (strcmp(s, "exp") == 0) {
                    push(exp(pop()));
                } else if (strcmp(s, "sin") == 0) {
                    push(sin(pop()));
                } else if (strcmp(s, "cos") == 0) {
                    push(cos(pop()));
                } else if (strcmp(s, "tan") == 0) {
                    push(tan(pop()));
                } else {
                    printf("error: unknown function %s\n", s);
                }
                break;
            default:
                printf("error: unknown command %s\n", s);
                break;
        }
    }
    return 0;
}

/* push: push f onto value stack */
void push(double f)
{
    if (top < MAXVAL) {
        stack[top] = f;
        top++;
    } else {
        printf("error: stack full, can't push %g\n", f);
    }
}

/* pop: pop and return top value from stack */
double pop(void)
{
    if (top > 0) {
        top--;
        return stack[top];
    } else {
        return 0.0;
    }
}

/* getop: get next operator or numeric operand */
int getop(char s[])
{
    int k, c, n;

    while ((c = getch()) == ' ' || c == '\t')
        ;
    s[0] = c;
    s[1] = '\0';

    if (isalpha(c)) {
        k = 0;
        while (isalpha(s[++k] = c = getch()))
            ;
        s[k] = '\0';
        if (c != EOF) {
            ungetch(c);
        }

        if (strlen(s) == 1) {
            n = getch();
            while (n == ' ' || n == '\t') {
                n = getch();
            }
            if (n == '=') {
                return '=';
            } else {
                if (n != EOF) {
                    ungetch(n);
                }
                return 'v';
            }
        }
        return 'f';
    }

    if (!isdigit(c) && c != '.' && c != '-') {
        return c;
    }

    k = 0;
    if (c == '-') {
        n = getch();
        if (isdigit(n) || n == '.') {
            c = n;
            s[++k] = c;
        } else {
            if (n != EOF) {
                ungetch(n);
            }
            return '-';
        }
    }

    if (isdigit(c)) {
        while (isdigit(s[++k] = c = getch()))
            ;
    }
    if (c == '.') {
        while (isdigit(s[++k] = c = getch()))
            ;
    }
    s[k] = '\0';
    if (c != EOF) {
        ungetch(c);
    }
    return NUMBER;
}

static int ch = '\0'; /* internal static buffer for pushed-back character */

/* getch: get a (possibly pushed back) character */
int getch(void)
{
    int t;
    if (ch != '\0') {
        t = ch;
        ch = '\0';
        return t;
    }
    return getchar();
}

/* ungetch: push character back on input */
void ungetch(int c)
{
    if (ch != '\0') {
        printf("ungetch: too many characters\n");
    } else {
        ch = c;
    }
}