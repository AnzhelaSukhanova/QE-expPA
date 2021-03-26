(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and 
 (or (bvult x #b0010) (= x #b0010)) 
 (or (bvult #b0011 x) (= #b0011 x)))))
