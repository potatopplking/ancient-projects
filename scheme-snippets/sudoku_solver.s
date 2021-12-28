#!/usr/bin/gsi
; zadani
(define s1 '(	0 0 1 9 2 4 3 5 0
				3 0 0 0 1 7 0 0 9
				2 6 9 8 3 0 4 0 0
				0 9 0 0 0 0 1 0 0
				0 0 0 0 0 0 0 0 0
				0 5 0 2 0 0 0 0 0
				0 0 0 0 0 8 0 0 3
				0 0 0 0 0 0 0 0 0
				8 0 0 3 4 1 0 2 0))

(define s2 '(	0 0 4 0 0 0 0 3 0
				0 8 0 0 0 0 0 0 5
				0 9 0 0 3 6 4 8 0
				2 4 0 0 0 0 0 0 0
				0 3 0 7 0 5 0 4 0
				1 6 5 0 0 4 0 0 2
				0 5 0 0 9 3 8 0 0
				0 0 0 0 0 2 0 0 4
				0 0 9 8 6 7 0 0 0))

(define s3 '(	0 2 9 0 5 6 0 0 0
				0 4 0 0 0 0 0 0 0
				5 0 0 0 0 0 0 9 6
				0 0 7 0 0 8 0 0 1
				3 0 0 0 7 0 0 5 9
				0 5 8 6 0 3 2 7 0
				9 7 0 0 6 0 0 0 0
				0 0 0 0 1 4 0 6 0
				2 0 0 3 0 0 0 0 0))

(define s4 '(	0 0 0 5 0 3 6 0 0
				0 0 0 8 6 0 7 0 0
				0 0 8 0 7 4 0 0 0
				0 8 0 0 4 9 0 3 0
				4 0 0 0 0 8 0 2 6
				0 0 5 2 0 0 0 0 8
				5 0 0 1 0 2 3 0 7
				0 0 4 0 0 0 0 0 0
				0 0 0 4 0 7 0 0 9))

(define s5 '(	0 0 0 3 0 0 1 0 6 
				0 0 0 0 0 0 0 0 0 
				1 0 0 0 0 4 8 0 0 
				8 2 0 0 0 0 0 0 0 
				0 0 0 6 0 0 5 3 8 
				0 0 3 0 0 1 0 0 7 
				0 3 0 0 0 8 0 9 0 
				0 5 0 0 7 0 0 0 4 
				0 9 0 0 4 2 0 0 0 ))

; definice pouzitych funkci
(define print_sudoku (lambda (x) 
	(do ((i 0 (+ i 1))) ((= i 9))
		(do ((j 0 (+ j 1))) ((= j 9)) (display (car x)) (display " ") (set! x (cdr x))) (display "\n"))))

; vrati n prvku z listu l
(define get_n (lambda (l n) (if (= n 0) '() (cons (car l) (get_n (cdr l) (- n 1))))))

; vrati count prvku z listu l od adresy h
(define slice (lambda (l start count) (if (> start 0) (slice (cdr l) (- start 1) count) (get_n l count))))

; vrati radek, do ktereho spada dane cislo
(define get_row (lambda (mat pos) (let ((start (- pos (modulo pos 9))))
	(slice mat start 9))))

; vrati nasledujici prvek ve sloupci
(define get_next (lambda (l n)
	(if (null? l) '()
	(if (= n 0) (cons (car l) (get_next (cdr l) 8)) (get_next (cdr l) (- n 1))))))

; vrati sloupec, do ktereho spada dane cislo
(define get_col (lambda (mat pos) (let ((col (modulo pos 9)))
	(get_next mat col))))

; vrati "maly ctverec" 3x3, do ktereho spada dane cislo
(define get_sq (lambda (mat pos) (let (
	(px (floor (/ (modulo pos 9) 3)))
	(py (floor (/ (floor (/ pos 9)) 3))))
	(let ((start (+ (* 27 py) (* 3 px))))
	(append (slice mat start 3) (slice mat (+ start 9) 3) (slice mat (+ start 18) 3))))))

; vrati mnozinu bez nulovych prvku
(define emptiless (lambda (l)
	(if (null? l) '() (if (not (= (car l) 0)) (cons (car l) (emptiless (cdr l))) (emptiless (cdr l))))))

(define incl? (lambda (set sym) (if (null? set) #f (if (eqv? (car set) sym) #t (incl? (cdr set) sym)))))

; sjednoceni mnozin
(define disj (lambda (u v)
	(if (null? v) u (if (incl? u (car v)) (disj u (cdr v)) (disj (append u (list (car v))) (cdr v))))))

; prunik mnozin
(define conj (lambda (u v)
	(if (null? v) '() (if (incl? u (car v)) (cons (car v) (conj u (cdr v))) (conj u (cdr v))))))

; umozni pouziti procedury pro 2 argumenty s neomezenym mnozstvim argumentu (v listu) ... (a (b (c (d (e f))))))
(define many (lambda (f l)
	(if (= 1 (length l)) (car l) (f (car l) (many f (cdr l))))))

; rozdil mnozin
(define set_diff (lambda (u v)
	(if (null? u) '() (if (incl? v (car u)) (set_diff (cdr u) v) (cons (car u) (set_diff (cdr u) v))))))

; "inverze" z hlediska abecedy symbolu, tj. 1-9
(define inv_f (lambda (u c)
	(if (= c 10) '() (if (incl? u c) (inv_f u (+ c 1)) (cons c (inv_f u (+ c 1)))))))
(define inv (lambda (u) (inv_f u 1)))

; vrati list moznych symbolu pro pozici pos
(define psbl (lambda (mat pos)
	(inv (disj (disj (emptiless (get_row mat pos)) (emptiless (get_col mat pos))) (emptiless (get_sq mat pos))))))

; nastaveni hodnoty v listu l na pozici i na hodnotu val
(define (list-set! l i val)
	(if (zero? i)
		(set-car! l val)
		(list-set! (cdr l) (- i 1) val)))

; vrati list listu moznych symbolu pro vsechny pozice
(define psblts (lambda (mat pos)
	(if (= pos 81) '()
		(if (= 0 (list-ref mat pos))
			(cons (psbl mat pos) (psblts mat (+ pos 1)))
			(cons '() (psblts mat (+ pos 1)))))))

; vrati #f pokud sudoku "mat" neni hotove, jinak vrati #t
(define solved? (lambda (mat) (= 81 (length (emptiless mat)))))

; vrati #t pokud je mozne z listu pos_syms neco doplnit
(define can_fill? (lambda (pos_syms)
	(if (null? pos_syms) #f (if (= 1 (length (car pos_syms))) #t (can_fill? (cdr pos_syms))))))

; zapise do sudoku "mat" cisla tam, kde je v pos_syms jen jedna moznost
(define fill (lambda (mat pos_syms)
	(if (not (null? mat))
		(if (= 1 (length (car pos_syms)))
			(and (set-car! mat (car (car pos_syms))) (fill (cdr mat) (cdr pos_syms)))
			(fill (cdr mat) (cdr pos_syms))))))

; porovnani listu
(define leqv? (lambda (u v)
	(if (not (= (length u) (length v))) #f
	(if (null? u) #t (if (eqv? (car u) (car v)) (leqv? (cdr u) (cdr v)) #f)))))

; smaze z listu hodnotu val (jen jednou)
(define del (lambda (l val)
	(if (null? l) '() (if (leqv? (car l) val) (cdr l) (cons (car l) (del (cdr l) val))))))

; funkce pro ziskani moznych symbolu vylucovaci metodou 
(define rem_sym (lambda (mat pos)
	(let ((cur (list-ref mat pos)))
	(let (
		(row (set_diff cur (many disj (del (get_row mat pos) cur))))
		(col (set_diff cur (many disj (del (get_col mat pos) cur))))
		(sq (set_diff cur (many disj (del (get_sq mat pos) cur)))))
		(cond
			; pokud je delka 1, je jasne co vratit			
			((= (length row) 1) row)
			((= (length col) 1) col)
			((= (length sq) 1) sq)
			((= (length (conj row col)) 1) (conj row col))
			((= (length (conj row sq)) 1) (conj row sq))
			((= (length (conj col sq)) 1) (conj col sq))
			((= (length (many conj (list col sq row))) 1) (many conj (list col sq row)))
			(else '()))))))

; funkce pro ziskani moznych symbolu vylucovaci metodou pro vsechny pozice
(define rem_syms (lambda (mat pos)
	(if (= pos 81) '()
		(if (not (null? (list-ref mat pos)))
			(cons (rem_sym mat pos) (rem_syms mat (+ pos 1)))
			(cons '() (rem_syms mat (+ pos 1)))))))

; hlavni cyklus
(define solve (lambda (mat)
	(let ((pos_syms (psblts mat 0)))
		(if (can_fill? pos_syms)
			(fill mat pos_syms)
			(let ((rem_pos (rem_syms pos_syms 0)))
				(if (can_fill? rem_pos)
				(fill mat rem_pos)
				(and
					(display "wtf is this error? You shouldn't get here!\n")
					(print_sudoku mat)
					(newline)
					(print_sudoku rem_pos)
					(newline)
					(print_sudoku pos_syms)
					(exit)))))
		(if (solved? mat)
			(display "Done!\n")
			(solve mat)))))

; telo skriptu
(let ((zadani s4)) ; zde menit zadani
	(display "Zadani:\n")
	(print_sudoku zadani)
	(solve zadani)
	(display "Vysledek:\n")
	(print_sudoku zadani))
