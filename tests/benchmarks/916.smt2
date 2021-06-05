(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 8))
(declare-fun z () (_ BitVec 8))
(declare-fun t () (_ BitVec 8))
(assert
 (exists ((x (_ BitVec 8))) (and 
 (or (bvult (bvshl #b00000001 x) (bvmul #b00000111 y)) (= (bvshl #b00000001 x) (bvmul #b00000111 y))) 
 (or (bvult (bvshl #b00000001 x) z) (= (bvshl #b00000001 x) z)) 
 (or (bvult (bvshl #b00000001 x) (bvshl #b00000001 t)) (= (bvshl #b00000001 x) (bvshl #b00000001 t))))))
