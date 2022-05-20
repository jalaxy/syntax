int printl(int n);
void printc(char ch);
int f1(int n);
int f1(int n)
{
    if (n)
        return f2(n - 1) * n;
    return 1;
}
int f2(int n)
{
    if (n)
        return f1(n - 1) * n;
    return 1;
}
int main()
{
    printl(f1(10));
    printc(10);
    return 0;
}
