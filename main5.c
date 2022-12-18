needsub()
        {
        int num[1];
        if(match("]"))return 0; /* null size */
        if (number(num)==0)     /* go after a number */
                {error("must be constant");     /* it isn't * /
                num[0]=l;               /* so force one */
                }
        if (num[0]<0)
                {error("negative size illegal");
                num[0]=(-num[0]);
                }
        needbrack("]");         /* force single dimension */
        return num[0];          /* and return size */
        }
