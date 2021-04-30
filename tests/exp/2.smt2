(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult (bvmul (bvshl #b0001 x) #b1011) (bvadd y #b1100)) (= (bvmul (bvshl #b0001 x) #b1011) (bvadd y #b1100)))))
