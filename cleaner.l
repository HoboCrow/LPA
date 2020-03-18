
%X IN
%%
"#ifdef DEBUG" BEGIN(IN);
<IN>"#endif"   BEGIN(0);
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