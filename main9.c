putmac(c)
        char c;
{       macq[macptr]=c;
        if(macptr<macmax)macptr++;
        return c;
}
