(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4)))
 (or (bvult x (bvadd (bvadd (bvmul (bvshl #b0001 y) #b0101) y) #b1000)) (= x (bvadd (bvadd (bvmul (bvshl #b0001 y) #b0101) y) #b1000)))))
