openout()
        {
        kill();                 /* erase line */
        output=0;               /* start with none */
        pl("output filename? "); /* ask...*/
        gets(line); /* get a filename */
        if(ch()==0)return;      /*none given... */
        if((output=fopen(line,"wt"))==NULL) /* it given, open */
                {output=0;      /* can't open */
                error("open failure");
                }
        kill();                 /* erase line */
        }
