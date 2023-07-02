# Box VM
Box - simple stack-based VM [(like JVM)](https://en.wikipedia.org/wiki/Java_virtual_machine).\
It has simple assembler like programming language **[WIP]**.\
Box developed only for educational and recreational purposes.

## Quick Start
**[NOTE]: Now box using [clang](https://clang.llvm.org/) + bat scripts as a build system**
``` console
# .\build.bat
# .\box.exe
# -------------
# .\clean.bat
```

## Docs

### **Data types**
```
[VOID] - internal type to repesent no value -> [X]
[U64 ] - unsigned 64-bit integer            -> 10u
[I64 ] - signed 64-bit integer              -> 10
[CHAR] - usigned 8-bit charecter            -> 'A'
```
---
### **PUT**
``` 
put [OPERAND] - append [OPERAND] value on top of a stack.
Supported [OPERAND] types are: [U64], [I64], [CHAR].
Could produce errors: 
    [INVALID OPERAND] - if type of [OPERAND] is not correct.
```

#### **Examples:** 
```
put 10
put 20
add
print 1u
put '\n'
print 1u
halt
---OUT---
30
---------
```
```
put 10
put X
add
print 1u
put '\n'
print 1u
halt
---OUT---
[ERROR] => [INVALID OPERAND]
---------
```
---
### **ADD**
``` 
add - sum up first two values on top of a stack.
Could produce errors: 
    [STACK UNDERFOW] - if stack conteins less than two values.
    [TYPE MISSMATCH] - if values has different types.
```

#### **Examples:** 
```
put 10
put 20
add
print 1u
put '\n'
print 1u
halt
---OUT---
30
---------
```
```
put 10
add
print 1u
put '\n'
print 1u
halt
---OUT---
[ERROR] => [STACK UNDERFLOW]
---------
```
```
put 10
put 'A'
add
print 1u
put '\n'
print 1u
halt
---OUT---
[ERROR] => [TYPE MISSMATCH]
---------
```
---
### **PRINT**
``` 
print [OPERAND] - consumes [OPERAND] count values on top of a stack and prints it.
Supported [OPERAND] types are: [U64].
Could produce errors: 
    [STACK UNDERFOW]  - if stack conteins less than [OPERAND] values.
    [INVALID OPERAND] - if type of [OPERAND] is not correct.
```
#### **Examples:** 
```
put 10
put 20
add
print 1u
put '\n'
print 1u
halt
---OUT---
30
---------
```
```
put 10
put 20
add
print 10u
put '\n'
print 1u
halt
---OUT---
[ERROR] => [STACK UNDERFLOW]
---------
```
```
put 10
put 20
add
print 'A'
put '\n'
print 1u
halt
---OUT---
[ERROR] => [INVALID OPERAND]
---------
```
---
### **FLIP**
``` 
flip [OPERAND] - flips [OPERAND] count values.
Supported [OPERAND] types are: [U64].
Could produce errors: 
    [STACK UNDERFOW]  - if stack conteins less than [OPERAND] values.
    [INVALID OPERAND] - if type of [OPERAND] is not correct.
```
#### **Examples:** 
```
put 'H'
put 'i'
put '\n'
flip
print 3u
halt
---OUT---
Hi
---------
```
```
put 'H'
put 'i'
put '\n'
flip
print 12u
halt
---OUT---
[ERROR] => [STACK UNDERFLOW]
---------
```
```
put 'H'
put 'i'
put '\n'
flip
print 'A'
halt
---OUT---
[ERROR] => [INVALID OPERAND]
---------
```
---
### **HALT**
``` 
halt - indicates VM where execution should be finised.
```