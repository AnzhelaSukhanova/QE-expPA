(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 6))
(assert
 (exists ((x (_ BitVec 6))) (and 
 (or (bvult (bvmul (bvshl #b000001 y) #b000011) x) (= (bvmul (bvshl #b000001 y) #b000011) x)) 
 (or (bvult x (bvmul y #b001100)) (= x (bvmul y #b001100))))))
