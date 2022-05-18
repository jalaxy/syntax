#include <cstdio>
#include <cstring>
char s[32768];
int main()
{
    FILE *fin = fopen("test_convert_re_input.txt", "rt"),
         *fout = fopen("test_convert_re_output.txt", "wt");
    fscanf(fin, "%s", s);
    for (int i = 0; i < strlen(s); i++)
        if (s[i] == '|')
            fprintf(fout, "%c", '+');
        else if (s[i] != '.')
            fprintf(fout, "%c", s[i]);
}