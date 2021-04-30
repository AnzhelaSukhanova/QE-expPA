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
The folder **tests** contains five subfolders:  
* **simp_lin** contains tests with a trivial occurrence of the bound variable;  
* **simp_exp** contains those tests in which the bound variable occurs trivially in an exponential term;  
* **simp_mix** contains tests with conjunctions of formulas of said two types;  
* **lin** contains tests with a linear (non-trivial) occurrence of the bound variable;  
* **exp** contains those tests in which the bound variable occurs non trivially in an exponential term.  
The implementation is currently working for formulas like (instead of ≤ there can be < or =)
* <img src="https://render.githubusercontent.com/render/math?math=\exists x:\bigwedge (g_j(\overline{y})\leq x \wedge x\leq g_i(\overline{y}))">, where <img src="https://render.githubusercontent.com/render/math?math=\overline{y}"> — free variables, <img src="https://render.githubusercontent.com/render/math?math=g_i(\overline{y}), g_j(\overline{y})"> — terms in the signature of Presburger arithmetic expanded by <img src="https://render.githubusercontent.com/render/math?math=2^x">.  
* <img src="https://render.githubusercontent.com/render/math?math=\exists x:\bigwedge (2^x\leq g_i(\overline{y}))"> (in progress: <img src="https://render.githubusercontent.com/render/math?math=\exists x:\bigwedge (a \cdot 2^x %2B b \cdot x %2B c\leq g_i(\overline{y}))">, where a, b, c — fixed-size constants).  
* Сonjunctions of formulas of the first two types.  

To run all tests and check the output use `bash all_tests.sh` or `bash all_tests.sh <folder>`  if you want to run tests from the specific folder.

## About
More details about the project can be found in **pres/slides.pdf**.
