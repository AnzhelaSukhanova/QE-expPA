(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and 
 (or (bvult (bvmul (bvshl #b0001 y) #b0011) x) (= (bvmul (bvshl #b0001 y) #b0011) x)) 
 (or (bvult x (bvmul y #b1100)) (= x (bvmul y #b1100))))))
