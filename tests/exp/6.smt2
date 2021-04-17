(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(declare-fun t () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and 
 (or (bvult (bvshl #b0001 x) (bvmul #b0111 y)) (= (bvshl #b0001 x) (bvmul #b0111 y))) 
 (or (bvult (bvshl #b0001 x) z) (= (bvshl #b0001 x) z)) 
 (or (bvult (bvshl #b0001 x) (bvshl #b0001 t)) (= (bvshl #b0001 x) (bvshl #b0001 t))))))
