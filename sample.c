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
  /* see if user wants us to allocate static * /
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
