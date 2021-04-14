(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult (bvshl #b0001 x) (bvadd (bvmul (bvshl #b0001 y) #b0101) #b0111)) (= (bvshl #b0001 x) (bvadd (bvmul (bvshl #b0001 y) #b0101) #b0111)))))
