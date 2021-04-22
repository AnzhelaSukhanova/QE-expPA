(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and
 (or (bvult (bvshl #b0001 x) z) (= (bvshl #b0001 x) z))
 (or (bvult x y) (= x y))))) 
