(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult (bvshl #b0010 x) y) (= (bvshl #b0010 x) y))))
