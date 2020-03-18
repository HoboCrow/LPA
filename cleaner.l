%{
    int c = 0;
%}
%X IN
%%
"#ifdef DEBUG" BEGIN(IN); c++;
<IN>"ifdef"|"ifndef" c++;
<IN>"#endif"\n?   if (--c == 0) BEGIN(0);
<IN>.|\n       ;
%%
int main(int argc, const char** argv)
{
    yylex();
    return 0;
}

int yywrap()
{
return 1;
}