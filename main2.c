/************************************************/
/*                                              */
/*                  small-c compiler            */
/*                      rev. 1.1                */
/*                    by Ron Cain               */
/*                                              */
/************************************************/

/*      Define system dependent parameters      */

/*      Stand-alone definitions                 */

/* #define NULL 0       */
/* #define eol 13       */
/*      UNIX definitions (if not stand-alone)   */

#include<stdio.h>
#define eol 10
/*      Define the symbol table parameters      */

#define symsiz  14
#define symtbsz 5040
#define numglbs 300
#define startglb symtab
#define endglb startglb+numglbs*symsiz
#define startloc endglb+symsiz
#define endloc symtab+symtbsz-symsiz

/*      Define symbol table entry format        */

#define name     0
#define ident    9
#define type    10
#define storage 11
#define offset  12

/*      System wide name size (for symbols)     */

#define namesize 9
#define namemax  8

/*      Define possible entries for "ident"     */

#define variable 1
#define array   2
#define pointer 3
#define function 4

/*      Define possible entries for "type"      */
#define cchar   1
#define cint    2

/*      Define possible entries for "storage"   */

#define statik  1
#define stkloc  2

/*      Define the "while" statement queue      */

#define wqtabsz 100
#define wqsiz   4
#define wqmax   wq+wqtabsz-wqsiz

/*      Define entry offsets in while queue     */

#define wqsym   0
#define wqsp    1
#define wqloop  2
#define wqlab   3

/*      Define the literal pool                 */

#define litabsz 2000
#define litmax litabsz-1

/*      Define the input line                   */

#define linesize 80
#define linemax linesize-1
#define mpmax   linemax

/*      Define the macro (define) pool          */

#define macqsize 1000
#define macmax macqsize-1

/*      Define statement types (tokens)         */
#define stif    1
#define stwhile 2
#define streturn 3
#define stbreak 4
#define stcont  5
#define stasm   6
#define stexp   7

/*      Now reserve some storage words          */

char    symtab[symtbsz];    /* symbole table */
char    *glbptr,*locptr;    /* ptrs to next entries */
int     wq[wqtabsz];        /* while queue */    
int     *wqptr;             /* ptr to next entry */

char    litq[litabsz];      /* literal pool */
int     litptr;             /* ptr to next entry */

char    macq[macqsize];     /* macro string buffer */
int     macptr;             /* and its index */

char    line[linesize];     /* parsing buffer */
char    mline[linesize];    /* temp macro buffer */
int     lptr,mptr;          /* ptrs into each */

/*      Misc storage    */
int     nxtlab,         /* next avail label # */
        litlab,         /* label # assigned to literal pool */
        sp,             /* compiler relative stk ptr */
        argstk,         /* function arg sp */
        ncmp,           /* # open compound statements */        
        errcnt,         /* # errors in compilation */
        eof,            /* set non-zero on final input eof */
        input,          /* iob # for input file */
        output,         /* iob # for output file (if any) */
        input2,         /* iob # for "include" file */
        glbflag,        /* non-zero if internal globals */
        ctext,          /* non-zero to intermix c-source */
        cmode,          /* non-zero while parsing c-code */
                        /* zero when passing assembly code */
        lastst;         /* last executed statement type */

char    quote[2];       /* literal string fo '"' */
char    *cptr;          /* work ptr to any char buffer */
int     *iptr;          /* work ptr to any int buffer */

/*      >>>>>> start cc1 <<<<<<         */
/*                                      */
/*      Compiler begins execution here  */
/*                                      */

main()
        {
        glbptr=startglb;        /* clear global symbols */
        locptr=startloc;        /* clear local symbols */
        wqptr=wq;               /* clear while queue */
        macptr=         /* clear the macro pool */
        litptr=         /* clear literal pool */
        sp =            /* stack ptr (relative) */
        errcnt=         /* no errors */
        eof=            /* not eof yet */
        input=          /* no input file */
        input2=         /* or include file */
        output=         /* no open units */
        ncmp=           /* no open compound states */
        lastst= /* no last statement yet */
        quote[1]=
        0;              /* ...all set to zero.... */
        quote[0]='"';   /* fake a quote literal */
        cmode=1;        /* enable preprocessing */
        /*                              */
        /*      compiler body           */
        /*                              */
        ask();                  /* get user options */
        openout();              /* get an output file */
        openin();               /* and initial input file */
        header();               /* intro code */
        parse();                /* process ALL input */
        dumplits();             /* then dump literal pool */
        dumpglbs();             /* and all static memory */
        errorsummary();         /* summarize errors */
        trailer();              /* follow-up code */
        closeout();             /* close the output (if any) */
        return;                 /* then exit to system */
        }
/*                                      */
/*      Process all input text          */
/*                                      */
/* At this level, only static declarations, */
/*      defines, includes, and function */
/*      defintions are legal...         */
parse()
        {
        while (eof==0)          /* do until no more input */
                {
                if(amatch("char",4)){declglb(cchar);ns();}
                else if(amatch("int",3)){declglb(cint);ns();}
                else if(match("#asm"))doasm();
                else if(match("#include"))doinclude();
                else if(match("#define"))addmac();
                else newfunc();
                blanks();       /* force eof if pending */
                }
        }
/*                                      */
/*      Dump the literal pool           */
/*                                      */
dumplits()
        {int j,k;
        if (litptr==0) return;  /* if nothing there, exit...*/
        printlabel(litlab);col(); /* print literal label */
        k=0;                    /* init an index... */
        while (k<litptr)        /*      to loop with */
                {defbyte();     /* pseudo-op to define byte */
                j=10;           /* max bytes per line */
                while(j--)
                        {outdec((litq[k++]&127));
                        if ((j==0) | (k>=litptr))
                                {nl();          /* need <cr> */
                                break;
                                }
                        outbyte(",");   /* separate bytes */
                        }
                }
        }

/*                                      */
/*      Dump all static variables       */
/*                                      */
dumpglbs()
        {
        int j;
        if(glbflag==0)return;   /* don't if user said no */
        cptr=startglb;
        while(cptr<glbptr)
                {if(cptr[ident]!=function)
                        /* do if anything but function */
                        {outstr(cptr);col();
                                /* output name as label... */
                        defstorage(); /* define storage */
                        j=((cptr[offset]&255)+
                                ((cptr[offset+1]&255)<<8));
                                        /* calc # bytes */
                        if((cptr[type]==cint)|
                                (cptr[ident]==pointer))
                                j=j+j;
                        outdec(j);      /* need that many */
                        nl();
                        }
                cptr=cptr+symsiz;
                }
        }
/*                                      */
/*      Report errors for user          */
/*                                      */
errorsummary()
        {
        /* see if anything left hanging... */
        if (ncmp) error("missing closing bracket");
                /* open compound statement ... */
        nl();
        comment();
        outdec(errcnt); /* total # errors */
        outstr(" errors in compilation.");
        nl();
        }
