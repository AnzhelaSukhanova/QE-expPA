(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 10))
(declare-fun z () (_ BitVec 10))
(declare-fun t () (_ BitVec 10))
(assert
 (exists ((x (_ BitVec 10))) (and 
 (or (bvult x (bvmul #b1111100101 y)) (= x (bvmul #b1111100101 y))) 
 (or (bvult z x) (= z x)) 
 (or (bvult x t) (= x t)))))
