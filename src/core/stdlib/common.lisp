(define (empty lst) (null? lst))
(define (empty? lst) (null? lst))
(define (pair? lst) (cons? lst))

(define (cadr lst) (cdr (car lst)))
(define (caddr lst) (cdr (cdr (car lst))))
(define (cadddr lst) (cdr (cdr (cdr (car lst)))))

(define (first lst) (car lst))
(define (rest lst) (cdr lst))
(define (second lst) (cadr lst))
(define (third lst) (caddr lst))
(define (fourth lst) (cadddr lst))

(define (list . x) x)

(define (length lst)
  (if (null? lst)
    0
    (+ (length (cdr lst)) 1)))

(define (list-tail lst n)
  (if (= n 0)
    lst
    (list-tail (cdr lst) (- n 1))))

(define (list-ref lst n)
  (car (list-tail lst n)))

(define (take lst n)
  (define (take-impl lst lst-res n)
    (if (= n 0)
      lst-res
      (take-impl (cdr lst) (cons (car lst) lst-res) (- n 1))))
  (reverse (take-impl lst '() n)))

(define (append lst x)
  (if (null? lst)
    x
    (cons (car lst) (append (cdr lst) x))))

(define (reverse lst)
  (define (reverse-impl lst lst-res)
    (if (null? lst)
      lst-res
      (reverse-impl (cdr lst) (cons (car lst) lst-res))))
  (reverse-impl lst '()))

(define (map proc lst)
  (if (null? lst)
    lst
    (cons (proc (car lst)) (map proc (cdr lst)))))

(define (filter proc lst)
  (cond
    ((null? lst) lst)
    ((proc (car lst)) (cons (car lst) (filter proc (cdr lst))))
    (#t (filter prox (cdr lst)))))

(define (member v lst)
  (if (null? lst)
    #f
    (or (= (car lst) v) (member v (cdr lst)))))

(define (foldr op lst)
  (cond
    ((null? lst) '())
    ((null? (cdr lst)) (car lst))
    (#t (op (car lst) (foldr op (cdr lst))))))

(define (make-list n v)
  (if (= n 0)
    '()
    (cons v (make-list (- n 1)))))


(define (max frst . rest)
  (define (max2 x y) (if (> x y) x y))
  (if (not (number? frst)) (throw "number expected, got " frst))
  (foldr max2 (cons frst rest)))

(define (min frst . rest)
  (define (min2 x y) (if (< x y) x y))
  (if (not (number? frst)) (throw "number expected, got " frst))
  (foldr min2 (cons frst rest)))

(define (abs x)
  (if (< x 0) (- x) x))
