/*                                      */
/*      Declare argument types          */
/*                                      */
/* called from "newfunc" this routine adds an entry in the */
/*      local symbol table for each named argument */
getarg(t)               /* t = cchar or cint */
        int t;
        {
        char n[namesize],c;int j;
        while(1)
                {if(argstk==0) return;  /* no more args */
                if(match("*"))j=pointer;
                        else j=variable;
                if (symname(n)==0) illname();
                if(findloc(n))multidef(n);
                if(match("[")) /* pointer ? */
                /* it is a pointer, so skip all */
                /* stuff between "[]" */
                        {while(inbyte()!="]")
                                if(endst())break;
                        j=pointer;
                        /* add entry as pointer */
                        }
                addloc(n,j,t,argstk);
                argstk=argstk-2;        /* cnt down */
                if(endst())return;
                if(match(",")==0)error("expected comma");
                }
        }
/*                                      */
/*      Statement parser                */
/*                                      */        
/* called whenever syntax requires      */
/*      a stat ement.                   */
/*  this routine performs that statement */
/*  and returns a number telling which one */
statement()
 {      if((ch()==0) & (eof)) return;
        else if(amatch("char",4))
                {declloc(cchar);ns();}
        else if(amatch("int",3))
                {declloc(cint);ns();}
        else if(amatch("{")) compound();
        else if(amatch("if",2))
                {doif(); lastst=stif;}
        else if(amatch("while",5))
                {dowhile(); lastst=stwhile;}
        else if(amatch("return",6))
                {doreturn(); ns(); lastst=streturn;}
        else if(amatch("break",5))
                {dobreak(); ns(); lastst=stbreak;}
        else if(amatch("continue",8))
                {docont(); ns() ; lastst=stcont;}
        else if(match(";"));
        else if(match("#asm"))
                {doasm(); lastst=stasm;}
        /* if nothing else, assume it's an expr expression */
        else{expression(); ns(); lastst=stexp; }
        return lastst;
 }

/*                                      */
/*      Semicolon enforcer              */ 
/*                                      */
/* called whenever syntax requires a semicolon */
ns() {if(match(";")==0)error("missing semicolon");}
/*                                      */
/*      Compound statement              */
/*                                      */
/* allow any number of statements to fall between "{}" */
compound()
        {
        ++ncmp;         /* new level open */
        while(match("}")==0)
                if(eof) return;
                else statement();
        --ncmp;         /* close current level */
        }
/*                                      */
/*      "if" statement                  */
/*                                      */
doif()
        {
        int flev,fsp,flab1,flab2;
        flev=locptr;    /* record current local level */
        fsp=sp;         /* record current stk ptr */
        flab1=getlabel(); /* get label for false branch */
        test(flab1);    /* get expression, and branch false */
        statement();    /* if true, do a statement */
        sp=modstk(fsp); /* then clean up the stack */
        locptr=flev;    /* and deallocate any locals */
        if (amatch("else",4)==0)        /* if...else ? */
                /* simple "if"...print false label */
                {printlabel(flab1);col();nl();
                return;         /* and exit */
                }
        /* an "if...else" statement. */
        jump(flab2=getlabel()); /* jump around false code */
        printlabel(flab1);col();nl();   /* print false label */
        statement();            /* and do "else" clause */
        sp=modstk(fsp);         /*then clean up stk ptr */
        locptr=flev;            /* and deallocate locals */
        printlabel(flab2);col();nl(); /* print true label */
        }
/*                                      */
/* "while" statement                    */
/*                                      */
dowhile()
        {
        int wq[4];              /* allocate local queue */
        wq[wqsym]=locptr;       /* record local level */
        wq[wqsp]=sp;            /* and stk ptr */
        wq[wqloop]=getlabel();  /* and looping label */
        wq[wqlab]=getlabel();   /* and exit label */
        addwhile(wq);           /* add entry to queue */
                                /* (for "break" statement) */
        printlabel(wq[wqloop]);col();nl(); /* loop label */
        test(wq[wqlab]);        /* see if true */
        statement();            /* if so, do a statement */
        jump(wq[wqloop]);       /* loop to label */
        printlabel(wq[wqlab]);col();nl(); /* exit label */
        locptr=wq[wqsym];       /* deallocate locals */
        sp=modstk(wq[wqsp]);    /* clean up stk ptr */
        delwhile();             /* delete queue entry */
        }
/*                                      */
/*      "return statement"              */
/*                                      */
doreturn()
        {
        /* if not end of statement/ get an expression */
        if(endst()==0)expression();
        modstk(0);      /* clean up stk */
        ret();          /* and exit function */
        }
/*                                      */
/*      "break" statement               */
/*                                      */
dobreak()
        {
        int *ptr;
        /* see if any "whiles" are open */
        if ((ptr=readwhile())==0) return;       /* no */
        modstk((ptr[wqsp]));    /* else clean up stk ptr */
        jump(ptr[wqlab]);       /* jump to exit label */
        }
/*                                      */
/*      "continue" statement            */
/*                                      */
docont()
        {
        int *ptr;
        /* see if any "whiles" are open */
        if ((ptr=readwhile())==0) return;       /* no */
        modstk((ptr[wqsp]));    /* else clean up stk ptr */
        jump(ptr[wqloop]);      /* jump to loop label */
        }

/*                                      */
/*      "asm" pseudo-statement          */
/*                                      */
/* enters mode where assembly language statement are */
/*      passed Intact through parser    */
doasm()
        {
        cmode=0;                /* mark mode as "asm" */
        while (1)
                {inline();      /* get and print lines */
                if (match("#endasm")) break;    /* until... */
                if(eof)break;
                outstr(line);
                nl();
                }
        kill();         /* invalidate line */
        cmode=1;        /* then back to parse level */
        }
/*      >>>>>> start of cc3 <<<<<<      */

/*                                      */
/*      Perform a function call         */
/*                                      */
/* called from heir11, this routine will either call */
/*      the named function, or if the supplied ptr is */
/*      zero, will call the contents of HL              */
callfunction(ptr)
        char *ptr;      /* symbol table entry (or 0) */
{       int nargs;
        nargs=0;
        blanks();       /* already saw open paren */
        if(ptr==0)push();       /* calling HL */
        while(streq(line+lptr,")")==0)
                {if(endst())break;
                expression();   /* get an arguirent */
                if(ptr==0)swapstk(); /* don't push addr */
                push(); /* push argument */
                nargs=nargs+2;  /* count args*2 */
                if (match(",")==0) break;
                }
        needbrack(")");
        if(ptr)call(ptr);
        else callstk();
        sp=modstk(sp+nargs);    /* clean up arguments */
}
junk()
{       if(an(inbyte()))
                while(an(ch()))gch();
        else while(an(ch())==0)
                {if(ch()==0)break;
                gch();
                }
        blanks();
}
endst()
{       blanks();
        return ((streq(line+lptr,";")|(ch()==0)));
}
illname()
{       error("illegal symbol name");junk();}

multidef(sname)
        char *sname;
{       error("already defined");
        comment();
        outstr(sname);nl();
}
needbrack(str)
        char *str;
{       if (match(str)==0)
                {error("misslng bracket");
                comment();outstr(str);nl();
                }
}
needlval()
{       error("must be lvalue");
}
findglb(sname)
        char *sname;
{       char *ptr;
        ptr=startglb;
        while(ptr!=glbptr)
                {if(astreq(sname,ptr,namemax))return ptr;
                ptr=ptr+symsiz;
                }
        return 0;
}
findloc(sname)
        char *sname;
{       char *ptr;
        ptr=startloc;
        while(ptr!=locptr)
                {if(astreq(sname,ptr,namemax))return ptr;
                ptr=ptr+symsiz;
                }
        return 0;
}
addglb(sname,id,typ,value)
        char *sname,id,typ;
        int value;
{       char *ptr;
        if(cptr=findglb(sname))return cptr;
        if(glbptr>=endglb)
                {error("global symbol table overflow");
                return 0;
                }
        cptr=ptr=glbptr;
        while(an(*ptr++ = *sname++));   /* copy name */
        cptr[ident]=id;
        cptr[type]=typ;
        cptr[storage]=statik;
        cptr[offset]=value;
        cptr[offset+1]=value>>8;
        glbptr=glbptr+symsiz;
        return cptr;
}
addloc(sname,id,typ,value)
        char *sname,id,typ;
        int value;
{       char *ptr;
        if(cptr=findloc(sname))return cptr;
        if(locptr>=endloc)
                {error("local symbol table overflow");
                return 0;
                }
        cptr=ptr=locptr;
        while(an(*ptr++ = *sname++));   /* copy nane */
        cptr[ident]=id;
        cptr[type]=typ;
        cptr[storage]=stkloc;
        cptr[offset]=value;
        cptr[offset+1]=value>>8;
        locptr=locptr+symsiz;
        return cptr;
}
/* Test if next input string is legal symbol name */
symname(sname)
        char *sname;
{       int k;char c;
        blanks();
        if(alpha(ch())==0)return 0;
        k=0;
        while(an(ch()))sname[k++]=gch();
        sname[k]=0;
        return 1;
}
/* Return next avail Internal label number */
getlabel()
{       return(++nxtlab);
}
/* print specified number as label */
printlabel(label)
        int label;
{       outstr("cc");
        outdec(label);
}
/* Test if given character is alpha */
alpha(c)
        char c;
{       c=c&127;
        return(((c>='a')&(c<='z'))|
                ((c>='A')&(c<='Z'))|
                (c=='_'));
}
/* Test if given character is numeric */
numeric(c)
        char c;
{       c=c&127;
        return((c>='0')&(c<='9'));
}
/* Test if given character is alphanumeric */
an(c)
        char c;
{       return((alpha(c))|(numeric(c)));
}
/* Print a carriage return and a string only to console */
pl(str)
        char *str;
{       int k;
        k=0;
        putchar(eol);
        while(str[k])putchar(str[k++]);
}
addwhile(ptr)
        int ptr[];
{
        int k;
        if (wqptr==wqmax)
                {error("too many active whiles");return;};
        k=0;
        while (k<wqsiz)
                {*wqptr++ = ptr[k++];}
}
delwhile()
        {if(readwhile()) wqptr=wqptr-wqsiz;
        }
readwhile()
{
        if (wqptr==wq){error("no active whiles");return 0;}
        else return (wqptr-wqsiz);
}
ch()
{       return(line[lptr]&127);
}
nch()
{       if(ch()==0)return 0;
                else return(line[lptr+1]&127);
}
gch()
{       if(ch()==0)return 0;
                else return(line[lptr++]&127);
}
kill()
{       lptr=0;
        line[lptr]=0;
}
inbyte()
{
        while(ch()==0)
                {if (eof) return 0;
                inline();
                preprocess();
                }
        return gch();
}
inchar()
{
        if(ch()==0)inline();
        if(eof)return 0;
        return(gch());
}
inline()
{
        int k,unit;
        while(1)
                {if (input==0)openin();
                if(eof)return;
                if((unit=input2)==0)unit=input;
                kill();
                while((k=getc(unit))>0)
                        {if((k==eol)|(lptr>=linemax))break;
                        line[lptr++]=k;
                        }
                line[lptr]=0;   /* append null */
                if (k<=0)
                        {fclose(unit);
                        if(input2)input2=0;
                                else input=0;
                        }
                if(lptr)
                        {if((ctext)&(cmode))
                                {comment();
                                outstr(line);
                                nl();
                                }
                        lptr=0;
                        return;
                        }
                }
}
/*      >>>>>> start of cc4 <<<<<<      */

keepch(c)
        char c;
{       mline[mptr]=c;
        if(mptr<mpmax)mptr++;
        return c;
}
