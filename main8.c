preprocess()
{       int k;
        char c,sname[namesize];
        if(cmode==0) return;
        mptr=lptr=0;
        while(ch())
                {if((ch()==' ')|(ch()==9))
                        {keepch(' ');
                        while((ch()==' ')|
                                (ch()==9))
                                gch();
                        }
                else if(ch()=='"')
                        {keepch(ch());
                        gch();
                        while(ch()!='"')
                                {if(ch()==0)
                                  {error("missing quote");
                                  break;
                                  }
                                keepch(gch());
                                }
                        gch();
                        keepch('"');
                        }
                else if(ch()==39)
                        {keepch(39);
                        gch();
                        while(ch()!=39)
                                {if(ch() ==0)
                                  {error("missing apostrophe");
                                  break;
                                  }
                                keepch(gch());
                                }
                        gch();
                        keepch(39);
                        }
                else if((ch()=='/')&(nch()=='*'))
                        {inchar();inchar();
                        while(((ch()=='*')&
                                (nch()=='/'))==0)
                                {if(ch()==0)inline();
                                        else inchar();
                                if(eof)break;
                                }
                        inchar();inchar();
                        }
                else if(an(ch()))
                        {k=0;
                        while(an(ch()))
                                {if(k<namemax)sname[k++]=ch();
                                gch();
                                }
                        sname[k]=0;
                        if(k=findmac(sname))
                                while(c=macq[k++])
                                        keepch(c);
                        else
                                {k=0;
                                while(c=sname[k++])
                                        keepch(c);
                                }
                        }
                else keepch(gch());
        }
        keepch(0);
        if(mptr>=mpmax)error( "line too long");
        lptr=mptr=0;
        while(line[lptr++]=mline[mptr++]);
        lptr=0;
}
addmac()
{       char sname[namesize];
        int k;
        if(symname(sname)==0)
                {illname();
                kill();
                return;
                }
        k=0;
        while(putmac(sname[k++]));
        while(ch()==' ' | ch()==9) gch();
        while(putmac(gch()));
        if(macptr>=macmax)error("macro table full");
}
putmac(c)
        char c;
{       macq[macptr]=c;
        if(macptr<macmax)macptr++;
        return c;
}
findmac(sname)
        char *sname;
{       int k;
        k=0;
        while(k<macptr)
                {if(astreq(sname,macq+k,namemax))
                        {while(macq[k++]);
                        return k;
                        }
                while(macq[k++]);
                while(macq[k++]);
                }
        return 0;
}
outbyte(c)
        char c;
{
        if(c==0)return 0;
        if(output)
                {if((putc(c,output))<=0)
                        {closeout();
                        error("Output file error");
                        }
                }
        else putchar(c);
        return c;
}
outstr(ptr)
        char ptr[];
{
        int k;
        k=0;
        while(outbyte(ptr[k++]));
}
nl()
        {outbyte(eol);}
tab()
        {outbyte(9);}
col()
        {outbyte(58);}
error(ptr)
        char ptr[];
{       
        int k;
        comment();outstr(line);nl();comment();
        k=0;
        while(k<lptr)
                {if(line[k]==9) tab();
                        else outbyte(' ');
                ++k;
                }
        outbyte('^');
        nl();comment();outstr("*****  ");
        outstr(ptr);
        outstr("  *****");
        nl();
        ++errcnt;
}
ol(ptr)
        char ptr[];
{
        ot(ptr);
        nl();
}
ot(ptr)
        char ptr[];
{
        tab();
        outstr(ptr);
}
streq(str1,str2)
        char str1[],str2[];
{
        int k;
        k=0;
        while (str2[k])
                {if ((str1[k]) != (str2[k])) return 0;
                k++;
                }
        return k;
}

astreq(str1,str2,len)
        char str1[],str2[];int len;
{
        int k;
        k=0;
        while (k<len)
                {if ((str1[k])!=(str2[k]))break;
                if(str1[k]==0)break;
                if(str2[k]==0)break;
                k++;
                }
        if (an(str1[k]))return 0;
        if (an(str2[k]))return 0;
        return k;
}

match(lit)
        char *lit;
{
        int k;
        blanks();
        if (k=streq(line+lptr,lit))
                {lptr=lptr+k;
                return 1;
                }
        return 0;
}
amatch(lit,len)
        char *lit;int len;
{
        int k;
        blanks();
        if (k=astreq(line+lptr,lit,len))
                {lptr=lptr+k;
                while(an(ch())) inbyte();
                return 1;
                }
        return 0;
}
blanks()
        {while(1)
                {while(ch()==0)
                        {inline();
                        preprocess();
                        if(eof)break;
                        }
                if(ch()==' ') gch();
                else if(ch()==9)gch();
                else return;
                }
        }
outdec(number)
        int number;
{
        int k,zs;
        char c;
        zs = 0;
        k=10000;
        if (number<0)
                {number=(-number);
                outbyte('-');
                }
        while (k>=1)
                {
                c=number/k + '0';
                if ((c!='0')|(k==1)|(zs))
                        {zs=1;outbyte(c);}
                number=number%k;
                k=k/10;
                }
}
/*      >>>>>> start of cc5 <<<<<<      */


store(lval)
        int *lval;
{
        if (lval[1]==0)putmem(lval[0]);
        else putstk(lval[1]);
}
rvalue(lval)
        int *lval;
{
        if((lval[0] != 0) & (lval[1] == 0))
                getmem(lval[0]);
                else indirect(lval[1]);
}
test(label)
        int label;
{
        needbrack("(");
        expression();
        needbrack(")");
        testjump(label);
}

constant(val)
        int val[];
{
        if (number(val))
                immed();
        else if (pstr(val))
                immed();
        else if (qstr(val))
                {immed();printlabel(litlab);outbyte('+');}
        else return 0;
        outdec(val[0]);
        nl();
        return 1;
}
number(val)
        int val[];
{
        int k,minus;char c;
        k=minus=1;
        while(k)
                {k=0;
                if (match("+")) k=1;
                if (match("-")) {minus=(-minus);k=1;}
                }
        if(numeric(ch())==0)return 0;
        while (numeric(ch()))
                {c=inbyte();
                k=k*10+(c-'0');
                }
        if (minus<0) k=(-k);
        val[0]=k;
        return 1;
}
pstr(val)
        int val[];
{       int k;char c;
        k=0;
        if (match("'")==0) return 0;
        while((c=gch())!=39)
                k=(k&255)*256 + (c&127);
        val[0]=k;
        return 1;
}
qstr(val)
        int val[];
{       char c;
        if (match(quote)==0) return 0;
        val[0]=litptr;
        while (ch()!='"')
                {if(ch()==0)break;
                if(litptr>=litmax)
                        {error("strlng space exhausted");
                        while(match(quote)==0)
                                if(gch()==0)break;
                        return 1;
                        }
                litq[litptr++]=gch();
                }
        gch();
        litq[litptr++]=0;
        return 1;
}

/*      >>>>>> start of cc8 <<<<<<      */

/* Begin a comment line for the assembler */
comment()
{
        outbyte(';');
}
/* Print all assembler info before any code is generated */
header()
{
        comment();
        outstr("small-c compiler rev 1.1");
        nl();
}
/* Print any assembler stuff needed after all code */
trailer()
{ /* ol("END"); */
}
/* Fetch a static memory cell into the primary register */
getmem(sym)
        char *sym;
{
        if((sym[ident]!=pointer)&(sym[type]==cchar))
                {ot("LDA ");
                outstr(symname);
                nl();
                call("ccsxt");
                }
        else
                {ot("LHLD ");
                outstr(sym+name);
                nl();
                }
}
/* Fetch the address of the specified symbol    */
/*      into the primary register               */
getloc(sym)
        char *sym;
{
        immed();
        outdec(((sym[offset]&255)+
                ((sym[offset+1]&255)<<8))-
                sp);
        nl();
        ol("DAD SP");
        }
/* store the primacy register into the specified */
/*      static memory cell */
putmem(sym)
        char *sym;
{
        if((sym[ident]!=pointer)&(sym[type]==cchar))
                {ol("MOV A,L");
                ot("STA ");
                }
        else ot("SHLD ");
        outstr(sym+name);
        nl();
}
/* Store the specified object type in the primary register */
/*      at the address on the top of the stack          */
putstk(typeobj)
        char typeobj;
{       pop();
        if(typeobj==cchar)call("ccpchar");
                else call("ccpint");
}
/* Fetch the specified object type indirect through the */
/*      primary register into the prirary register      */
indirect(typeobj)
        char typeobj;
{       if(typeobj==cchar)call("ccgchar");
                else call("ccgint");
}
/* Swap the primary and secondary registers */
swap()
{
        ol("XCHG");
}
/* Print partial instruction to get an immediate value  */
/*      into the primary register                       */
immed()
{       ot("LXI H," );
}
/* Push the primary register onto the stack */
push()
{       ol("PUSH H");
        sp=sp-2;
}
/* Pop the top of the stack into the secondary register */
pop()
{       ol("POP D");
        sp=sp+2;
}
/* Swap the primary register and the top of the stack */
swapstk()
{       ol("XTHL");
}
/* Call the specified subroutine name */
call(sname)
        char *sname;
{       ot("CALL ");
        outstr(sname);
        nl();
}
/* Return from subroutine */
ret()
{       ol("RET");
}
/* Perform subroutine call to value on top of stack */
callstk()
{       immed();
        outstr("$+5");
        nl();
        swapstk();
        ol("PCHL");
        sp=sp+2;
}
/* Jump to specified internal label number */
jump(label)
        int label;
{       ot("JMP ");
        printlabel(label);
        nl();
}
/* Test the primary register and jump if false to label */
testjump(label)
        int label;
{       ol("MOV A, H");
        ol("ORA L");
        ot("JZ ");
        printlabel(label);
        nl();
}
/* Print pseudo-op to define a byte */
defbyte()
{       ot("DB ");
}
/* Print pseudo-op to define storage */
defstorage()
{       ot("DS ");
}
/* Print pseudo-op to define a word */
defword()
{       ot("DW ");
}
/* Modify the stack pointer to the new value indicated */
modstk(newsp)
        int newsp;
{       int k;
        k=newsp-sp;
        if(k==0)return newsp;
        if(k>=0)
                {if(k<7)
                        {if (k&1)
                                {ol("INX SP");
                                k--;
                                }
                        while(k)
                                {ol("POP B");
                                k=k-2;
                                }
                        return newsp;
                        }
                }
        if(k<0)
                {if(k>-7)
                        {if(k&1)
                                {ol("DCX SP");
                                k++;
                                }
                        while(k)
                                {ol("PUSH B");
                                k=k+2;
                                }
                        return newsp;
                        }
                }
        swap();
        immed();outdec(k);nl();
        ol("DAD SP");
        ol("SPHL");
        swap();
        return newsp;
}
/* Double the primary register */
doublereg()
        {ol("DAD H");}
/* Add the primary and secondary registers */
/*      .(results in primary) */
add()
        {ol("DAD D");}
/* Subtract the primary register from the secondary * /
/* (results in primary) */
sub()
        {call("ccsub");}
/* Multiply the primary and secondary registers */
/* (results in primary */
mult()
        {call("ccmult");}
/* Divide the secondary register by the primary * /
/* (quotient in primary, remainder in secondary) */
div()
        {call("ccdiv");}
/* Compute remainder (mod) of secondary register divided */
/*      by the primary */
/*      (remainder in primary, quotient in secondary) */
mod()
{
        div();
        swap();
}
/* Inclusive 'or' the primary and the secondary registers */
/*      (results in primary) */
or()
        {call("ccor");}
/* Exclusive 'or* the primary anc seconday registers */
/*      (results In primary) */
xor()
        {call("ccxor");}
/* 'And' the primary and secondary registers */
/*      (results in primary) */
and()
        {call("ccand");}
/* Arithmetic shift right the secondary register number of * /
/* times in primary (results in primary) */
asr()
        {call("ccasr");}
/* Arithmetic left shift the secondary register number ot * /
/*      times in primary (results in primary) */
asl()
        {call("ccasl");}
/* Form tuo's complement of primary register */
neg()
        {call("ccneg");}
/* Form one's complement of primary register */
com()
        {call("cccom");}
/* Increment the primary register by one */
inc()
        {ol("INX H");}
/* Decrement the primary register by one */
dec()
        {ol("DCX H");}
/* Following are the conditional operators */
/* They compare the secondary register against the primary */
/* and put a literal 1 in the primary if the condition is */
/* true, otherwise they clear the primary register */

/* Test for equal */
eq()
        {call("cceq");}

/* Test for not equal */
ne()
        {call("ccne");}
/* Test for less than (signed) */
lt()
        {call("cclt");}
/* Test for less than or equal to (signed) */
le()
        {call("ccle");}
/* Test for greater than (signed) */
gt()
        {call("ccgt");}
/* Test for greater than or equal to (signed) */
ge()
        {call("ccge");}
/* Test for less than (unsigned) */
ult()
        {call("ccult");}
/* Test for less than or equal to (unsigned) */
ule()
        {call("ccule");}
/* Test for greater than (unsigned) */
ugt()
        {call("ccugt");}
/* Test for greater then or equal to (unsigned) */
uge()
        {call("ccuge");}

/*      <<<<< End of compiler >>>>> */

