(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 8))
(declare-fun z () (_ BitVec 8))
(assert
 (forall ((x (_ BitVec 8))) (and
 (or (bvult x (bvadd (bvmul (bvshl #b00000001 y) #b00000011) (bvshl #b00000001 z))) (= x (bvadd (bvmul (bvshl #b00000001 y) #b00000011) (bvshl #b00000001 z))))
 (or (bvult (bvadd (bvshl #b00000001 y) #b00001010) x) (= (bvadd (bvshl #b00000001 y) #b00001010) x)))))
