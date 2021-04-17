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
The implementation is currently working for formulas like
* <img src="https://render.githubusercontent.com/render/math?math=\exists x:">⋀<img src="https://render.githubusercontent.com/render/math?math=(g_j(\overline{y})\leq x">∧<img src="https://render.githubusercontent.com/render/math?math=x\leq g_i(\overline{y}))">, where <img src="https://render.githubusercontent.com/render/math?math=\overline{y}"> — free variables, <img src="https://render.githubusercontent.com/render/math?math=g_i(\overline{y}), g_j(\overline{y})"> — terms in the signature of Presburger arithmetic expanded by <img src="https://render.githubusercontent.com/render/math?math=2^x">.  
* <img src="https://render.githubusercontent.com/render/math?math=\exists x: 2^x + c\leq t(\overline{y})">, where <img src="https://render.githubusercontent.com/render/math?math=t(\overline{y})"> — terms in the signature of Presburger arithmetic expanded by <img src="https://render.githubusercontent.com/render/math?math=2^x">.  

To run all tests and check the output use `sh all_tests.sh`

## About
More details about the project can be found in **pres/report.pdf**.
