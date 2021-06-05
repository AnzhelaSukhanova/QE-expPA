(set-option :produce-models true)
(set-logic BV)
(declare-fun y () (_ BitVec 6))
(declare-fun z () (_ BitVec 6))
(assert
 (exists ((x (_ BitVec 6))) (or (bvult (bvadd y (bvadd (bvmul z #b000011) #b001000)) (bvshl #b000001 x)) (= (bvadd y (bvadd (bvmul z #b000011) #b001000)) (bvshl #b000001 x)))))
