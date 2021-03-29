# QE_expPA
[![Build Status](https://travis-ci.org/AnzhelaSukhanova/QE_expPA.svg?branch=main)](https://travis-ci.org/AnzhelaSukhanova/QE_expPA)  
The implementation of elimination of quantifiers for Presburger arithmetic expanded by function 2^x (in progress).

## Build
Download and build boolector according to the instructions given here: https://github.com/Boolector/boolector.  
Use `make`, to build the project.

## Use
`./main tests/<test_file> <out_file>`  
If you want check the result use `python check.py tests/<test_file> <out_file>`

## Tests
The folder **tests** contains two subfolders: **lin** and **exp**. The first contains tests with a linear occurrence of the bound variable, and the second contains those tests in which the bound variable occurs in an exponential term.  
The implementation is currently working for formulas like <img src="https://render.githubusercontent.com/render/math?math=\exists x:$\bigwedge\limits_{1\leq i\leq p, 1\leq j\leq q} g_j(\overline{y})\leq x\wedge x\leq g_i(\overline{y})$">, where <img src="https://render.githubusercontent.com/render/math?math=$\overline{y}$"> — free variables, <img src="https://render.githubusercontent.com/render/math?math=$g_i(\overline{y}), g_j(\overline{y})$"> — linear combination of variables and constant fixed size bit vectors.  

**tests/lin** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y">  
* 2.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y*3\leq x\wedge x\leq y*7">  
* 3.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y\leq x\wedge 2\leq x\wedge z\leq x">   
* 4.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq 2\wedge 3\leq x">  
* 5.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:y*3\leq x\wedge x\leq y*12">  
* 6.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y*3\wedge z\leq x\wedge x\leq t">  
* 7.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y + z">  
* 8.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y*5 + 8">  
* 9.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y*2 + z\wedge y*10\leq x">  
* 10.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y*5 + 7\wedge (y + z)*8\leq x">  
* 11.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x\leq y + ((9 + 2) + z)">  

**tests/exp** contains the following examples:  
* 1.smt2 — <img src="https://render.githubusercontent.com/render/math?math=\exists x:x^2\leq y"> (in progress)  

To run all tests and check the output use `sh all_tests.sh`

## About
More details about the project can be found in **pres/report.pdf**.
