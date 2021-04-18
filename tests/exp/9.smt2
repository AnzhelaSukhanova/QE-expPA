(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult (bvadd (bvshl #b0001 x) x) (bvadd (bvadd (bvshl #b0001 y) y) #b0001)) (= (bvadd (bvshl #b0001 x) x) (bvadd (bvadd (bvshl #b0001 y) y) #b0001)))))
