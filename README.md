# JIT-компилятор
JIT-компилятор, транслирующий бинарный код, предназначенный для исполнения виртуальным процессором, который был написан в первом семестре в качестве одной из учебных задач, в инструкции архитектуры x86.

Главная особенность JIT (Just-in-time) компилятора состоит в том, что результат трансляции - бинарный код, содержащий инструкции архитектуры, в которую происходит трансляция, исполняется сразу же после процесса трансляции. Для этого бинарный код сохраняется в массиве и вызывается в качестве функции. 

Главным результатом, которого мы хотим добиться при написании JIT-компилятора, это повышение быстродействия исполнения. Сравнение времени исполнения тестовой задачи будет заключающим пунктом работы.

## Архитектура виртуального процессора
Для того, чтобы быть более краткими в обсуждениях трансляции, будем называть архитектуру виртуального процессора, из которой происходит трансляция, r86.

Перед тем, как описывать процесс транляции инструкций r86 в инструкции архитектуры x86, опишем инструкции архитектуры r86.

Инструкции архитектуры r86 могут быть поделены на несколько групп: 
1) Инструкции, используемые для арифметических операций: эта группа включает в себя как инструкции, производящие простейшие арифметические операции - ADD, SUB, MUL, DIV, так и инструкции, вычисляющие значения некоторых математчиских функций - SIN, COS, TG, LN, ASIN, ATG, POW - и инструкции, результатом которых является bool значение выражения сравнения - MR, MRE, LS, LSE, EQ, NEQ.
2) Инструкции управления потоком исполнения - безусловный 'jump' - JMP, условные 'jump'ы - JA, JAE, JB, JBE, JE, JNE, а также вызов функции CALL, возврат из функции RET и прекращение работы процессора HLT. 
3) Инструкции перемещения данных - PUSH и POP

Необходимыми для исполнения инструкций виртуальным процессоров являются:
- 16 регистров общего назначения, содержащие в себе значения с плавающей точкой (float)
- Оперативная память определенного размера, представляющая из себя массив типа float
-  Структура данных стек, использующийся для арифметики, построенной на PUSH и POP инструкциях, а также для хранения адреса возврата при вызове функций.

#### Арифметика 
Особенность арифметических операций в архитектуре r86 - все инструкции, использующиеся для вычисления арифметических выражений, принимает аргументы со стека. Это означает, что инструкции, требующие два аргумента, используют для вычислений два значения, которые сейчас находятся на вершине стека. После выполнения инструкции результат также находится на вершине стека.

ADD, SUB, MUL и DIV  исполняются простейшие арифметические действия - сложение, вычитание, умножение и деление. При этом если до выполнения инстукции в стеке лежало два числа - два аргумента, то после на вершине стека будет находиться только одно значение - результат вычисления.

Команды SIN, COS, TG, LN, ASIN, ATG вычисляют соответсвенно синус, косинус, тангенс, натуральный логарифм, арктангенс и арксинус от аргумента с вершины стека и там же возвращают результат. Фактически, аргумент, находящийся на вершине стека, заменяется результатом применения к нему соответствующей функции. Для вычисления результата используются функции из библеотеки <math.h>. 

Инструкции MR, MRE, LS, LSE, EQ, NEQ сравнивают два значения, находящиеся на вершине стека и в зависимости от результата, возвращают единицу или ноль на вершине стека. Если сравнение верно, то результатом является 1, иначе результат равен 0.

#### Инструкции управления потоком 

Инструкция HLT является обазятельным условием завершения работы виртуального процессора. Как только виртуальный процессор встречается инструкцию HLT, вне зависимости от состояния стека ( его сбалансированности ) исполнение дальнейших инструкций прекращается.  

Инструкция вызова функции CALL сохраняет адрес следующей после нее инструкции в стеке, а после этого исполнение инструкций продолжается с адреса, который является аргументом инструкции CALL. Инструкция RET принимает значение с вершины стека, после чего исполнение инструкций происходит с адреса, равного этому значению. С помощью двух этих инструкций происходят вызов функции и возврат из нее в r86 архитектуре.

Безусловный 'jump' вне зависимости от состояния виртуального процессора изменяет текущую позицию в коде, инструкции продолжают исполнятся с другого адреса. В отличие от безусловного, условные 'jump'ы происходят только если результатом соответствующего сравнения двух аргументов - двух значений с вершины стека - является true. Иначе прыжок не происходит и исполнение инструкций продолжается со следующей после условного 'jump'а. При сравнении аргументы пропадают из стека. 

Аргументами условного и безусловных 'jump'ов являтся абсолютные адреса - позиции в бинарном коде относительно начала.

#### Перемещение данных

Для перемещения данных используются две инструкции - PUSH и POP.
Важно отметить, что в архитектуре r86 отсутствует возможность перемещения данных из регистра или оперативной памяти в регистр или оперативную память без использования стека. 

Инструкция PUSH добавляет значения в стек, а POP - удаляет его из стека.

В архитектуре r86 сущесвуют различные разновидности данных инструкций.

PUSH в стек:
- из регистра
- из оперативной памяти по индексу, равному значению в одном из регистров
- из оперативной памяти по индексу, равному аргументу инструкции из бинарного кода
- из оперативной памяти по индексу, равному сумме значения в одном из регистров и значения из бинарного кода 
- константного значения из бинарного кода

 POP из стека в:
- в регистр
- в оперативную памяти по индексу, равному значению в одном из регистров
- в оперативную памяти по индексу, равному аргументу инструкции из бинарного кода
- в оперативную памяти по индексу, равному сумме значения в одном из регистров и значения из бинарного кода 

#### Неподдерживаемые инструкции

Инструкция DRAW выводит в терминале содержание определленной области оперативной памяти по строчкам длины, заданной конфигурацией виртуального процессора. Данная область оперативной памяти является зарезервированной в качестве видеопамяти. Трансляция этой инструкции слишком затруднительна, поэтому на данный момент она не поддерживается JIT-компилятором. При трансляции даннная инструкция игнорируется и пропускается.

## Трансляция инструкций

For translating instructions with float registers in r86, XMM registers are used. If in binary code there are instructions using RAM, float array the same size as RAM in r86 is allocated and used during executing. 

Because of the fact, that XMM registers are used for containing values of registers in r86, regular integer registers of x86 are used for saving value of xmms registers. This will be discussed in details later.

as a stack for arithmetic is used native x86 stack.

Before translated version of instructions, integer registers are saved in stack according to calling convention. Also all XMM registers are set to zeros, because in r86 at the start of programm all of flaot registers are zeros. 

After translated instructions, if there is no HLT in binary code, saved integer registers are poped from stack and RET ( x86 ) exexcuted.

### HLT
Instructions HLT of r86 instruction set is translated into restoring ( poping ) from stack saved integer registers according to calling convention and ret ( x86 instruction )

### Arithmetics

Arithmetics in translated code is also Push-Pop. Before calculating, value of one of XMM registers is saved in integer register, one of the values from stack is moved to this XMM registers. Ther arithmetic instructions is performed, and result is XMM registers. Then RSP is increased by 8 ( poping one of the values from stack ).Then result of calculation is moved from XMM registers back to the stack and value of XMM register is restored.

Note: xmm15 and r15d registers are always used in arithmetics.

Example: ADD

<code>  movd r15d, xmm15                  </code>

<code>  movss xmm15, dword [ rsp + 8 ]    </code>

<code>  adds xmm15,  dword [ rsp ]        </code>

<code>  add rsp, 8                        </code>
  
<code>  movss dword [ rsp ], xmm15        </code>

<code>  movd xmm15, r15d                  </code>

### Data movement - PUSH and POP

##### PUSH

1) PUSH from RAM by index in register:
  Firstly char value is took from binary code - number of register - one of 16. Value in XMM registers is float, so we need to convert it in integer value. Size of float == 4.
  
  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> push qword[ Start_addr + r13d * 4] </code>

2) Push from register:

  <code> sub rsp, 8 </code>
  <code> movss dword [rsp], xmm(i) </code>
  
3) Push from RAM by index equal immediate value from binary code:
  Firstly get flaot value from binary code == index in RAM
  
  <code> mov r13d, index </code>
  
  <code> push qword [ Start_addr + r13d * 4] </code>
  
4) Push immediate value:
  Firstly get float immediate value from binary code, then add this value to specially allocated array of constant values and patch instruction with addres of this patrocular flaot value in array
  
  <code> push qword [Address_of_imm] </code>
  
5) Push from RAM by summ of value in register and immediate value in binary code
  Firstly get char value == number of register, then float value == index in array. This float value is converted into integer value, multiplied by 4 == sizeof(float) and  stored in r14d register. Convert value in xmm(i) registers to integer value in r13d. 

  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> mov r14d, Imm_value_from_binary_code </code>
  
  <code> push qword [ Start_addr + r13d * 4 + r14d ] </code>

##### POP

1) Pop to RAM by index in register:
  Convert float value from xmm register to integer value in r13d, pop from stack to r14, then move value from r14d to RAM

  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> pop r14 </code>
  
  <code> mov dword [ Start_addr + 4 * r13d ], r14d </code>

2) Pop to register:

  <code> movss xmm(i), dword [ rsp ] </code>
  
  <code> add rsp, 8 </code>

3) Pop to RAM by immediate index in bynary code:
  Float value from binary code converted into integer value and stored in r13d
  
  <code> mov r13d, Index_from_binary_code </code>
  
  <code> pop r14 </code>
  
  <code> mov dword [ Start_addr + r13d * 4 ], r14d </code>

4) Pop to RAM by index == summ of immediate value from binary code and value im one of 16 registers:
  Char value from binary == number of register, float value == index 
  
  <code> cvtss2si r13d, xmm(i) </code>
  
  <code> mov r15d, (unsigned int) (float value from binary) * 4 </code>

  <code> pop r14 </code>
  
  <code> mov dword [ Start_addr + r13d * 4 + r15d ], r14d </code>

### Comparing instructions: EQ, NEQ, MR, MRE, LS, LSE

1) Save xmm0, xmm13 values in integer registers:
  
  <code> movd r15d, xmm0  </code>
  <code> movd r13d, xmm13 </code>

2) Null xmm13

  <code> pxor xmm13, xmm13 </code>
 
3) Compare two values on the top of the stack
 
  <code> movss xmm0, dword [ rsp ] </code>
  
  <code> comiss xmm0, dword [ rsp + 8 ] </code>
 
4) Example of translating MR:

  <code> jbe $ + sizeof (next instruction ) </code>

5) Store 1 in xmm13 as result if comapre is true

  <code> movss xmm13, dword [ Address of 1 constant value ] </code>

6) Clear stack from compared values by adding to rsp 16
  
7) Push xmm13 to stack

  <code> sub rsp, 8 </code>
  
  <code> movss dword [ rsp ], xmm13 </code>

8) Restore xmm13 and xmm0 values from integer registers where they were saved.

### Jumps, conditional jumps, call and ret

1) Using system of calculating relative offset from jump to jump destination during translating, all jumps and conditional jumps are translated into near relative versions. Firstly these instructions are initialized in dynamic array of instructions with NULL offset, then during patching offsets are fixed. 
2) Jump, call and ret are translated directly to near relative call and jump and native x86 ret instructions.
3) Conditiolnal jumps in x86 are only short, so conditional jump translated as in example of translating JA (r86):
  
  <code> add rsp, 16 </code>
  
  <code> movd r15d, xmm0 </code>

  <code> movss xmm0, dword [ rsp - 16 ] </code>
  
  <code> comiss xmm0, dword [ rsp - 8 ] </code>

  <code> movd xmm0, r15d </code>

  <code> jbe $ + sizeof (near relative jump) </code>

### Arithmetic instructions - functions and IN/OUT

Input-output instructions and arithmetic functions - POW, SIN, COS, LN and other - are translated to call of needed function in my own standard library.
For each used needed function for <math.h> to calculate value. Firstly only empty call is initialized, but at patching stage of translating addresses are fixed up.
Every call is accompanied by forced pushing and poping using stack all XMM registers, due to the fact, that all XMMs are not preserved during call according to calling convention. 
Also call of calculating functions is accompanied by alignment stack to 16 boundary by calculating remainder of the division RSP by 16 and adding result to RSP ( 8 or 0 if stack is already aligned ). After return alignment, which is saved in one of the preserved during call integer registers, is subtracted from RSP.

##### Example of translating SIN:
  - saving xmm0 value
 
  <code> movd r15d, xmm0 </code>
  
  - pop from stack to xmm0
   
  <code> movss xmm0, dword [ rsp ] </code>
  
  <code> add rsp, 8 </code>
  
  - push XMM registers from 1 to 15
  
  - align stack to 16-boundary
   
  <code> mov r14, rsp </code>
  
  <code> and r14, 0xF </code>
  
  <code> and rsp, r14 </code>
  
  - relative near call, address will be patched later
  call Arithmetic_function
  
  - pop XMM 1 - 15 from stack, restore values
  
  - push XMM0 value to stack 

  <code> sub rsp, 8 </code>
  <code> movss dword [ rsp ], xmm0 </code>
  
  - restore xmm0 value from r15d
  <code> movd xmm0, r15d </code>
  
## Оптимизатор 

JIT-компилятор также предлагает возможность использовать оптимизации бинарного кода, в частности свёртку констант. 
Для проведения оптимизаций инициализируется список и при оптимизациях лишние элементы списка удаляются или заменяются на другие. После завершения оптимизаций на основе списка генерируется новый массив кодов инструкций архитектуры виртуального процессора, который используется в качестве входных данных транслятора.

Свертка констант вчключает в себя различные случаи. Примеры оптимизаций приведены ниже.

1) <code> PUSH 0; COS </code>              --> <code> PUSH 1 </code> 
2) <code> PUSH 2; PUSH 3; ADD </code>      --> <code> PUSH 5 </code>
3) <code> PUSH [ax+5]; PUSH 0; MUL </code> --> <code> PUSH 0 </code>
4) <code> PUSH [ax+5]; PUSH 1; MUL </code> --> <code> PUSH [ax+5] </code>
5) <code> PUSH [ax+5]; PUSH 0; ADD </code> --> <code> PUSH [ax+5] </code> 


### Тестирование оптимизаций

Для того, чтобы убедиться в уменьшении времени исполнения программы с применением оптимизаций, искуственнно создадим ситуацию, в которой оптимизация значительно сократит количество вычислений. Для этого запустим программу, вычисляющую 2^10 10,000,000 раз в цикле при помощи JIT-компилятора сначала без флага оптимизации, а затем с применением оптимизатора и сравним измеренные показатели времени исполнения в обоих случаях.

|           | no opt | -opt flag |
|-----------|--------|-----------|
| time, sec | 0,100  | 0,285     |

Можем увидеть, что в определенных ситуациях, свертка констант позволяет добиться более чем двухкратного уменьшения времени исполнения.

## Сравнение времени исполнения

Для сравнения времени исполнения виртуальным процессором и JIT-компилятором используем 
программу включающую в себя большой объем вычислений, чтобы время трансляции кода при исполнении JIT-компилятора было мало по сравнению со временем, приходящимся на непосредственно вычисления в самой программе.
Для проведения теста используем компилятор, написанный в качестве учебной задачи в первом семестре. Результатом работы компилятора является бинарный код, содержащий инстуркции архитектуры виртуального процессора. 
[Исходный текст тестовой программы на языке 'Harry Potter'](https://github.com/RustamSubkhankulov/BinaryTranslator/blob/main/lang/txt_files/testfactorial.txt "testfactorial.txt"). Она включает в себя вычисления факториала 34 в цикле 100,000 раз. 
Для измерения времени исполнения будем использовать built-in опцию командной строки 'time', в качестве времени будем принимать результат time 'real'. 
Каждое измерение проведем 5 раз и усредним полученные значения.


#### Сравнение времени

|             | CPU     | JIT    |
|-------------|---------|--------|
| <time>, sec | 25,3882 | 0,1064 |

Как можно увидеть по результатам измерений, время исполнения программы с использованием JIT-компилятора меньше времени исполнения программы виртуальным процессоров почти в 240 раз. 
 
