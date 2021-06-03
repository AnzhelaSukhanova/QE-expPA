# QE_expPA
[![Build Status](https://travis-ci.org/AnzhelaSukhanova/QE_expPA.svg?branch=main)](https://travis-ci.org/AnzhelaSukhanova/QE_expPA)  
The implementation of quantifier elimination for bit vector arithmetic based on elimination of quantifiers for Presburger arithmetic expanded by function <img src="https://render.githubusercontent.com/render/math?math=2^x"> (in progress).

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

The implementation is currently working for formulas like (instead of <img src="https://render.githubusercontent.com/render/math?math=\exists">there can be <img src="https://render.githubusercontent.com/render/math?math=\forall">, instead of ≤ there can be < or =)
* <img src="https://render.githubusercontent.com/render/math?math=\exists x:\bigwedge (g_j(\overline{y})\leq x \wedge \bigwedge x\leq g_i(\overline{y}))\wedge \bigwedge g_k(\overline{y})">, where <img src="https://render.githubusercontent.com/render/math?math=\overline{y}"> — free variables, <img src="https://render.githubusercontent.com/render/math?math=g_i(\overline{y}), g_j(\overline{y}), g_k(\overline{y})"> — terms in the signature of Presburger arithmetic expanded by <img src="https://render.githubusercontent.com/render/math?math=2^x">.  
* <img src="https://render.githubusercontent.com/render/math?math=\exists x:\bigwedge (2^x\leq g_i(\overline{y})\vee g_i(\overline{y})\leq 2^x)\wedge \bigwedge g_k(\overline{y})">.  

To run all tests and check the output use `bash all_tests.sh [-check]` or `bash all_tests.sh [-check] <folder>`  if you want to run tests from the specific folder from tests/.

## About
More details about the project can be found in **pres/slides.pdf**.
