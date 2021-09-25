extern void __div0(void);

unsigned int __udivmodsi4(unsigned int num, unsigned int den, unsigned int *rem_p)
{

    unsigned int quot = 0, qbit = 1;

    if (den == 0)
        return 0;

    while ((signed int)den >= 0)
    {

        den <<= 1;
        qbit <<= 1;

    }

    while (qbit)
    {

        if (den <= num)
        {

            num -= den;
            quot += qbit;

        }

        den >>= 1;
        qbit >>= 1;

    }

    if (rem_p)
        *rem_p = num;

    return quot;

}

