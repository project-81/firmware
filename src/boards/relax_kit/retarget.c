#ifdef __cplusplus
extern "C"
{
#endif

    void _exit(int code)
    {
        (void)code;

        while (1)
        {
            ;
        }
    }

#ifdef __cplusplus
}
#endif
