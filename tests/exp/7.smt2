(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult (bvadd (bvshl #b0001 x) #b0010) (bvadd y #b1110)) (= (bvadd (bvshl #b0001 x) #b0010) (bvadd y #b1110)))))
