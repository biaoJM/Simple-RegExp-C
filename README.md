# Simple-RegExp-C
C语言实现非完全正则表达式
=======

因为在写这个程序之前我没有用过正则表达式，所以在看语法的时候有几点困惑的地方。但是这个程序的目的只是练习C语言，所以困惑的点就按照自己的方式实现了

语法：
------

* 正则表达式和待匹配字符串都是一行

* “^” 标记正则表达式的开始

* “$” 标记正则表达式的结束

* “*” 匹配前面的子表达式零次或多次

* “+” 匹配前面的子表达式一次或多次

* “?” 匹配前面的子表达式零次或一次, 当该字符紧跟在任何一个其他限制符（*,+,?，{n}，{n,}，{n,m}）后面时，表示该匹配模式是非贪婪的，而不是匹配前面的子表达式

* “{n}” n是一个非负整数。匹配确定的n次

* “{n,}” n是一个非负整数。至少匹配n次

* “{n,m}” m和n均为非负整数，其中n<=m。最少匹配n次且最多匹配m次

* “.” 匹配除“\n”之外的任何单个字符

* “x|y” 匹配x或y，其中x和y是两个子表达式，如果是字符就是单个字符

* “[xyz]”字符合集，匹配其中的任意一个字符，如果两个字符之间有”-”，就表示这三个字符用于匹配一个ASSIC码值在两字符之间的一个字符，如果开头有”^”，表示负值字符集合。匹配未包含的任意字符

* “(pattern)”一个子表达式，可嵌套

* \b匹配一个单词边界，即是否后面是非标识符字符，只是检测，不匹配实际内容

* \B匹配非单词边界，即是否后面不是非标识符字符，只是检测，不匹配实际内容

* \d 匹配一个数字字符。等价于[0-9]。

* \D 匹配一个非数字字符。等价于[^0-9]。

* \f 匹配一个换页符。等价于\x0c 

* \n 匹配一个换行符。等价于\x0a 

* \r 匹配一个回车符。等价于\x0d

* \s 匹配任何空白字符，包括空格、制表符、换页符等等。等价于[ \f\n\r\t\v]。

* \S 匹配任何非空白字符。等价于[^ \f\n\r\t\v]。

* \t 匹配一个制表符。等价于\x09 

* \v 匹配一个垂直制表符。等价于\x0b 

* \w 匹配包括下划线的任何单词字符。等价于“[A-Za-z0-9_]”。

* \W 匹配任何非单词字符。等价于“[^A-Za-z0-9_]”。

* \xn匹配n，其中n为十六进制转义值。十六进制转义值必须为确定的两个数字长。例如，“\x41”匹配“A”。“\x041”则等价于“\x04&1”。正则表达式中可以使用ASCII编码。.

* \num匹配num，其中num是一个正整数。对所获取的匹配的引用。例如，“(.)\1”匹配两个连续的相同字符。只支持0≤num≤9，并且引用的只能是元表达式，比如(ab)将计数为两个表达式”a”和”b”而忽略括号

策略：
---
先将正则表达式解析，生成一棵语法树，树枝是”()”“[]”“|”带来的子表达式，节点的exp_id表示这个节点将用于匹配怎样的字符，还有字段记录这个表达式将重复的次数。
“()”的子树是和主树的结构一样的，解析规则也是相同的，但是”[]”和”|”的子树有自己的解析规则
每棵子树的根节点不匹配实际内容，而是用于指示它有子树
再用这棵语法树对字符串进行匹配

### Step1：解析语法树

每个函数都只进行一个或特定几个字符的解析，之后递归的调用以递归下降分析

#### Check： 
首先判断是否达到了正则表达式尾

* “|”会在当前节点之前插入一个节点，并将当前节点移到插入的节点的孩子上，因为难以只对之后的一个表达式进行特殊处理，所以之后将继续解析。而是在解析完成之后进行再次的处理，遇到”|”的表达式的时候会将其后面的一个表达式放到其孩子树（此时已有一个节点）的后面，这样孩子树将有两个节点
* “-”会判断当前的模式，如果在一个中括号中，并且当前节点和后面的都是一个非转义字符，那么就修改当前的节点为字符范围匹配，即便后面的是反斜杠也可以判断是否是非转义字符
* “+”“*”会直接修改当前节点的重复次数（即便它有一棵孩子树）
*  “?”需要判断当前的节点是否已经被限制符修饰过了，如果是就将当前节点设置为非贪婪的，否则就只修改重复次数
* 在遇到”(““{““[““\”时会进入相应的函数进行特定的解析
* 递归调用自己进行递归下降解析

#### Check “\”：
* \x将会有一个辅助函数将其后的两个字符解析为十进制整数，并插入为char
* \+数字，将会有一个函数寻找对应位置的表达式指针，因为找的只是元表达式，所以不会有孩子节点，而且因为表达式只会被使用而不会被改变，所以引用也可以直接复制，不用深复制

#### Check”(“：
因为用了全局变量存储表达式树的头节点和当前节点，所以只用备份当前的值，并将值设置为新节点的孩子节点，之后再调用check就可以为新节点的孩子节点生成一个子树

#### Check”[“：
会和”()”进行相同的操作，差异是调用时指定的模式，而且”[]”再开头会判断是否有”^”

#### Check”{}”：
识别模式并修改当前的节点重复次数

### Step2：检查字符串
Match pattern将会遍历表达式链表，并调用match expression检查每个单独的表达式。子树的遍历将会在match expression中通过调用match pattern进行

translate
======
C project - regular expression in my way
=======

I never used regular expression before I wrote this,so I'm confused about some points of its grammar.My purpose was to practice my C skill,so I just implemented the gramar of what I'm confused about in my way.

grammar：
------

* a line of regular expression and a line of string being tested

* "^" mark the begin of regular expression

* "$" mark the end of regular expression

* "*" match the previous sub-expression zero or more times  

* "+" match the previous sub-expression one or more times  

* "?" match the previous sub-expression one or two times.When it follow another constraint symbol，such as *,+,?，{n}，{n,}，{n,m},it has a different explanation which means the previous constraint symbol is un-greedy.

* "{n}" n is a non-negative integer which means match the previous sub-expression exactly n times

* "{n,}" n is a non-negative integer which means match the previous sub-expression at least n times

* "{n,m}" n and m are both a non-negative integer.n<=m.match the previous sub-expression at least n times and at most m times

* "." match a character expect "\n"

* "x|y" match x or y.x and y can be a sub-expression or a single character

* "[xyz]" character collection.Used to match a character in the collection,except when there is a '-' between two characters which means those three characters match a character has a ASSIC value within the value scope of two characters stand for.When there is a '^' next to '[',it means mach a character not in the collection.

* "(pattern)" a sub-expression which can be nested

* \b match the end of a word which means it will fail if a identifier-character next.Only for testing,no matching of anything meaningful.

* \B match not the end of a word which means it will fail if a non-identifier-character next.Only for testing,no matching of anything meaningful.

* \d match a number-character.equals to [0-9]

* \D match a non-number-character.equals to [^0-9]

* \f Form Feed.equals to \x0c 

* \n Line Feed.equals to \x0a 

* \r Carriage Return.equals to \x0d

* \s any blank-character.equals to [ \f\n\r\t\v]。

* \S any non-blank-character.equals to [^ \f\n\r\t\v]。

* \t Horizontal Tab.equals to \x09 

* \v Vertical Tab.equals to \x0b 

* \w match any identifier-character include '_'.equals to "[A-Za-z0-9_]"

* \W match any non-identifier-character.equals to "[^A-Za-z0-9_]"

* \xn match a n.n is a hexadecimal number.n has to be exactly two characters.For example,"\x41" means "A"."\x041" means "\x04&1".This let regular expression to use ASSIC code.

* \num match a num.num is a positive integer.It is a reference of a previous pattern.For example,"(.)\1" matches two same adjacent characters.Only support when 0≤num≤9,and only reference meta-expression.For example,(ab) will be two expressions ignoring Parentheses.

Strategy:
---
* First parse the regular expression to generate a syntax tree. The branches are subexpressions of "()", "[]", "|". The exp_id of the node indicates what character the node will match. And there are fields to record the number of times this expression will be repeated.

* The subtree of "()" is the same structure as the main tree, and the parsing rules are the same, but the subtrees of "[]" and "|" have their own parsing rules.

* The root node of each subtree does not match any actual contents, but is used to indicate that it has subtrees

* Use this syntax tree to match strings

### Step1：parse the syntax tree

Each function performs only one or a few characters of parsing, followed by recursive calls to recursive descent analysis.

#### Check： 
First determine if the regular expression tail has been reached.

* "|" inserts a node before the current node and moves the current node to the child of the inserted node, because it is difficult to perform special processing on only one subsequent expression, so the parsing will continue. Instead, after the parsing is completed, it will be processed again. When the expression of "|" is encountered, the expression after it will be placed behind its child tree (there is already a node), so the child tree will have Two nodes.
* "-" will judge the current mode, if it is in a bracket, and the current node and the following are both unescaped characters, then modify the current node to match the character range.
* "+" "*" will directly modify the number of repetitions of the current node (even if it has a child tree)
*  "?" needs to determine whether the current node has been modified by the limiter. If it is, the current node is set to be non-greedy, otherwise only the number of repetitions is modified.
* When encountering "(","{","[","\" will enter the corresponding function for specific analysis
* Recursively call to perform recursive descent analysis

#### Check “\”：
* \x There will be a helper function that parses the next two characters into a decimal integer and inserts it as char
* \+ a number.There will be a function to find the expression pointer of the corresponding position, because it is only a meta expression, so there will be no child nodes, and because the expression will only be used without being changed, the reference can also be copied directly. Without deep copying

#### Check”(“：
Because the global variable is used to store the head node and the current node of the expression tree, only the current value is backed up, and the value is set to the child node of the new node. Then, by calling check, a child tree can be generated for the child node of the new node. 

#### Check”[“：
Will perform the same operation as "()", the difference is the mode specified at the time of the call, and "[]" will start at the beginning to determine whether there is "^"

#### Check”{}”：
Identify patterns and modify current node repeats

### Step2：check the string
The Match pattern will iterate through the expression list and call the match expression to check each individual expression. The traversal of the subtree will be done by calling the match pattern in the match expression.



