(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (forall ((x (_ BitVec 4))) (and 
 (or (bvult (bvmul y #b0011) x) (= (bvmul y #b0011) x)) 
 (or (bvult x (bvmul y #b1100)) (= x (bvmul y #b1100))))))
