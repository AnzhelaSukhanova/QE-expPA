(set-option :produce-models true)
(set-logic BV)
(assert
 (exists ((CRRNT_PWR_1 (_ BitVec 16))) (or (bvugt CRRNT_PWR_1 #x2521) (= CRRNT_PWR_1 #x2521))))
