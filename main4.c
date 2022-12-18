/*                                      */
/*      Get options from user           */
/*                                      */
ask()
{
  int k,num[1];
  kill();                 /* clear input line */
  outbyte(12);            /* clear the screen */
  nl();nl();nl();         /* print banner */
  pl("   * * *  small-c compiler  * * *");
  nl();
  pl("           by Ron Cain");
  nl();nl();
  /* see if user wants to interleave the c-text */
  /* in form of comments (for clarity) */
  pl("Do you wish the c-text to appear? ");
  gets(line);             /* get answer */
  ctext=0;                /* assume no */
  if((ch()=='Y')|(ch()=='y'))
    ctext=1; /* user said yes   */
  /* see if user wants us to allocate static */
     /* variables by name In this module */
  /* (pseudo external capability)     */
  pl("Do you wish the globals to be defined?");
  gets(line);
  glbflag=0;
  if((ch()=='Y')|(ch()=='y'))
    glbflag=1; /* user said yes */
  /* get first allowable number for compiler-generated */
  /* labels (in case user will append modules) */
  while(1)
    {pl("Starting number for labels? ");
      gets(line);
      if(ch()==0){num[0]=0;break;}
      if(k=number(num))break;     /*TODO: check assignment correct?? */
    }
  nxtlab=num[0];
  litlab=getlabel();      /* first label=literal pool */
  kill();                 /* erase line */
}

/*                                      */
/*      Get output filename             */
/*                                      */
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
/*                                      */
/*      Get (next) input file           */
/*                                      */        
openin()
{
        input=0;                /* none to start with */
        while (input==0)        /* any above 1 allowed */
                {kill();        /* clear line */
                if(eof)break;   /* it user said none */
                pl("Input filename? ");
                gets(line);     /* get a name */
                if(ch()==0)
                        {eof=1;break;}  /* none given... */
                if((input=fopen(line,"rt"))==NULL)
                        {input=0;       /* can't open it */
                        pl("Open failure");
                        }
                }
        kill();         /* erase line */
        }
/*                                      */
/*      Open an include file            */
/*                                      */
doinclude()
{
        blanks();       /* skip over to name */
        if((input2=fopen(line+lptr,"r"))==NULL)
                {input2=0;
                error("Open failure on include file");
                }
        kill();         /* clear rest of line */
                        /* so next read will cone fron */
                        /* new file (if open */
}
/*                                      */
/*      Close the output file           */
/*                                      */
closeout()
{       if(output)fclose(output); /* if open, close it */
        output=0;               /* mark as closed */
}
/*                                      */
/*      Declare astatic variable        */
/*        (i.e. define for use)         */
/*                                      */
/* makes an entry in the symbol table so subsequent */
/*  references can call symbol by name */
declglb(typ)            /* typ is cchar or cint */
        int typ;
{       int k,j;char sname[namesize];
        while(1)
                {while(1)            
                        {if(endst())return;     /* do line */
                        k=1;            /* assume 1 element */
                        if(match("*"))  /* pointer ? */
                                j=pointer;      /* yes */
                                else j=variable; /* no */
                        if (symname(sname)==0) /* name ok? */
                                illname(); /* no... */
                        if(findglb(sname)) /* already there? */
                                multidef(sname);
                        if (match("["))         /* array? */
                                {k=needsub();   /* get size */
                                if(k)j=array;   /* !0=array */
                                else j=pointer; /* 0=ptr */
                                }
                        addglb(sname,j,typ,k); /* add symbol */
                        break;
                        }
                if (match(",")==0) return; /* more? */
                }
}
/*                                      */
/*      Declare local variables         */
/*      (i.e. define for use)           */
/*                                      */
/* works Just like "declglb" but modifies machine stack */
/*      and adds symbol table entry with appropriate */
/*      stack offset to find it again */
declloc(typ)            /* typ is cchar or cint */
        int typ;
        {
        int k,j;char sname[namesize];
        while(1)
                {while(1)
                        {if(endst())return;
                        if(match("*"))
                                j=pointer;
                                else j=variable;
                        if (symname(sname)==0)
                                illname();
                        if(findloc(sname))
                                multidef(sname);
                        if (match("["))
                                {k=needsub();
                                if (k)
                                        {j=array;
                                        if(typ==cint)k=k+k;
                                        }
                                else
                                        {j=pointer;
                                        k=2;
                                        }
                                }
                        else
                                if((typ==cchar)
                                        &(j!=pointer))
                                        k=1;else k=2;
                        /* change machine stack */
                        sp=modstk(sp-k);
                        addloc(sname,j,typ,sp);
                        break;
                        }
                if (match(",")==0) return;
                }
        }
/*                                      */
/*      >>>>>> start of cc2 <<<<<<      */
/*                                      */
/*      Get required array size         */
/* invoked when declared variable is followed by "[" */
/*      this routine makes subscript the absolute */
/*      size of the array. */
