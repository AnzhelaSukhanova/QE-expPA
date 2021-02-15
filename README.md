# QE_expPA
The implementation of elimination of quantifiers for Presburger arithmetic expanded by function 2^x (in progress).

## Build
Download and build boolector with support for the **MiniSAT** according to the instructions given here: https://github.com/Boolector/boolector.  
Use `make`, to build the project.

## Use
`./main tests/<test_file> <out_file>`

## Tests
The folder **tests** contains the following examples:  
* 1.in — ∃ x: x ≤ y  
out: true  
* 2.in — ∃ x: y\*3 ≤ x ∧ x ≤ y\*7  
* 3.in — ∃ x: y ≤ x ∧ 2 ≤ x ∧ z ≤ x   
* 4.in — ∃ x: x ≤ 2 ∧ 3 ≤ x  
out: false  
* 5.in — ∃ x: x ≤ y ∧ z ≤ x  
out: (not (bvult y z))  
* 6.in — ∃ x: x ≤ y\*3 ∧ z ≤ x ∧ x ≤ t  
* 8.in — ∃ x: y\*3 ≤ x ∧ x ≤ y\*6  

To run all tests use `bash all_tests.sh`
