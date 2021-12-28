; soucet vektoru
;(define add_v (lambda (x y) (if (null? x) '() (cons (+ (car x) (car y)) (add_v (cdr x) (cdr y))))))
(define add_v (lambda (u v) (map (lambda (x y) (+ x y)) u v)))
; suma clenu vektoru
(define sum_v (lambda (x) (let ((sum 0)) (if (null? x) sum (+ sum (car x) (sum_v (cdr x)))))))
; nasobeni vektoru clen po clenu
;(define mul_v (lambda (x y) (if (null? x) '() (cons (* (car x) (car y)) (mul_v (cdr x) (cdr y))))))
(define mul_v (lambda (u v) (map (lambda (x y) (* x y)) u v)))
; skalarni soucin vektoru
(define dot (lambda (x y) (sum_v (mul_v x y))))

(display (dot '(1 2 3 4 5) '(2 3 4 5 6)))
(display "\n")