(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4)))
 (or
 (or (bvult (bvshl #b0001 x) (bvadd y (bvadd (bvmul z #b0011) #b1000))) (= (bvshl #b0001 x) (bvadd y (bvadd (bvmul z #b0011) #b1000)))) 
 (or 
 (or (bvult (bvshl #b0001 x) (bvadd (bvshl #b0001 y) (bvadd (bvmul y #b1011) #b0100))) (= (bvshl #b0001 x) (bvadd (bvshl #b0001 y) (bvadd (bvmul y #b1011) #b0100))))
 (or 
 (or (bvult (bvmul (bvshl #b0001 y) #b0011) x) (= (bvmul (bvshl #b0001 y) #b0011) x)) 
 (or (bvult x (bvmul y #b1100)) (= x (bvmul y #b1100)))))))) 
