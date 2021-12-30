# TinyExtend
About Complier Experiment 4.

### Feature

**扩充的语法规则有：**

- 实现 do while循环，for循环，扩充算术表达式的运算符号：-= 减法赋值运算符号（类似于C语言的-=）、求余%、乘方^
- 扩充比较运算符号：==（等于），>(大于)、<=(小于等于)、>=(大于等于)、<>(不等于)等运算符号
- 新增支持正则表达式以及用于repeat循环、do while循环、if条件语句作条件判断的逻辑表达式：运算符号有 and（与）、 or（或）、 not（非） 。

1. Dowhile-stmt-->do  stmt-sequence  while(exp); 
2. for-stmt-->for identifier:=simple-exp  to  simple-exp  do  stmt-sequence enddo    步长递增1
3. for-stmt-->for identifier:=simple-exp  downto  simple-exp  do  stmt-sequence enddo    步长递减1
4.  -= 减法赋值运算符号、求余%、乘方^、>=(大于等于)、<=(小于等于)、>(大于)、<>(不等于)运算符号的文法规则请自行组织。
5. 把tiny原来的赋值运算符号(:=)改为(=),而等于的比较符号符号（=）则改为（==）
6. 为tiny语言增加一种新的表达式——正则表达式，其支持的运算符号有  或(|)  、连接(&)、闭包(#)、括号( ) 以及基本正则表达式 。
7. 为tiny语言增加一种新的语句，ID:=正则表达式  
8. 为tiny语言增加一种新的表达式——逻辑表达式，其支持的运算符号有  and(与)  、or (或)、非(not)。
9. 为了实现以上的扩充或改写功能，还需要对原tiny语言的文法规则做好相应的改造处理。 
