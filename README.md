# BinaryTranslator
Binary translator from binary code for virtual processor to x86 made as a task at programming course in MIPT DREC 1st grade.

## Formulation of the problem
The task was to write binary translator from architecture of virtual processor, that was made as task too previously, to x86 architecture. Binary translater is made in form of [JIT]( https://en.wikipedia.org/wiki/Just-in-time_compilation "Wikipedia" ) ( Just-in-time ) compiler. By link one can read more about jit compilation.

The main idea of jit compiler - translated code from one another to native code of CPU, executing JIT-compiler, is stored and array and called as function. 

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
  
## Optimization feature

My JIT-compiler includes additional feature, that optimize binary code of r86 architecture before translating. Operations from input are read to list amd then redudant instructions are removed from list. Then remaining instructions are flushed back to buffer, that is used then for translating. 

Optimizations include constant folding in various situations. Examples below:

1) <code> PUSH 0; COS </code>              --> <code> PUSH 1 </code> 
2) <code> PUSH 2; PUSH 3; ADD </code>      --> <code> PUSH 5 </code>
3) <code> PUSH [ax+5]; PUSH 0; MUL </code> --> <code> PUSH 0 </code>
4) <code> PUSH [ax+5]; PUSH 1; MUL </code> --> <code> PUSH [ax+5] </code>
5) <code> PUSH [ax+5]; PUSH 0; ADD </code> --> <code> PUSH [ax+5] </code> 


### Testing optimization increasing in performance. 
To test, how these simple optimization can increase performce, let's run programm, that calculates 2^10 in cycle 10,000,000 times and measure time of JIT-performance.

|           | no opt | -opt flag |
|-----------|--------|-----------|
| time, sec | 0,100  | 0,285     |

We can see increasing in performance by more then 2 times in this simple calculation programm. 

## Comparing performace

Due to the fact, that jit compiler includes executing translating every time it executed, we will test performance using very difficult and big calculations. 
Performance comparing will be between JIT-compiler and compiler for my own language, that was nade as task in previous semester. This compiler output is binary code for my virtual CPU of r86 architecture. The we will also compare results with performce of just only virtual CPU, cause performance of just only CPU is higher then Compiler + CPU. 

As test we will calculate factorial of 34 in cycle 100,000 times. For measuring time linux terminal built-in feature 'time' and save as result 'real' time given by 'time'. Every measurement will be hold several times to decrease random error and as time to compare will be used average of all of the measurements.

Here is [programm](https://github.com/RustamSubkhankulov/BinaryTranslator/blob/main/lang/txt_files/testfactorial.txt "testfactorial.txt") on my own 'harry potter' language, that was used for tests. For testing CPU and JIT generated by my own assembler using this programm r86 binary code was used. 

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

#### Comparing results and conclusion

|             | Compiler | CPU     | JIT    |
|-------------|----------|---------|--------|
| <time>, sec | 25,7546  | 25,3882 | 0,1064 |
  
As we can see in measured results, difference between compiler and CPU is negligible, calculations takes much more time then syntax and lexic analysis in compiler. Although, difference in performance between JIT and Compiler with CPU is huge. Performing calculation in native commands is increasing performace of programm very effectively. 
  
Total increasing in performace is about 240 times.
