(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult (bvshl #b0001 x) (bvadd (bvmul y #b1011) #b0100)) (= (bvshl #b0001 x) (bvadd (bvmul y #b1011) #b0100)))))
