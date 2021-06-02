(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and (bvult (bvadd y #b1111) x) (bvult x #b0001))))
