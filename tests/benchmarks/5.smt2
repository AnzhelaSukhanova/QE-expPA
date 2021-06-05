(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 6))
(declare-fun z () (_ BitVec 6))
(assert
 (exists ((x (_ BitVec 6))) (and
 (or (bvult x (bvadd (bvmul y #b000011) z)) (= x (bvadd (bvmul y #b000011) z)))
 (or (bvult (bvadd y #b001010) x) (= (bvadd y #b001010) x)))))
