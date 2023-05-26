int test1(int a, int b)
{
    return a + b;
}

int main(void)
{
    test1(1, 2);

    float a = 0.0f;
    for (int i = 0; i < 1000; i++)
    {
        a *= 2.0f;
        a /= 2.0f;
    }

    return 0;
}
