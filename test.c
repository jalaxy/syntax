void print(int n);
void println();
int f(int n);
int f(int n)
{
    if (n == 0)
        return 0;
    if (n == 1)
        return 1;
    return f(n - 1) + f(n - 2);
}
int main()
{
    int i;
    i = 0;
    while (i < 40)
    {
        print(f(i));
        i = i + 1;
    }
    println();
    return 0;
}
