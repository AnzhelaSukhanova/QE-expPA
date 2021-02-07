# QE_expPA
The implementation of elimination of quantifiers for Presburger arithmetic expanded by function 2^x (in progress).

## Build
Download and build boolector with support for the **MiniSAT** according to the instructions given here: https://github.com/Boolector/boolector.  
Use `make`, to build the project.

## Use
`./main tests/<test_file> <out_file>`

## Tests
The folder **tests** contains the following examples:  
1.in -- ∃ x: x<=y  
2.in -- ∃ x: 3*x<=y & y<=6*x  
3.in -- ∃ x: y<=6*y & y<=3*x & y<=2*x  
