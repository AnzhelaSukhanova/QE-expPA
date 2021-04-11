(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and 
 (or (bvult (bvshl #b0001 y) x) (= (bvshl #b0001 y) x)) 
 (or (bvult #b0010 x) (= #b0010 x)) 
 (or (bvult z x) (= z x)))))
