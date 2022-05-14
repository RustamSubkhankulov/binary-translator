# BinaryTranslator
Binary translator from binary code for virtual processor to x86 made as a task at programming course in MIPT DREC 1st grade.

## Formulation of the problem
The task was to write binary translator from architecture of virtual processor, that was made as task too previously, to x86 architecture. Binary translater is made in form of [JIT]( https://en.wikipedia.org/wiki/Just-in-time_compilation "Wikipedia" ) ( Just-in-time ) compiler. By link one can read more about jit compilation.

The amain idea of jut compiler - translated code from one another to native code of CPU, executing JIT-compiler, is stored and array and called as function. 

The main result of translating, that we want to achieve, is increasing in performance. Therefore, after talking about translating process, we will test performance and make conclusion.

## My virtual CPU architecture
To be more concise, let's call this architecture r86. 
Firstly, before making plans of translating, necessary step is to examine r86 instructions set and decide which instructions we will translate and which won't be supported by our translator.

| HLT | ADD | SUB | MUL | DIV | PUSH | POP | OUT | IN   | RET | POW  | EQ  | MR   | JA  | JB  | JE  |
|-----|-----|-----|-----|-----|------|-----|-----|------|-----|------|-----|------|-----|-----|-----|
| MRE | LS  | LSE | NEQ | SIN | COS  | TG  | LN  | ASIN | ATG | DRAW | JMP | CALL | JAE | JBE | JNE |

My virtual processor includes array of constant size - RAM, 16 froat registers, each can be used on general purposes.

Instruction set of r86 can be divied in three main parts - arithmetics part, data movement and control instructions ( jumps, call and ret )
My virtual CPU includes stack used for Push-Pop arithmetics. Also return address is stored in stack during call. 

#### Arithmetics

Push-pop arithmetic instructions take two float values from stack, perform calculation and returns result in stack.
Some of arithmetic although need only one argument - SIN, COS etc. These instructions takess only one float value from stack, performes calculation using functions from <math.h> and returns result on the top of the stack. 

Instructions MR, MRE, LS, LSE, EQ and NEQ take two float values from stack, perform comparing and returns bool result of comapring - 0 or 1 - on the top of stack.

#### Control instructions

Call and ret basically work the same as CALL and RET in x86 ( details will be discussed later ). Jumps instructions includes conditionals jumps, that are takes two float values from stack, perform comparing and change current instruction position depending on result of compare. Unlike x86, r86 architecture does not have flags. Also in binary code there are always absolute value of destination of jump, unlike relative version of jumps in x86. 

HLT is necessary instruction at the end of programm. Without is, virtual CPU won't stop executing.

#### Data movement

Two instructions - push and pop - used for data movement. Depending on operation code in binary, different data movement is performed. Unlike x86, in r86 there is no way to move value from one place to another not using stack.
PUSH pushes value to stack from:  1) one of 16 float registers 2) from ram by index containing in register 3) from ram by index containing in binary code 4) from memory by index = immediate value from binary + value in one of 16 registers. Also PUSH can store immediate value from binary code on the top os stack. 
POP performs reverse action, except there is no POP to immediate value.

#### Unsupported instructions

DRAW instruction is used for printing values from special part of RAM - video memory - in terminal. This instruction is too complicated for translating, so this instruction won't be supported in our translator. Actually, this instrucion will be translated to just nothing. So translating will not stop executing, just nothing will be performed. 

## Translating instructions

For translating instructions with float registers in r86, XMM registers are used. If in binary code there are instructions using RAM, float array the same size as RAM in r86 is allocated and used during executing. 

Because of the fact, that XMM registers are used for containing values of registers in r86, regular integer registers of x86 are used for saving value of xmms registers. This will be discussed in details later.

as a stack for arithmetic is used native x86 stack.

Before translated version of instructions, integer registers are saved in stack according to calling convention. Also all XMM registers are set to zeros, because in r86 at the start of programm all of flaot registers are zeros. 

After translated instructions, if there is no HLT in binary code, saved integer registers are poped from stack and RET ( x86 ) exexcuted.

#### Arithmetics

Arithmetics in translated code is also Push-Pop. Before calculating, value of one of XMM registers is saved in integer register, one of the values from stack is moved to this XMM registers. Ther arithmetic instructions is performed, and result is XMM registers. Then RSP is increased by 8 ( poping one of the values from stack ).Then result of calculation is moved from XMM registers back to the stack and value of XMM register is restored.

Note: xmm15 and r15d registers are always used in arithmetics.

Example: ADD

<code>  movd r15d, xmm15                  </code>

<code>  movss xmm15, dword [ rsp + 8 ]    </code>

<code>  adds xmm15,  dword [ rsp ]        </code>

<code>  add rsp, 8                        </code>
  
<code>  movss dword [ rsp ], xmm15        </code>

<code>  movd xmm15, r15d                  </code>

#### Data movement - PUSH and POP

## Optimization feature

My JIT-compiler includes additional feature, that optimize binary code of r86 architecture before translating. Operations from input are read to list amd then redudant instructions are removed from list. Then remaining instructions are flushed back to buffer, that is used then for translating. 

Optimizations include constant folding in various situations. Examples below:

1) <code> PUSH 0; COS </code>              --> <code> PUSH 1 </code> 
2) <code> PUSH 2; PUSH 3; ADD </code>      --> <code> PUSH 5 </code>
3) <code> PUSH [ax+5]; PUSH 0; MUL </code> --> <code> PUSH 0 </code>
4) <code> PUSH [ax+5]; PUSH 1; MUL </code> --> <code> PUSH [ax+5] </code>
5) <code> PUSH [ax+5]; PUSH 0; ADD </code> --> <code> PUSH [ax+5] </code> 

## Comparing performace

Due to the fact, that jit compiler includes executing translating every time it executed, we will test performance using very difficult and big calculations. 
Performance comparing will be between JIT-compiler and compiler for my own language, that was nade as task in previous semester. This compiler output is binary code for my virtual CPU of r86 architecture. The we will also compare results with performce of just only virtual CPU, cause performance of just only CPU is higher then Compiler + CPU. 

As test we will calculate factorial of 34 in cycle 100,000 times. For measuring time linux terminal built-in feature 'time' and save as result 'real' time given by 'time'. Every measurement will be hold several times to decrease random error and as time to compare will be used average of all of the measurements.

Here is programm on my own 'harry potter' language, that was used for tests. For testing CPU and JIT generated by my own assembler using this programm r86 binary code was used. 


#### Compiler

| #       | time, sec |
|---------|-----------|
| 1       | 26,082    |
| 2       | 25,231    |
| 3       | 25,321    |
| 4       | 25,355    |
| 5       | 26,784    |
| AVERAGE | 25,7546   |

#### Only CPU

| #       | time, sec |
|---------|-----------|
| 1       | 25,166    |
| 2       | 25,708    |
| 3       | 25,277    |
| 4       | 25,406    |
| 5       | 25,384    |
| AVERAGE | 25,3882   |

#### JIT

| #       | time, sec |
|---------|-----------|
| 1       | 0,1       |
| 2       | 0,133     |
| 3       | 0,096     |
| 4       | 0,105     |
| 5       | 0,098     |
| AVERAGE | 0,1064    |

#### Comparing results

