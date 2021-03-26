# QE_expPA
[![Build Status](https://travis-ci.org/AnzhelaSukhanova/QE_expPA.svg?branch=main)](https://travis-ci.org/AnzhelaSukhanova/QE_expPA)  
The implementation of elimination of quantifiers for Presburger arithmetic expanded by function 2^x (in progress).

## Build
Download and build boolector according to the instructions given here: https://github.com/Boolector/boolector.  
Use `make`, to build the project.

## Use
`./main tests/<test_file> <out_file>`

## Tests
The implementation is currently working for formulas like **∃ x: (⋀ s\_i (ȳ) ≤ x) ∧ (⋀ x ≤ s\_j (ȳ))**, where ȳ — variables, s\_i(ȳ) — linear combination of variables and constant fixed size bit vectors.  

The folder **tests** contains the following examples:  
* lin/1.smt2 — ∃ x: x ≤ y  
out: (not (and (not (= y #b0001)) (not (= y #b0000)) (not (bvult #b0001 y))))  
* lin/2.smt2 — ∃ x: y\*3 ≤ x ∧ x ≤ y\*7  
* lin/3.smt2 — ∃ x: y ≤ x ∧ 2 ≤ x ∧ z ≤ x   
* lin/4.smt2 — ∃ x: x ≤ 2 ∧ 3 ≤ x  
out: false  
* lin/5.smt2 — ∃ x: y\*3 ≤ x ∧ x ≤ y\*12  
* lin/6.smt2 — ∃ x: x ≤ y\*3 ∧ z ≤ x ∧ x ≤ t  
* lin/7.smt2 — ∃ x: x ≤ y + z  
* lin/8.smt2 — ∃ x: x ≤ y\*5 + 8  
* lin/9.smt2 — ∃ x: x ≤ y\*2 + z ∧ y\*10 ≤ x  
* lin/10.smt2 — ∃ x: x ≤ y\*5 + 7 ∧ (y + z)\*8 ≤ x  
* lin/11.smt2 — ∃ x: x ≤ y + ((9 + 2) + z)  

To run all tests and check output use `sh all_tests.sh`
