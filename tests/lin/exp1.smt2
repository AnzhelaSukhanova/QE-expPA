(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult x (bvshl #b0001 y)) (= x (bvshl #b0001 y)))))
