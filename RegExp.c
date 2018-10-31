#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE (1)                /* return value */
#define FALSE (0)               /* return value */
#define MAX_LENGTH (100)        /* max expression and string length */
#define MAX_MESSAGE_LENGTH (100)/* max message length */
#define MAX_INT (2147483647)    /* max int value */

// error message code
#define MALLOC_FAILED (0)       /* failed to malloc enough space */

#define MAIN_MODE (-1)          /* to identify different mode,sometimes different mode acts differently */
// expression type code
#define CHAR (0)                /* match a character */
#define DOT (1)                 /* '.' */
#define VERTICAL_BAR (2)        /* '|' */
#define SQARE_BRACKET (3)       /* "[]" */
#define SQARE_BRACKET_NO (4)    /* "[^]" */
#define CHAR_RANGE (5)          /* "[-]" */
#define CHAR_RANGE_NO (6)       /* "[^-]" */
#define NUMBER (7)              /* match a single number(0-9) */
#define NUMBER_NO (8)           /* match a single character which is not a number */
#define BLANKS (9)              /* a blank char */
#define BLANKS_NO (10)          /* not a blank char */
#define WORD_CHAR (11)          /* match a char that can be used in a identifier */
#define WORD_CHAR_NO (12)       /* match a char which cannot be used in a identifier */
#define WORD_END (13)           /* match the end of a identifier,but don't move index of string */
#define WORD_END_NO (14)        /* current character must not be end of string */
#define SINGLE_PLUS (15)        /* 
                                   if current expression has repeat times then make it not greedy
                                   ,else make it same as {0,1} 
                                */
#define PARENTHESIS (16)        /* start matching a sub-expression */
#define MINUS (17)              /* 
                                   if it is in sqare bracket and is in middle of two characters
                                   ,then match a character has ASSIC code between this two characters
                                   ,else match just a '-' 
                                */

char error_message[][MAX_MESSAGE_LENGTH] = {
    "Failed to malloc.\n"
};

struct expression{
    int exp_id;             /* identify a expression */     
    char*sa,*sb;            /* two char arrays(without '\0') */
    int lsa,lsb;            /* mark length of two arrays above */
    int greedy;             /* expression is greedy by default,but can modify to greedy */
    int repeat_times_min;   /* minimum times to match this expression */
    int repeat_times_max;   /* maximum times to match this expression */
    struct expression* next;/* next expression in expression list */
    struct expression*child;/* some expressions act differently from main expression list,so make a child list */
};
typedef struct expression expression;

/* main-expression-list's head and current index */
expression *e_list=NULL,*e_list_index=NULL;
/*
01234567890123456789012345678901234567890123456789012345678901
^([a-z0-9_\\.-]+)@([\\da-z\\.-]+)\\.([a-z\\.]{2,6})$
^[a-z\d]+(\.[a-z\d]+)*@([\da-z](-[\da-z])?)+(\.{1,2}[a-z]+)+$
^w[o-o][^s-z]\Bd\b. n?ui*m:\dn{0,2}un{0,12}m:\D\s\n\w$
0123456789012345678901234
mjb6077@mail.ustc.edu.cn
word: nuauam:8nunm:a n_
*/
//////////////////////////////////////////////////////////////////////////////////
// check the expression and generate expression list
//////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************
 * description  :check the begining of the string and expression
 *               then call check to check the body
 * parameter    :void
 * return       :true if matched false if failed
 ********************************************************************************/
int check_begin(void);
/*********************************************************************************
 * description  :check the end of the string and expression
 * parameter    :void
 * return       :true if matched false if failed
 ********************************************************************************/
int check_end(void);
/*********************************************************************************
 * description  :check the expression body to generate the expression list
 * parameter    :mode to distinct between main list and child list
 * return       :true if matched false if failed
 ********************************************************************************/
int check(int mode);
/*********************************************************************************
 * description  :check characters after '/' and generate corresponding expression
 * parameter    :mode to distinct between main list and child list
 * return       :true if matched false if failed
 ********************************************************************************/
int check_backslash(int mode);
/*********************************************************************************
 * description  :check characters after '{' and therefor before '}'
 *               modify current expression if succeed,and won't add new expression
 * parameter    :void
 * return       :true if matched false if failed
 ********************************************************************************/
int check_curly_bracket(void);
/*********************************************************************************
 * description  :check characters after '(' and therefor before ')'
 *               generate a sub-expression 
 *               insert into main-expression-list as child list
 * parameter    :void
 * return       :true if matched false if failed
 ********************************************************************************/
int check_left_parenthesis(void);
/*********************************************************************************
 * description  :check characters after '[' and therefor before ']'
 *               it is a alternative character list
 *               a char expression-child-list 
 * parameter    :void 
 * return       :true if matched false if failed
 ********************************************************************************/
int check_sqare_bracket(void);

//////////////////////////////////////////////////////////////////////////////////
// based on the expression list to match the string 
//////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************
 * description  :check if e can match some characters after current string index
 * parameter    :the expression
 * return       :true if matched false if failed
 ********************************************************************************/
int match_expression(expression* e);
/*********************************************************************************
 * description  :check if the main-expression-list can match the string
 * parameter    :expression-list
 * return       :true if matched false if failed
 ********************************************************************************/
int match_pattern(expression* e_list);

//////////////////////////////////////////////////////////////////////////////////
// helper functions
//////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************
 * description  :handle errors
 * parameter    :error code
 * return       :void
 ********************************************************************************/
void error(int error_code);
/*********************************************************************************
 * description  :print main-expression-list
 * parameter    :head of main-expression-list
 * return       :void
 ********************************************************************************/
void print_tree(expression* e);
/*********************************************************************************
 * description  :check if the char in the index of string is a escape character
 * parameter    :inedx of string
 * return       :true if matched false if failed
 ********************************************************************************/
int is_escape(int index);
/*********************************************************************************
 * description  :insert a node before current index of expression-list
 * parameter    :the node to be inserted
 * return       :void
 ********************************************************************************/
void insert_linked_list(expression* e);
/*********************************************************************************
 * description  :run after generate main-expression-list
 *               do some modification inconvenient to do while generatating
 * parameter    :void
 * return       :true if matched false if failed
 ********************************************************************************/
int modify_tree();
/*********************************************************************************
 * description  :get a expression from expression-list specified by index
 * parameter    :index of expression,list to find from,current index
 * return       :NULL if not found,pointer of the expression if found
 ********************************************************************************/
expression* get_list(int index,expression* list,int *count);
/*********************************************************************************
 * description  :generate a expression struct 
 * parameter    :void
 * return       :the pointer of the new expression
 ********************************************************************************/
expression* init_node(void);
/*********************************************************************************
 * description  :append a expression after current index of main-expression-list
 * parameter    :pointer of expression to be inserted
 * return       :void
 ********************************************************************************/
void append_linked_list(expression* e);
/*********************************************************************************
 * description  :transfer a string to a int 
 * parameter    :str is the string version of a int,result to put the int in
 * return       :true if matched false if failed
 ********************************************************************************/
int my_atoi(char*str,int* result);
/*********************************************************************************
 * description  :hex version of atoi
 * parameter    :just like atoi
 * return       :true if matched false if failed
 ********************************************************************************/
int my_atoi_hex(char*str,int* result);

/* global variables */
// a string represent the expression
char expr[MAX_LENGTH]="^\x77[o-o]|a[^s-z]\\Bd\\b. n??\\8(u*(i{0,3})a)*i*m:\\dn{0,2}un{0,12}m:\\D\\S\\n\\W$";
//"^([a-z0-9_\\.-]+)@([\\da-z\\.-]+)\\.([a-z\\.]{2,6})$";
// a string to check
char str[MAX_LENGTH]="word: nuiaiiam:8nunm:a \n_";
//"aa666@aaa.aaa.aa.aaa";
// length of expr and str,current index of expr and str
int lexpr,lstr,iexpr,istr;

int main(void){    
    lexpr = strlen(expr);
    lstr = strlen(str);
    
    printf("lexpr:%d lstr:%d\n",lexpr,lstr);
    printf(expr);
    printf("\n");
    printf(str);
    
    iexpr = 0;
    istr = 0;
    
    if(check_begin() && modify_tree()){
        //print_tree(e_list);
        if(match_pattern(e_list))
            printf("matched!");
        else 
            printf("failed!");
    }
    else 
        printf("failed!");
    if(!e_list)printf("failed!");
}

int check_begin(){
    //printf("check_begin...iexpr %d istr %d\n",iexpr,istr);
    // if current expr if '^' and current str index is 0
    if(expr[iexpr]=='^' && istr==0){
        iexpr++;
        return check(MAIN_MODE);
    }
    else return FALSE;
}

int check_end(){
    //printf("check_end...iexpr %d istr %d\n",iexpr,istr);
    // check if expr come to the end
    return(expr[iexpr]=='$');
}

int check(int mode){
    //printf("check...iexpr %d istr %d\n",iexpr,istr);
    // check end when expr finished
    if(iexpr>=lexpr-1){
        return(check_end());      
    }
    expression* e;
    switch(expr[iexpr]){
        case '\\':
            iexpr++;
            return check_backslash(mode);
            break;
        case '(': 
            iexpr++;
            return check_left_parenthesis();
            break;
        case ')':// 用于子表达式的返回
            iexpr++;
            return TRUE;
            break;
        case '[': 
            iexpr++;
            return check_sqare_bracket();
            break;
        case ']': // 中括号类型的子表达式返回
            iexpr++;
            return TRUE;
            break;
        case '-':
            // 如果是中括号而且前后都是非转义字符，那么就是匹配字符范围表达式
            if(mode==SQARE_BRACKET && e_list_index && e_list_index->exp_id==CHAR && !is_escape(iexpr+1)){
                e_list_index->exp_id = MINUS;
                e_list_index->sb = (char*)malloc(sizeof(char));
                if(!e->sb)error(MALLOC_FAILED);
                e_list_index->sb[0] = expr[++iexpr];
                iexpr++;
            // just a '-' character
            }else{
                e = init_node();
                e->exp_id = CHAR;
                e->sa = (char*)malloc(sizeof(char));
                if(!e->sa)error(MALLOC_FAILED);
                e->sa[0] = expr[iexpr++];
                append_linked_list(e);
            }
            break;
        case '{':
            iexpr++;
            return check_curly_bracket();
            break;
        case '+':
            iexpr++;
            if(!e_list_index)return FALSE;
            e_list_index->repeat_times_min=1;
            e_list_index->repeat_times_max=MAX_INT;
            break;
        case '*':
            iexpr++;
            if(!e_list_index)return FALSE;
            e_list_index->repeat_times_min=0;
            e_list_index->repeat_times_max=MAX_INT;
            break;
        case '?':
            iexpr++;
            if(!e_list_index)return FALSE;
            // 如果当前的表达式已经被*?{}修饰过了，那么就修改为非贪婪的
            if(e_list_index->repeat_times_min!=1 || e_list_index->repeat_times_max!=1)
                e_list_index->greedy=0;
            // 如果没有就是重复0-1次
            else{
                e_list_index->repeat_times_min=0;
                e_list_index->repeat_times_max=1;                
            }
            break;
        case '.':
            iexpr++;
            e = init_node();
            e->exp_id = DOT;
            append_linked_list(e);
            break;
        case '|':
            // 创建一个新节点，将当前作为其孩子节点
            // 会在生成完所有的表达式之后将其后面的一个表达式移动到其孩子子树上
            iexpr++;
            if(!e_list_index)
                return FALSE;
            e = init_node();
            e->exp_id = VERTICAL_BAR;
            insert_linked_list(e);
            e->child = e_list_index;
            e->next = NULL;
            e_list_index = e;
            break;
        default:{ // 字符
            e = init_node();
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = expr[iexpr++];
            append_linked_list(e);
        }
    }
    // 递归下降
    return check(mode);
}

int check_backslash(int mode){
    //printf("check_backslash...iexpr %d istr %d\n",iexpr,istr);
    expression* e = init_node();
    switch(expr[iexpr]){
        case 'b':
            iexpr++;
            e->exp_id = WORD_END;
            append_linked_list(e);
            break;
        case 'B':
            iexpr++;
            e->exp_id = WORD_END_NO;
            append_linked_list(e);
            break;
        //case 'c':break;
        case 'd':{
            iexpr++;
            e->exp_id = NUMBER;
            append_linked_list(e);
            break;
        }
        case 'D':{
            iexpr++;
            e->exp_id = NUMBER_NO;
            append_linked_list(e);
            break;
        }
        case 'f':{ // 转义字符
            iexpr++;
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = '\f';
            append_linked_list(e);
            break;
        }
        case 'n':{ // 转义字符
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = '\n';
            append_linked_list(e);
            iexpr++;   
            break;
        }
        case 'r':{ // 转义字符
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = '\r';
            append_linked_list(e);
            iexpr++;    
            break;
        }
        case 's':{ // 非任意空白字符
            e->exp_id = BLANKS_NO;
            append_linked_list(e);
            iexpr++; 
            break;
        }
        case 'S':{ // 任意空白字符
            e->exp_id = BLANKS;
            append_linked_list(e);
            iexpr++; 
            break;
        }
        case 't':{ // 转义字符
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = '\t';
            append_linked_list(e);
            iexpr++; 
            break;
        }
        case 'v':{ // 转义字符
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = '\v';
            append_linked_list(e);
            iexpr++;    
            break;
        }
        case 'w':{ // 非标识符字符
            e->exp_id = WORD_CHAR_NO;
            append_linked_list(e);
            iexpr++; 
            break;
        }
        case 'W':{ // 标识符字符
            e->exp_id = WORD_CHAR;
            append_linked_list(e);
            iexpr++; 
            break;
        }
        case 'x':{ // 十六进制的数字表示ASSIC码的字符，固定是两位数字
            // 如果后面没有两个字符
            if(iexpr >= lexpr-2)
                return FALSE;
            // 将十六进制的字符转化为int数
            char hex[] = {expr[iexpr+1],expr[iexpr+2],'\0'};
            int result=-1;
            if(!my_atoi_hex(hex,&result))
                return FALSE;
            // 插入一个字符表达式
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = result;
            append_linked_list(e);
            iexpr+=3;
            break;
        }
        // only support 0-9
        // 向前获取某个元表达式的引用，支持0-9，只计数元表达式，从1开始
        case '0':;
        case '1':;
        case '2':;
        case '3':;
        case '4':;
        case '5':;
        case '6':;
        case '7':;
        case '8':;
        case '9':{
            int index = expr[iexpr]-'0';
            int count = 0;// begin with 1
            expression* temp = get_list(index,e_list,&count);
            // the node must has actual meaning,so we don't need or count its child or next
            // sa and sb with the same opinter is enough,because we don't change it ,just use it.
            e->lsa = temp->lsa;
            e->lsb = temp->lsb;
            e->exp_id = temp->exp_id;
            e->sa = temp->sa;
            e->sb = temp->sb;
            
            append_linked_list(e);
            iexpr++;
            break;
        }
        default:{// 转义字符
            e->exp_id = CHAR;
            e->sa = (char*)malloc(sizeof(char));
            if(!e->sa)error(MALLOC_FAILED);
            e->sa[0] = expr[iexpr++];
            append_linked_list(e);
        }
    }
    return check(mode);
}

int check_curly_bracket(){
    //printf("check_curly_bracket...iexpr %d istr %d\n",iexpr,istr);
    if(!e_list_index)return FALSE;// there must be a expression.
    
    char nstr[MAX_LENGTH]="";
    int index=-1;
    int n=0,m=MAX_INT;
    // 获取{}内部的字符串，并记录','的位置
    int i;
    for(i=iexpr;i<lexpr;i++){
        if(expr[i]=='}'){
            nstr[i-iexpr]='\0';
            break;
        }
        if(expr[i]==',')index=i-iexpr;
        nstr[i-iexpr]=expr[i];
    }
    iexpr=i+1;
    
    if(index==0)return FALSE;
    // 只有一个n且没有','
    else if(index==-1){
        if(!my_atoi(nstr,&n) || n<0)
            return FALSE;
        m=n;
    }
    // n,
    else if(index==strlen(nstr)-1){
        nstr[index]='\0';
        if(!my_atoi(nstr,&n) || n<0)
            return FALSE;
    }
    // n,m
    else{
        if(!my_atoi(nstr+index+1,&m) || m<0)
            return FALSE;   
        nstr[index]='\0';
        if(!my_atoi(nstr,&n) || n<0)
            return FALSE;        
    }
    
    if(n>m)return FALSE;
    
    e_list_index->repeat_times_min = n;
    e_list_index->repeat_times_max = m;
    // 递归下降
    return check(MAIN_MODE);
}

int check_left_parenthesis(){
    // 插入表示一个子表达式的节点
    expression* e = init_node();
    e->exp_id = PARENTHESIS;
    append_linked_list(e);
    // 备份主表达式链表，将主表达式的head和index设置为子表达式节点的child
    expression*e_list_backup=e_list,*e_list_index_backup=e_list_index;
    e_list = NULL;
    e_list_index = NULL;
    // 调用check
    if(!check(PARENTHESIS))return FALSE;
    // 恢复主表达式的head和index
    e_list_index_backup->child = e_list;// 将这个子树放到当前节点的孩子节点
    e_list = e_list_backup;
    e_list_index = e_list_index_backup;
    // 递归下降
    return check(MAIN_MODE);
}

int check_sqare_bracket(){
    expression* e = init_node();
    // 用lsa标记[]最前面是否是'^'
    if(expr[iexpr]=='^'){
        e->lsa=1;
        iexpr++;
    }else{
        e->lsa=0;
    }
    // 插入表示一个子表达式的节点
    e->exp_id = SQARE_BRACKET;     
    append_linked_list(e);
    // 备份
    expression*e_list_backup=e_list,*e_list_index_backup=e_list_index;
    e_list = NULL;
    e_list_index = NULL;
    
    if(!check(SQARE_BRACKET))return FALSE;
    // 恢复备份
    e_list_index_backup->child = e_list;// 将这个子树放到当前节点的孩子节点
    e_list = e_list_backup;
    e_list_index = e_list_index_backup;
    // 递归下降
    return check(MAIN_MODE);
}

// increase istr but not increase iexpr
int match_expression(expression* e){
    //printf("%d %d %d %d ",e->exp_id,e->repeat_times_min,e->repeat_times_max,e->exp_id?-1:e->sa[0]);
    //printf("match_expression...iexpr %d istr %d\n",iexpr,istr);
    switch(e->exp_id){
        case CHAR : 
            if(str[istr]==e->sa[0]){
                istr++;
                return TRUE;
            }
            else 
                return FALSE;
            break;
        case DOT :             
            if(str[istr]!='\n'){
                istr++;
                return TRUE;
            }
            else 
                return FALSE;
            break;
        case VERTICAL_BAR :{
            // 孩子树一定有两个节点
            if(!e->child || !e->child->next)
                return FALSE;
            if(match_expression(e->child) || match_expression(e->child->next))
                return TRUE;
            return FALSE;
        }
        case SQARE_BRACKET :{
            expression* temp = e->child;
            int matched=e->lsa;// 是否为^
            // 寻找到匹配就跳出
            while(temp){
                if(match_expression(temp)){
                    matched++;
                    break;
                }
                temp = temp->next;
            }
            // 是否找到了匹配
            // 返回值由^决定
            if(matched%2==1){
                istr++;
                return TRUE;
            }
            else return FALSE;
            break;            
        } 
        case CHAR_RANGE : 
            for(int i=e->sa[0];i<=e->sb[0];i++)
                if(((char)i)==str[istr]){
                    istr++;
                    return TRUE;
                }
            return FALSE;
            break;
        case CHAR_RANGE_NO :  
            for(int i=e->sa[0];i<=e->sb[0];i++)
                if(((char)i)==str[istr])
                    return FALSE;
            istr++;
            return FALSE;   
            break;
        case NUMBER :  
            if(str[istr]<'0' || str[istr]>'9')
                return FALSE;
            istr++;
            return TRUE;
            break;
        case NUMBER_NO :
            if(str[istr]>='0' && str[istr]<='9')
                return FALSE;
            istr++;
            return TRUE;
            break;
        case BLANKS :  
            switch(str[istr]){
                case ' ' :;
                case '\f':;
                case '\n':;
                case '\r':;
                case '\t':;
                case '\v':istr++;return TRUE;
                default:return FALSE;
            }
            break;
        case BLANKS_NO :  
            switch(str[istr]){
                case ' ' :;
                case '\f':;
                case '\n':;
                case '\r':;
                case '\t':;
                case '\v':return FALSE;
                default:istr++;return TRUE;
            }
            break;
        // 下一个字符应该是构成标识符的字符
        case WORD_CHAR :  
            if((str[istr]>='a' && str[istr]<='z')
                || (str[istr]>='A' && str[istr]<='Z')
                || (str[istr]>='0' && str[istr]<='9')
                || str[istr]=='_'
            ){
                istr++;
                return TRUE;
            }
            return FALSE;
            break;
        // 下一个字符不应该是构成标识符的字符
        case WORD_CHAR_NO :
            if((str[istr]>='a' && str[istr]<='z')
                || (str[istr]>='A' && str[istr]<='Z')
                || (str[istr]>='0' && str[istr]<='9')
                || str[istr]=='_'
            )return FALSE;
            istr++;
            return TRUE;
            break;
        // 下一个字符不应该是构成标识符的字符，代表标识符结束
        case WORD_END:
            if((str[istr]>='a' && str[istr]<='z')
                || (str[istr]>='A' && str[istr]<='Z')
                || (str[istr]>='0' && str[istr]<='9')
                || str[istr]=='_'
            )return FALSE;
            break;
        // 下一个字符应该是构成标识符的字符，代表标识符未结束
        case WORD_END_NO:
            if((str[istr]>='a' && str[istr]<='z')
                || (str[istr]>='A' && str[istr]<='Z')
                || (str[istr]>='0' && str[istr]<='9')
                || str[istr]=='_'
            )return TRUE;
            return FALSE;
            break;
        // 子模式匹配
        case PARENTHESIS:
            return match_pattern(e->child);
            break;
        case MINUS:
            // 这里不需要将istr+1，因为-只会出现在[]中，而[]匹配成功的时候将会增加istr
            // 因为这里匹配失败了，[]中也可能增加istr
            if(str[istr]>=e->sa[0] && str[istr]<=e->sb[0])
                return TRUE;
            else 
                return FALSE;
            break;
        default:return FALSE;
    }
}

// the caller increase iexpr
int match_pattern(expression* e){
    //printf("match_pattern...iexpr %d istr %d\n",iexpr,istr);
    while(e){
        //printf("@%d %d %d %d @",e->exp_id,e->repeat_times_min,e->repeat_times_max,e->exp_id?-1:e->sa[0]);
        // 重复次数
        for(int i=0;i<e->repeat_times_max;i++){
            // 如果是非贪婪的
            if(!e->greedy && i>=e->repeat_times_min)
                break;
            if(!match_expression(e)){
                if(i<e->repeat_times_min)
                    return FALSE;
                else
                    break;
            }
        }
        e = e->next;
    }
    //printf("end match_pattern...\n");
    return TRUE;
}

int modify_tree(){
    expression* temp = e_list;
    while(temp){
        // 将'|'表达式后面的一个表达式移动到其孩子树后面
        if(temp->exp_id==VERTICAL_BAR){
            if(!temp->next || !temp->child)
                return FALSE;
            temp->child->next = temp->next;
            temp->next = temp->next->next;
            temp->child->next->next=NULL;
        }
        temp = temp->next;
    }
    return TRUE;
}

expression* init_node(){
    //printf("initing...\n");
    // printf("%d",sizeof(expression));
    expression* e = (expression*)malloc(sizeof(expression)*1);
    if(!e)error(MALLOC_FAILED);
    e->next=NULL;
    e->child=NULL;
    e->sa = NULL;
    e->sb = NULL;
    e->greedy = 1;
    // 默认只进行一次匹配
    e->repeat_times_min = 1;
    e->repeat_times_max = 1;
    return e;
}

expression* get_list(int index,expression* list,int *count){
    expression* result;
    while(list){
        // 孩子不为空的节点才是元表达式
        if(!list->child){
            //printf("%d\n",*count);
            (*count)++;
            if(*count == index)
                return list;
        }else{
            if((result=get_list(index,list->child,count)))
                return result;
        }
        list = list->next;
    }
    return NULL;
}

void append_linked_list(expression* e){
    //printf("inserting...\n");
    if(!e_list){
        e_list = e;
        e_list_index = e;
    }
    else{
        e_list_index->next = e;
        e_list_index = e;        
    }
}

void insert_linked_list(expression* e){
    if(e_list == e_list_index){
        e->next = e_list;
        e_list = e;
        return ;
    }
    
    expression* temp = e_list;
    while(!temp && temp->next!=e_list_index){
        temp = temp->next;
    }
    
    temp->next = e;
    e->next = e_list_index;
    
    return ;
}

int is_escape(int index){
    switch(expr[index]){
        case '{':;
        case '}':;
        case '[':;
        case ']':;
        case '(':;
        case ')':;
        case '^':;
        case '$':;
        case '+':;
        case '?':;
        case '*':;
        case '|':return TRUE;break;
        case '\\':{
            switch(expr[index+1]){
                case 'b':;
                case 'B':;
                case 'w':;
                case 'W':;
                case 's':;
                case 'S':;
                case 'd':;
                case 'D':return TRUE;
                default:return FALSE;
            }
        }
        default:return FALSE;
    }
}

int my_atoi(char*str,int* result){
    if(str[0]=='\0')return FALSE;
    int minus=0,index=0,temp_result=0;
    if(str[0]=='-'){
        if(str[1]=='\0')return FALSE;
        minus=1;
        index++;
    }
    while(str[index]!='\0'){
        if(str[index]<'0' || str[index]>'9')return FALSE;
        temp_result=temp_result*10+(str[index++]-'0');
    }
    if(minus)temp_result=-temp_result;
    *result=temp_result;
    return TRUE;
}

int my_atoi_hex(char*str,int* result){
    if(str[0]=='\0')return FALSE;
    int minus=0,index=0,temp_result=0;
    if(str[0]=='-'){
        if(str[1]=='\0')return FALSE;
        minus=1;
        index++;
    }
    int temp=0;
    while(str[index]!='\0'){
        if( !(
            (str[index]>='0' && str[index]<='9')
            ||(str[index]>='A' && str[index]<='Z')
            ||(str[index]>='a' && str[index]<='z')
        ))
            return FALSE;
        if(str[index]>='0' && str[index]<='9'){
            temp = str[index]-'0';
        }else{
            if(str[index]>='A' && str[index]<='Z')
                temp = str[index]-'A'+10;
            else 
                temp = str[index]-'a'+10;
        }
        index++;
        temp_result=temp_result*16+temp;
    }
    if(minus)temp_result=-temp_result;
    *result=temp_result;
    return TRUE;
}

void error(int error_code){
    printf(error_message[error_code]);
    exit(-1);
}

void print_tree(expression* e){
    expression*echild=NULL;
    while(e){
        printf("%d min:%d max:%d--",e->exp_id,e->repeat_times_min,e->repeat_times_max);
        if(e->child){
            printf("[");
            print_tree(e->child);
            printf("]");
        }
        e=e->next;
    }
}