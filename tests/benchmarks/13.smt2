(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 8))
(declare-fun z () (_ BitVec 8))
(assert
 (exists ((x (_ BitVec 8))) (and 
 (or (bvult x (bvadd (bvmul (bvshl #b00000001 y) #b00000101) #b00000111)) (= x (bvadd (bvmul (bvshl #b00000001 y) #b00000101) #b00000111))) 
 (or (bvult (bvmul (bvadd (bvshl #b00000001 y) z) #b00001000) x) (= (bvmul (bvadd (bvshl #b00000001 y) z) #b00001000) x)))))
