(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and
 (or (bvult x (bvadd (bvmul (bvshl #b0001 y) #b0011) (bvshl #b0001 z))) (= x (bvadd (bvmul (bvshl #b0001 y) #b0011) (bvshl #b0001 z))))
 (or (bvult (bvadd (bvshl #b0001 y) #b1010) x) (= (bvadd (bvshl #b0001 y) #b1010) x)))))
