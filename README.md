# QE_expPA
[![Build Status](https://travis-ci.org/AnzhelaSukhanova/QE_expPA.svg?branch=main)](https://travis-ci.org/AnzhelaSukhanova/QE_expPA)  
The implementation of elimination of quantifiers for Presburger arithmetic expanded by function <img src="https://render.githubusercontent.com/render/math?math=2^x"> (in progress).

## Build
Download and build boolector according to the instructions given here: https://github.com/Boolector/boolector.  
Use `make`, to build the project.

## Use
`./main tests/<test_file> <out_file>`  
If you want check the result use `python check.py tests/<test_file> <out_file>`

## Tests
The folder **tests** contains two subfolders: **lin** and **exp**. The first contains tests with a linear occurrence of the bound variable, and the second contains those tests in which the bound variable occurs in an exponential term.  
The implementation is currently working for formulas like <img src="https://render.githubusercontent.com/render/math?math=\exists x:">⋀<img src="https://render.githubusercontent.com/render/math?math=(g_j(\overline{y})\leq x">∧<img src="https://render.githubusercontent.com/render/math?math=x\leq g_i(\overline{y}))">, where <img src="https://render.githubusercontent.com/render/math?math=\overline{y}"> — free variables, <img src="https://render.githubusercontent.com/render/math?math=g_i(\overline{y}), g_j(\overline{y})"> — terms in the signature of Presburger arithmetic expanded by <img src="https://render.githubusercontent.com/render/math?math=2^x"> without bound variables.  

**tests/lin** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y">  
* 2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y\cdot 3\leq x\wedge x\leq y\cdot 7">  
* 3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y\leq x\wedge 2\leq x\wedge z\leq x">   
* 4.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2\wedge 3\leq x">  
* 5.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y\cdot 3\leq x\wedge x\leq y\cdot 12">  
* 6.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y\cdot 3\wedge z\leq x\wedge x\leq t">  
* 7.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y %2B z">  
* 8.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y\cdot 5 %2B 8">  
* 9.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y\cdot 2 %2B z\wedge y\cdot 10\leq x">  
* 10.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y\cdot 5 %2B 7\wedge (y %2B z)\cdot 8\leq x">  
* 11.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y %2B ((9 %2B 2) %2B z)">  
* exp1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2^y">  
* exp2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:2^y\cdot 3\leq x\wedge x\leq 2^y\cdot 7">  
* exp3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:2^y\leq x\wedge 2\leq x\wedge z\leq x">   
* exp5.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:2^y\cdot 3\leq x\wedge x\leq y\cdot 12">  
* exp6.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2^y\cdot 3\wedge z\leq x\wedge x\leq t">  
* exp7.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2^y %2B z">  
* exp8.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2^y\cdot 5 %2B y %2B 8">  
* exp9.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2^y\cdot 2 %2B z\wedge 2^y\cdot 10\leq x">  
* exp10.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2^y\cdot 5 %2B 7\wedge (2^y %2B z)\cdot 8\leq x">  

**tests/exp** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:2^x\leq y">  

To run all tests and check the output use `sh all_tests.sh`

## About
More details about the project can be found in **pres/report.pdf**.
