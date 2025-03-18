任务2.1：算术表达式的处理

【任务名称】算术表达式的处理

【任务性质】主线任务的前趋任务，应在规定时间内完成。

【输入】一个名字类似 lex_out.txt 的纯文本文件，内容是单词符号所构成的串。单词由两部分构成：单词的种类和具体的值或能够联系到值的指针。

【主要功能】对输入串从左到右做一次处理，生成其抽象语法树。

【输出】以非终结符 EXPR 为树根的一棵抽象语法树。

【任务说明】复用第1章中做过的工作，可以轻松完成本次任务。

【理论层面的一些思考】

    （1）第1章中处理算术表达式的方法上升到理论层面实际上就是算符优先分析法。这是一种简单高效的自下而上分析法，特别适合于处理表达式结构。所以在这里要注意自下而上分析（算符优先分析）和自上而下分析（比如说递归下降分析）的区别：他们分别适用于什么样的CFG，或者说对基础文法的要求有什么不同。

    （2）要正确认识中缀形式表达式自带的”二义“特性。首先要清楚：二义性文法给语法分析带来的麻烦是什么，也就是如果放任不管会导致什么样的麻烦；其次还应掌握：有没有什么办法可以在语法分析过程中解决二义性带来的这些麻烦；这两个问题都想清楚了之后，就完全可以做到：既不修改二义性文法，又能保证语法分析的正确性。采用这种“解题思路”主要出于两方面的考虑：一是二义文法往往简洁，用它做出的语法树相应也简单；二是消除二义性需要修改文法的结构，也就会打乱语法单位与其所表达的语义之间的对应关系，会给后续语义处理带来额外负担。

    （3）基于上述两点考虑，一个较优的解决方案就是基于文法：E->E+E|E*E|(E)|id|num 来进行分析设计。具体分析时，可以将该文法与 E->E+T|T 的那个文法做一个比较，然后就能发现后者中出现的单非产生式（如：E->T ,  T-> F）是个比较讨厌的东西；这一点在下个任务中处理布尔表达式时表现的更明显，因为布尔表达式的构造过程用到了很多单非产生式。

【设计层面的一些思考】

    （1）算术表达式的抽象语法树结构简单而明确：对每一个子表达式来说，其对应的子树总是以该子表达式最外层的运算符为树根，以该运算符所结合的运算对象为左（右）子树。表达式和树都是递归定义的。对于这样的树，做一次后根遍历就可以得到后缀表达式，做一次中根遍历就可以得到中缀表达式，做一次先根遍历就可以得到前缀表达式。

    （2）树设计好了之后，后续的语义处理都可以基于树的遍历过程来进行。

    （3）树结构的存储和输出：二叉化。可以考虑：左孩子、右兄弟的形式。

    （4）为了降低问题复杂度，可以先不考虑错误恢复，发现错误后立即报错、终止程序。



任务2.2：布尔表达式的处理

【任务名称】布尔表达式的处理

【任务性质】主线任务的前趋任务，应在规定时间内完成。

【输入】一个名字类似 lex_out.txt 的纯文本文件，内容是单词符号所构成的串。单词由两部分构成：单词的种类和具体的值或能够联系到值的指针。

【主要功能】对输入串从左到右做一次处理，生成其抽象语法树。

【输出】以非终结符 BOOL 为树根的一棵抽象语法树。

【任务说明】复用上一个任务的算法即可，只是问题规模变大了：要处理更长更复杂的式子，要考虑更多的算符之间的关系，本质上还是同一个问题。

#任务2.3：声明语句的处理

【任务名称】声明语句的处理

【任务性质】主线任务的前趋任务，应在规定时间内完成。

【输入】一个名字类似 lex_out.txt 的纯文本文件，内容是单词符号所构成的串。单词由两部分构成：单词的种类和具体的值或能够联系到值的指针。

【主要功能】对输入串从左到右做一次递归下降分析，生成其抽象语法树。

【输出】以非终结符  DECLS  为树根的一棵抽象语法树。

【理论层面的一些思考】

    （1）表达式结构用算符优先分析处理起来很方便，而语句结构用递归下降处理起来更便捷一些。虽然都是做语法分析，都是生产语法树，但表达式结构和语句结构的差异性很大，所以使用不同的处理方法来区别对待就是一种很好的思路。本节的标题即来源于此。

    （2）使用不同的子程序去处理不同的语法单位（非终结符），用子程序之间的“调用/被调”关系来表达语法单位之间的“包含/被包含”关系（整体/局部关系），用子程序之间的数据传递（参数传递/返回值）来表达语法单位之间的语义传递，这就是递归下降的精髓。

    （3）既然要做递归下降分析，那么首先要明确一点：递归下降对文法的要求。这一要求用理论化的形式表达出来就是一个概念：LL(1)文法。所以对于源语言中的声明语句，也即以  DECLS  为首的那一系列非终结符及其产生式，要依次考虑下列问题：有无二义性？有无左递归？每一条右部有多个候选的产生式，其多个候选的FIRST集是否两两不相交？有没有右部导出空串的产生式，是否要进一步查看FOLLOW集？对于这些问题要有确定的判断和对应的解决方案。

【设计层面的一些思考】

    （1）在声明语句中出现的“list”结构（如：NAMES）往往可以抽象为a*形式或其变种(ab)*，这种结构用CFG来表达时，可以将其左递归表示直接对调符号位置，变成等价的右递归表示。灵活运用这一特性能大大简化对文法的设计以及后续的分析过程的设计。参考书附录中的递归下降分析器处理类似结构时，展示了两种不同的处理方法，很值得揣摩、借鉴。

    （2）声明语句的结构比较简单，其语法树的设计也相对简单一些。抽象语法树和规范的语法分析树的结构、形状相差不多，设计树的结构时主要考虑如何让生成树的过程简单、方便。





任务2.4：执行语句的处理

【任务名称】执行语句的处理

【任务性质】主线任务的前趋任务，应在规定时间内完成。

【输入】一个名字类似 lex_out.txt 的纯文本文件，内容是单词符号所构成的串。单词由两部分构成：单词的种类和具体的值或能够联系到值的指针。

【主要功能】对输入串从左到右做一次处理，生成其抽象语法树。

【输出】以非终结符 STMTS  为树根的一棵抽象语法树。


【理论层面的一些思考】

    （1）跟上一题一样，此处采用递归下降分析方法。同样需要考虑从源语言中抽象出来的CFG是否满足LL(1)的要求。

    （2）源语言中存在五类不同的执行语句：赋值语句、选择语句、循环语句、语句块和读写语句，分别完成：表达顺序、选择、循环三种控制流，表达块结构和处理I/O的功能。可以进一步地将其细分为6种、7种或8种不同的语句结构来分别处理。具体怎么分完全取决于算法设计的方便。

    （3）参考书附录中的递归下降分析器处理单分支、双分支选择语句的做法很值得借鉴。

    （4）源语言中有两种赋值语句：整型量赋值和布尔型量赋值，其结构一样，但却使用了两条不同的产生式。为了彰显两者的区别，甚至违反惯例使用了不同的赋值运算符。此处值得认真思考：这么设计的目的是什么？能得到什么好处？或者从反方向思考：如果不加区分，都使用 = 作为赋值运算符，会有什么麻烦？

    （5）源语言中省略了对块结构的命名：BLOCK，也没有采用 STMT → BLOCK ，而是采用了 STMT →  {  STMTS   STMT  } 的形式。可以从设计源语言的角度思考一下这两者的区别。另外，这里虽然不存在左递归的问题，但可以思考一个类似的问题：如果用 STMT →  {  STMT   STMTS  } ，对语法分析是否有影响？

    （6）还有一个简单的思考题：while 语句中的 do 起什么作用？去掉这个关键字，影响有多大？

【设计层面的一些思考】

    （1） STMTS 的结构与 DECLS 基本一样，所以STMTS 的处理可以复制上一题中DECLS 的处理。

    （2）相对声明语句，执行语句的语法树要复杂一些。每一种不同的语句，其抽象语法树结构也不相同，需要逐一分析、设计。



#

【任务名称】语法分析器的设计与实现

【任务性质】主线任务，应在规定时间内完成。

【输入】一个名字类似 source.txt 的纯文本文件，内容是字符串形式的源程序。

【输出】一个名字类似 parse_out.txt 的纯文本文件，内容是一棵完整的语法分析树或抽象语法树，形式可以是树结构的图形化表示或者某种（某两种）遍历序。

【题目说明】设计一个程序，针对选定的源语言，从输入文件中读入一段源程序，对其做词法分析和语法分析。若源程序不含语法错误，则输出源程序对应的语法树或抽象语法树；若源程序中存在错误，则报错、退出分析。程序应将所有结果信息写入输出文件。

【设计约束】

（1）从技术路线上说，建议在总体层面遵循递归下降分析方法。在处理表达式结构时，可以复用本章前趋任务2.1和2.2的工作成果，用算符优先分析方法来处理算术表达式和布尔表达式。处理语句结构时，可以复用本章前趋任务2.3和2.4的工作成果。实现时可以借助子程序之间的过程调用机制完成各个模块之间的功能协作，用全局数据结构（公共数据区）或子程序之间的数据传递（参数/返回值）来完成各个模块之间的数据传递。当然，完全使用递归下降方法或者LL（1）分析、SLR（1）分析来做也是很好的。

（2）从处理过程上来说，整个程序对输入的源程序应该从左到右处理一遍且仅此一遍。

（3）从结构上说，该程序应该至少包含3个模块：驱动模块、词法分析模块和语法分析模块。驱动模块包含了程序的入口和出口，主要负责输入、输出处理并调用另外2个工作模块；词法分析模块可以复用主线任务一中的工作成果；语法分析模块的主要任务是：把词法模块送来的单词符号串拼接成一个个语法单位，再用树结构表达出来，同时检查是否存在语法错误。

（4）在分析时，可以把本次任务划分为若干个子任务，其中有些子任务可以进一步细分为若干个子子任务......在设计时，相应地可以把整个程序分解成若干个子模块及其子子模块。整个过程可以参照结构化程序设计的思路来做。如果用C语言来实现，程序可能就由若干个.c和若干个.h组成；若用OOP，比如说Java，程序可能就由若干个.class组成。

【其他约束】

（1）deadline：第六次实验课结束后。

（2）文档要求：完成任务的标志是在规定时间内提交规范的实验报告。

a. 报告的正文应该是一篇word文档或者word转pdf文档；

b. 报告应包含对源语言的描述，例如：形式化描述和自然语言描述等；要让阅读者清楚该报告的工作对象。如果语法分析时对词法分析时选定的源语言有修改，请在报告中详细说明。

c. 建议报告中包含对程序的主要流程、关键步骤的算法和总体结构的简要描述，这些信息用图形化工具加少量文字说明的方式来表达效果更好；

d. 报告应包含程序正常运行的画面截图和使用测试用例测试程序的画面截图及文字说明；截图时请注意图片中文字大小，以视觉上清晰、干净为好。

e. 程序写好后应进行充分的测试。报告中应包含不少于5个测试用例（参考资料中sourceProgram1~8.txt）的测试结果。测试尽量覆盖以下几种情况：声明语句为空，负数与减法同时出现，复杂算术表达式、嵌套的IF语句（存在二义性风险）。

f.  应将程序源代码、测试用例及可能需要的资源文件打包作为报告附件。

g. 若某个源程序的语法树过大，不利于图形化显示，可以仅展示部分有代表性的子树。

#

