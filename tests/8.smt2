(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4)))
 (or (bvult x (bvadd (bvmul y #b0101) #b1000)) (= x (bvadd (bvmul y #b0101) #b1000)))))
