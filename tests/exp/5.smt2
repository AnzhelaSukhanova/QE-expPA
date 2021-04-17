(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 4))
(declare-fun z () (_ BitVec 4))
(assert
 (exists ((x (_ BitVec 4))) (or (bvult (bvshl #b0001 x) (bvadd y (bvadd (bvmul z #b0011) #b1000))) (= (bvshl #b0001 x) (bvadd y (bvadd (bvmul z #b0011) #b1000))))))
