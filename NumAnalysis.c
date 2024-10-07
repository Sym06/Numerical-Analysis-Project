#include <ctype.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#define EPSILON 0.0001
#define h 0.001

enum {ASSOC_NONE = 0, ASSOC_LEFT, ASSOC_RIGHT};

typedef struct {
    char* operator;
    int prec;
    int assoc;
    float(*func)(float a, float b);
} Operator;

typedef struct {
    char *name;
    float (*func)(float val);
} Function;

typedef struct {
    char* input;
} Var;

typedef struct Node {
    Var Variable;
    struct Node* next;
} Node;

float eval_exp(float a, float b) { return powf(a, b); }
float eval_add(float a, float b) { return a+b; }
float eval_sub(float a, float b) { return a-b; }
float eval_mult(float a, float b) { return a*b; }
float eval_div(float a, float b) { return a/b; }
float eval_log(float a, float b) { return log10f(a)/log10f(b); }

float eval_sin(float angle) { return sinf(angle); }
float eval_cos(float angle) { return cosf(angle); }
float eval_tan(float angle) { return tanf(angle); }
float eval_csc(float angle) { return 1/sinf(angle); }
float eval_sec(float angle) { return 1/cosf(angle); }
float eval_cot(float angle) { return 1/tanf(angle); }
float eval_arcsin(float angle) { return asinf(angle); }
float eval_arccos(float angle) { return acosf(angle); }
float eval_arctan(float angle) { return atanf(angle); }

Node* createNode(Var data);
void insertAtBeginning(Node** headref, Var data);
void popTop(Node** headref);
void freeList(Node* head);
void printList(Node* head);

void parse(char* func, Var** infix);
void shuntingYard(Var* infix, Var** postfix);
float evalPostfix(Var* postfix, float x);

int matchStrings(char* string, int structNum);
int findStruct(char* string, int structNum);
int isInt(char* string);

float bisection(float a, float b, Var* postfix);
float regula_falsi(float a, float b, Var* postfix);
float newton_raphson(float a, float b, Var* postfix);
float numerical_derivative(Var* postfix);
float simpsons(float a, float b, Var* postfix);
float trapezoidal(float a, float b, Var* postfix);
float gregory_newton(Var* postfix);
void inverse_matrix();
void gauus_elimination();
void gauss_seidal();

void takeIntervals(float *a, float *b, Var* postfix);
float derive(Var* postfix, float x);

Function functions[] = {
    {"\0", NULL},
    {"log", NULL},
    {"arcsin", eval_arcsin},
    {"arccos", eval_arccos},
    {"arctan", eval_arctan},
    {"sin", eval_sin},
    {"cos", eval_cos},
    {"tan", eval_tan},
    {"csc", eval_csc},
    {"sec", eval_sec},
    {"cot", eval_cot},
    {"end"}
};
Operator operators[] = {
    {"\0", 0, ASSOC_NONE, NULL},
    {"_", 1, ASSOC_RIGHT, eval_log},
    {"^", 3, ASSOC_RIGHT, eval_exp},
    {"*", 2, ASSOC_LEFT, eval_mult},
    {"/", 2, ASSOC_LEFT, eval_div},
    {"+", 1, ASSOC_LEFT, eval_add},
    {"-", 1, ASSOC_LEFT, eval_sub},
    {"end"}
};
int size = 0;

int main() {
    char* input = (char*) calloc(100, sizeof(char));
    Var* infix = (Var*) calloc(100, sizeof(Var));
    Var* postfix = (Var*) calloc(size, sizeof(Var));

    int funcNeeded[] = {1, 2, 3, 7, 8, 9};

    int methodSelected = 0;
    printf("Choose a method:\n");
    printf("1. Bisection Method\n");
    printf("2. Regula-Falsi Method\n");
    printf("3. Newton-Raphson\n");
    printf("4. Inverse of an NxN Matrix\n");
    printf("5. Gauus Elimination\n");
    printf("6. Gauss Seidal Methods\n");
    printf("7. Numerical Derviative\n");
    printf("8. Simpson Method\n");
    printf("9. Trapezoidal Method\n");
    printf("10. Gregory Newton Interpolation\n");
    printf("Method (1-10): ");
    scanf("%d", &methodSelected);

    if (methodSelected > 10 || methodSelected < 1) {
        printf("out of bounds.");
        exit(1);
    }

    getchar();

    for (int i = 0; i < 6; i++) {
        if (methodSelected == funcNeeded[i]) {
            printf("(Use _ for base, ^ for exponent, *,/,+,- for arithmetics, arc for inverse trig, csc/sec/cot for reciprocals of sin/cos/tan)\n");
            printf("Input Function: ");
            fgets(input, 100, stdin);

            input = (char*) realloc(input, strlen(input) + 1);
            for (int i = 0; input[i] != '\0'; i++) {
                if (input[i] == '\n') {
                    input[i] = '\0';
                }
            }

            parse(input, &infix);
            free(input);
            
            shuntingYard(infix, &postfix);
            free(infix);
        }
    }

    float a = 0;
    float b = 0;
    float root = 0;

    switch (methodSelected) {
        case 1:
            takeIntervals(&a, &b, postfix);
            root = bisection(a, b, postfix);
            printf("Root is %lf", root);
            break;
        case 2:
            takeIntervals(&a, &b, postfix);
            root = regula_falsi(a, b, postfix);
            printf("Root is %lf", root);
            break;
        case 3:
            takeIntervals(&a, &b, postfix);
            root = newton_raphson(a, b, postfix);
            printf("Root is %lf", root);
            break;
        case 4:
            inverse_matrix();
            break;
        case 5:
            gauus_elimination();
            break;
        case 6:
            gauss_seidal();
            break;
        case 7:
            root = numerical_derivative(postfix);
            printf("Answer is %lf", root);
            break;
        case 8:
            takeIntervals(&a, &b, postfix);
            root = simpsons(a, b, postfix);
            printf("Answer is %lf", root);
            break;
        case 9:
            takeIntervals(&a, &b, postfix);
            root = trapezoidal(a, b, postfix);
            printf("Answer is %lf", root);
            break;
        case 10:
            root = gregory_newton(postfix);
            printf("Answer is %lf", root);
            break; 

        free(postfix);

    }
}

void parse(char* func, Var** infix) {
    char buffer = '\0';
    int i = 0;

    int l, k;
    for (l = 0, k = 0; func[l]; l++) {
        if (func[l] != ' ') {
            func[k++] = func[l];
        }
    }
    func = (char*) realloc(func, k * sizeof(char));
    func[k] = '\0';

    while (func[i] != '\0') {
        Var variable;
        variable.input = (char*) calloc(100, sizeof(char));
        int j = 0;
        int bool = 0;

        if (buffer != '\0') {
            strncat(variable.input, &buffer, 1);
            bool = 0;
        } 
        
        if (isdigit(func[i]) || func[i] == '.') {
            do {
                strncat(variable.input, &func[i++], 1);
                if (isdigit(func[i]) || func[i] == '.') {
                    j++;
                }
            } while ((isdigit(func[i]) || func[i] == '.') && func[i] != '\0');
        } else if (isalpha(func[i])) {
            do {
                strncat(variable.input, &func[i++], 1);
                if (size > 0 && (((*infix)[size-1].input[0] == '-' || (*infix)[size-1].input[0] == '+') ? isdigit((*infix)[size-1].input[1]):isdigit((*infix)[size-1].input[0])) && strcmp(variable.input, "x") == 0) {
                    Var tempVar;
                    tempVar.input = strdup("*");
                    (*infix)[size++] = tempVar;
                    *infix = realloc(*infix, (size+1) * sizeof(Var));
                }
                if (isalpha(func[i])) {
                    j++;
                }
            } while (isalpha(func[i]) && func[i] != '\0');
        } else if (func[i] == '_') {
            if ((!isdigit(func[i-1]) && strcmp(&func[i-1], "x") != 0) && func[i-1] != ')') {
                buffer = func[i++];
                bool = 1;
            } else {
                strncat(variable.input, &func[i++], 1);
            }
        } else {
            strncat(variable.input, &func[i++], 1);
        }

        if (buffer == '\0') {
            variable.input = realloc(variable.input, (j+1) * sizeof(char));
            variable.input[j+1] = '\0';

            (*infix)[size++] = variable;
            *infix = realloc(*infix, (size+1) * sizeof(Var));
        } else if (buffer != '\0' && bool == 0) {
            variable.input = realloc(variable.input, (j+2) * sizeof(char));
            variable.input[j+2] = '\0';

            (*infix)[size++] = variable;
            *infix = realloc(*infix, (size+1) * sizeof(Var));
            buffer = '\0';
            bool = 0;
        }
    }
}

void shuntingYard(Var* infix, Var** postfix) {
    Node* stack = NULL;
    Var* queue = (Var*) calloc(size, sizeof(Var));

    int j = 0;
    for (int i = 0; i < size; i++) {
        char* head = "\0";
        if (stack != NULL) {
            head = stack->Variable.input;
        }

        int matchHeadTrig = matchStrings(head, 1);
        int headPrec = operators[findStruct(head, 0)].prec;
        int infixPrec = operators[findStruct(infix[i].input, 0)].prec;
        int headAssoc = operators[findStruct(head, 0)].assoc;

        if (isInt(infix[i].input)) {
            queue[j++].input = infix[i].input;
        } else if (isalpha(*infix[i].input) && !matchStrings(infix[i].input, 1)) {
            queue[j++].input = infix[i].input;
        } else if (matchStrings(infix[i].input, 1) || strchr(infix[i].input, '_') || strcmp(infix[i].input, "log") == 0) {
            insertAtBeginning(&stack, infix[i]);
        } else if (matchStrings(infix[i].input, 0)) {
            while (stack != NULL && ((matchHeadTrig || headPrec > infixPrec || strchr(head, '_') || (headPrec == infixPrec && headAssoc == ASSOC_LEFT)) && *head != '(')) {
                queue[j++] = stack->Variable;
                popTop(&stack);
                if (stack != NULL) {
                    head = stack->Variable.input;
                    matchHeadTrig = matchStrings(head, 1);
                    headPrec = operators[findStruct(head, 0)].prec;
                    headAssoc = operators[findStruct(head, 0)].assoc;
                }
            }
            insertAtBeginning(&stack, infix[i]);
        } else if (*infix[i].input == '(') {
            insertAtBeginning(&stack, infix[i]);
        } else if (*infix[i].input == ')') {
            while (*stack->Variable.input != '(') {
                queue[j++] = stack->Variable;
                popTop(&stack);
            }
            popTop(&stack);
        }
    }

    while (stack != NULL) {
        queue[j++] = stack->Variable;
        popTop(&stack);
    }

    size = j;
    *postfix = realloc(queue, size * sizeof(Var));
    freeList(stack);
}

float evalPostfix(Var* postfix, float x) {
    Node* stack = NULL;
    int j = 0;
    for (int i = 0; i < size; i++) {
        if (isInt(postfix[i].input)) { 
            insertAtBeginning(&stack, postfix[i]);
        } else if (matchStrings(postfix[i].input, 0)) { 
            Var var;
            var.input = (char*) malloc(32 * sizeof(char));
            float b = strtof(stack->Variable.input, NULL);
            popTop(&stack);
            float a = strtof(stack->Variable.input, NULL);
            popTop(&stack);

            float answer = operators[findStruct(postfix[i].input, 0)].func(a, b);

            gcvt(answer, 6, var.input);
            insertAtBeginning(&stack, var);
        } else if (matchStrings(postfix[i].input, 1) && strcmp(postfix[i].input, "log") != 0) { 
            Var var;
            var.input = (char*) malloc(32 * sizeof(char));
            int index = findStruct(postfix[i].input, 1);
            float val = 0.0;
            float num = (index > 4) ? strtof(stack->Variable.input, NULL) * M_PI / 180 : strtof(stack->Variable.input, NULL);
            popTop(&stack);

            val = functions[findStruct(postfix[i].input, 1)].func(num);

            gcvt(val, 6, var.input);
            insertAtBeginning(&stack, var);
        } else if (isalpha(*postfix[i].input) && !matchStrings(postfix[i].input, 1) && strcmp(postfix[i].input, "log") != 0) {
            Var var;
            var.input = (char*) malloc(32 * sizeof(char));

            if (strcmp(postfix[i].input, "x") == 0) {
                gcvt(x, 6, var.input);
            } else if (strcmp(postfix[i].input, "e") == 0) {
                gcvt(M_E, 6, var.input);
            } else if (strcmp(postfix[i].input, "pi") == 0) {
                gcvt(M_PI, 6, var.input);
            }

            insertAtBeginning(&stack, var);
        } else if (strchr(postfix[i].input, '_')) { 
            Var var;
            var.input = (char*) malloc(32 * sizeof(char));
            float a = strtof(stack->Variable.input, NULL);
            float b = strtof(postfix[i].input + 1, NULL); 
            popTop(&stack);

            float answer = eval_log(a, b);

            gcvt(answer, 6, var.input);
            insertAtBeginning(&stack, var);
        }
    }

    float answer = strtof(stack->Variable.input, NULL);
    freeList(stack);
    return answer;
}

float bisection(float a, float b, Var* postfix) {
    float c;

    printf("i\ta\t\tb\t\tc\t\tf(c)\n");
    for (int i = 0; (fabs(b-a) >= EPSILON); i++) {
        printf("%d\t", i);
        c = (a+b)/2;
        printf("%lf\t", a);
        printf("%lf\t", b);
        printf("%lf\t", c);
        printf("%lf\t\n", evalPostfix(postfix, c));

        if (evalPostfix(postfix, c) == 0) { return c; }

        if (evalPostfix(postfix, c) * evalPostfix(postfix, a) < 0) {
            b = c;
        } else {
            a = c;
        }
    }

    return c;
}

float regula_falsi(float a, float b, Var* postfix) {
    int i = 0;
    float c;
    float fa = evalPostfix(postfix, a);
    float fb = evalPostfix(postfix, b);
    float fc;

    printf("i\ta\t\tb\t\tc\t\tf(c)\n");
    do {
        c = a - (a - b) * evalPostfix(postfix, a) / (evalPostfix(postfix, a) - evalPostfix(postfix, b));
        fc = evalPostfix(postfix, c);
        printf("%d\t", i);
        printf("%lf\t", a);
        printf("%lf\t", b);
        printf("%lf\t", c);
        printf("%lf\t\n", evalPostfix(postfix, c));

        if (evalPostfix(postfix, c) * evalPostfix(postfix, a) < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }
        i++;
    } while (fabs(fc) > EPSILON);

    return c;
}

float newton_raphson(float a, float b, Var* postfix) {
    int i = 0;
    float fa;
    float fb = 0;
    float fc;
    float ga;

    printf("i\ta\t\tf(a)\t\tb\t\tf(b)\n");
    do {
        ga = derive(postfix, a);
        fa = evalPostfix(postfix, a);

        b = a - fa/ga;
        a = b;

        printf("%d\t%lf\t%lf\t%lf\t%lf\n", i, a, fa, b, fb);

        i++;

        fb = evalPostfix(postfix, b);

    } while (fabs(fb) > EPSILON);

    return b;
}

void inverse_matrix() {
    int n;
    printf("Enter size of matrix: ");
    scanf("%d", &n);

    float matrix[n][n];

    printf("Enter elements of matrix:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("(%d, %d): ", i+1, j+1);
            scanf("%f", &matrix[i][j]);
        }
    }

    float identity[n][n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            identity[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    for (int i = 0; i < n; i++) {
        float pivot = matrix[i][i];
        for (int j = 0; j < n; j++) {
            matrix[i][j] /= pivot;
            identity[i][j] /= pivot;
        }
        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = matrix[k][i];
                for (int j = 0; j < n; j++) {
                    matrix[k][j] -= factor * matrix[i][j];
                    identity[k][j] -= factor * identity[i][j];
                }
            }
        }
    }

    printf("Inverse of the matrix:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%f\t", identity[i][j]);
        }
        printf("\n");
    }
}

void gauus_elimination() {
    int numEq;

    printf("Enter num of equations: ");
    scanf("%d", &numEq);

    float matrix[numEq][numEq+1];
    float solution[numEq];

    printf("Enter coefficients of matrix:\n");
    for (int i = 0; i < numEq; i++) {
        for (int j = 0; j <= numEq; j++) {
            printf("(%d, %d): ", i+1, j+1);
            scanf("%f", &matrix[i][j]);
        }
    }

    float factor;

    for (int i = 0; i < numEq - 1; i++) {
        for (int j = i+1; j < numEq; j++) {
            if (matrix[i][i] == 0) {
                exit(1);
            }

            factor = matrix[j][i] / matrix[i][i];
            for (int k = 0; k <= numEq; k++) {
                matrix[j][k] -= factor * matrix[i][k];
            }
        }
    }

    for (int i = numEq-1; i >= 0; i--) {
        solution[i] = matrix[i][numEq];
        for (int j = i+1; j < numEq; j++) {
            solution[i] -= matrix[i][j] * solution[j];
        }
        solution[i] /= matrix[i][i];
    }

    printf("The solution is:\n");
    for (int i = 0; i < numEq; i++) {
        printf("matrix[%d] = %f\n", i+1, solution[i]);
    }
}

void gauss_seidal() {
    int numEq;

    printf("Enter num of equations: ");
    scanf("%d", &numEq);

    float matrix[numEq][numEq+1];
    float solution[numEq];
    float guess[numEq];

    printf("Enter coefficients of matrix:\n");
    for (int i = 0; i < numEq; i++) {
        for (int j = 0; j <= numEq; j++) {
            printf("(%d, %d): ", i+1, j+1);
            scanf("%f", &matrix[i][j]);
        }
    }

    for (int i = 0; i < numEq; i++) {
        guess[i] = 0;
    }

    float sum;

    for (int i = 0; i < 100; i++) {
        float err = 0;
        for (int j = 0; j < numEq; j++) {
            sum = 0;
            for (int k = 0; k < numEq; k++) {
                if (j != k) {
                    sum += matrix[j][k] * guess[k];
                }
            }
            float newguess = (matrix[j][numEq] - sum) / matrix[j][j]; 
            err += fabs(newguess - guess[j]);
            guess[j] = newguess;
        }
    }

    printf("The solution is:\n");
    for(int i = 0; i < numEq; i++) {
        printf("matrix[%d] = %f\n", i, guess[i]);
    }
}

float numerical_derivative(Var* postfix) {
    int method = 0;
    float x = 0;
    printf("Choose a method: \n1. Forward\n2. Backward\n3. Central\nMethod(1-3): ");
    scanf("%d", &method);

    if (method > 3 || method < 1) {
        printf("Out of bounds.");
        exit(1);
    }

    printf("Enter point x: ");
    scanf("%f", &x);

    float derivative = 0; 
    switch (method) {
        case 1:
            derivative = ((evalPostfix(postfix, x+h) - evalPostfix(postfix, x))/h);
            break;
        case 2:
            derivative = ((evalPostfix(postfix, x) - evalPostfix(postfix, x-h))/h);
            break;
        case 3:
            derivative = ((evalPostfix(postfix, x+h) - evalPostfix(postfix, x-h))/(2*h));
            break;
    }

    return derivative;
}

float simpsons(float a, float b, Var* postfix) {
    int method= 0;
    printf("Select a method\n1. (1/3)\n2. (3/8)\n");
    scanf("%d", &method);

    if (method == 1) {
        float step = (b-a)/100;
        float sum = evalPostfix(postfix, a) + evalPostfix(postfix, b);

        for (int i = 1; i < 100; i++) {
            float x = a + i * step;
            if (i % 2 == 0) {
                sum += 2 * evalPostfix(postfix, x);
            } else {
                sum += 4 * evalPostfix(postfix, x);
            }
        }

        return (step / 3) * sum;
    } else if (method == 2) {
        float step = (b-a)/99;
        float sum = evalPostfix(postfix, a) + evalPostfix(postfix, b);

        for (int i = 1; i < 99; i++) {
            float x = a + i * step;
            if (i % 3 == 0) {
                sum += 2 * evalPostfix(postfix, x);
            } else {
                sum += 3 * evalPostfix(postfix, x);
            }
        }

        return (3 * step / 8) * sum;
    }

    return 0;
}

float trapezoidal(float a, float b, Var* postfix) {
    float step = (b-a)/100;
    float sum = 0.5*(evalPostfix(postfix, a) + evalPostfix(postfix, b));

    for (int i = 1; i < 100; i++) {
        float x = a + i * step;
        sum += evalPostfix(postfix, x);
    }

    return sum*step;
}

float gregory_newton(Var* postfix) {
    int count;
    printf("Enter number of data points: ");
    scanf("%d", &count);

    float x[count], y[count];
    printf("Enter x data points:\n");
    for (int i = 0; i < count; i++) {
        scanf("%f", &x[i]);
    }

    printf("Enter y data points:\n");
    for (int i = 0; i < count; i++) {
        scanf("%f", &y[i]);
    }

    float val;
    printf("Enter value to interpolate: ");
    scanf("%f", &val);

    float diff[count];
    for (int i = 0; i < count; i++) {
        diff[i] = y[i];
    }

    float result = y[0];
    float term = 1;

    for (int i = 1; i < count; i++) {
        for (int j = 0; j < count - i; j++) {
            diff[j] = (diff[j + 1] - diff[j]) / (x[j + i] - x[j]);
        }
        term *= (val - x[i - 1]);
        result += (diff[0] * term);
    }

    return result;
}

float derive(Var* postfix, float x) {
    return (evalPostfix(postfix, x+h) - evalPostfix(postfix, x)) / h;
}

Node* createNode(Var data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->Variable = data;
    newNode->next = NULL;
    return newNode;
}

void insertAtBeginning(Node** headref, Var data) {
    Node* newNode = createNode(data);
    newNode->next = *headref;
    *headref = newNode;
}

void popTop(Node** headref) {
    if (*headref != NULL) {
        Node* temp = *headref;
        *headref = (*headref)->next;
        free(temp);
    }
}

void freeList(Node* head) {
    Node* current = head;
    Node* temp;

    while (current != NULL) {
        temp = current->next;
        free(current);
        current = temp;
    }
}

int matchStrings(char* string, int structNum) {
    switch (structNum) {
        case 0:
            for (int i = 0; strcmp(operators[i].operator, "end") != 0; i++) {
                if (strcmp(string, operators[i].operator) == 0) {
                    return 1;
                }
            }
            break;
        case 1:
            for (int i = 0; strcmp(functions[i].name, "end") != 0; i++) {
                if (strcmp(string, functions[i].name) == 0) {
                    return 1;
                }
            }
            break;
    }

    return 0;
}

int findStruct(char* string, int structNum) {
    switch (structNum) {
        case 0:
            for (int i = 0; strcmp(operators[i].operator, "end") != 0; i++) {
                if (strcmp(string, operators[i].operator) == 0) {
                    return i;
                }
            }
            break;
        case 1:
            for (int i = 0; strcmp(functions[i].name, "end") != 0; i++) {
                if (strcmp(string, functions[i].name) == 0) {
                    return i;
                }
            }
            break;
    }

    return 0;
}

int isInt(char* string) {
    if (string[0] == '-' || string[0] == '+') {
        if (isdigit(string[1])) {
            return 1;
        }
    } else if (isdigit(*string)) {
        return 1;
    }

    return 0;
}

void printList(Node* head) {
    Node* temp = head;
    int count = 1;

    while (temp != NULL) {
        printf("%s,", temp->Variable.input);
        temp = temp->next;
        count++;
    }

}

void takeIntervals(float *a, float *b, Var* postfix) {
    printf("Input intervals [a, b]:\n");
    printf("a:");
    scanf("%f", a);
    printf("b:");
    scanf("%f", b);
}