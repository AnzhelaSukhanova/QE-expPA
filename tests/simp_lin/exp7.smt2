(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4)))
 (or (bvult x (bvadd (bvshl #b0001 y) z)) (= x (bvadd (bvshl #b0001 y) z)))))
