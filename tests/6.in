(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(declare-fun t () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and 
 (or (bvult x (bvmul #b0011 y)) (= x (bvmul #b0011 y))) 
 (or (bvult z x) (= z x)) 
 (or (bvult x t) (= x t)))))
