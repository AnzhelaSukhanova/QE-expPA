(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 6))
(declare-fun z () (_ BitVec 6))
(declare-fun t () (_ BitVec 6))
(assert
 (exists ((x (_ BitVec 6))) (and 
 (or (bvult x (bvmul #b000011 (bvshl #b000001 y))) (= x (bvmul #b000011 (bvshl #b000001 y)))) 
 (or (bvult (bvshl #b000001 z) x) (= (bvshl #b000001 z) x)) 
 (or (bvult x t) (= x t)))))
