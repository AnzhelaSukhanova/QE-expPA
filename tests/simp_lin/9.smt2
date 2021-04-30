(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (and
 (or (bvult x (bvadd (bvmul y #b0011) z)) (= x (bvadd (bvmul y #b0011) z)))
 (or (bvult (bvadd y #b1010) x) (= (bvadd y #b1010) x)))))
