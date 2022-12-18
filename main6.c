/*                                      */
/*      Begin a function                */
/*                                      */
/* Called from "parse" this routine tries to irake a function */
/*      out of what follows. */
newfunc()
        {
        char n[namesize],*ptr;
        if (symname(n)==0)
                {error("illegal function or declaration");
                kill(); /* invalidate line */
                return;
                }
        if(ptr=findglb(n))      /* already in symbol table ? */
                {if(ptr[ident]!=function)multidef(n);
                        /* already variable by that name */
                else if(ptr[offset]==function)multidef(n);
                        /* already function by that name */
                else ptr[offset]=function;
                        /* otherwise we have what was earlier*/
                        /* assumed to be a function */
                }
        /* if not in table, define as a function now */
        else addglb(n,function,cint,function);
        /* we had better see open paren for args... */
        if(match("(")==0)error("missing open paren");
        outstr(n);col();nl();   /* print function name */
        argstk=0;               /* init arg count */
        while(match(")")==0)    /* then count args */
                /* any legal name bumps arg count */
                {if(symname(n))argstk=argstk+2;
                else{error("illegal argument name");junk();}
                blanks();
                /* if not closing paren, should be comma */
                if(streq(line+lptr,")")==0)
                        {if(match(",")==0)
                        error("expected comma");
                        }
                if(endst())break;
                }
        locptr=startloc;        /* "clear" local symbol table*/
        sp=0;                   /* preset stack ptr */

        while(argstk)
                /* now let user declare what types of things */
                /*      those arguments were */
                {if (amatch("char",4)) {getarg(cchar);ns();}
                else if (amatch("int",3)) {getarg(cint);ns();}
                else{error("wrong number args"); break;}
                }
        if(statement()!=streturn)   /* do a statement, but if */
                                    /* it's a return, skip */
                                    /* cleaning up the stack */
                {modstk(0);
                ret();
                }
        sp=0;                       /* reset stack ptr again */
        locptr=startloc;            /* deallocate all locals */
        }
