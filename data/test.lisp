(begin

; should all print true

(print (if (= (quote (1 2 3)) '(1 2 3)) true false))

(define f (lambda (x) (if (= x 5) true false)))
(print (f (int "5.5")))
(print (f (double "5.0")))

(define find
  (lambda (l v)
    (if (= (length l) 0)
      false
      (if (= (car l) v)
        true
        (find (cdr l) v)))))
(print (find (quote (1 2 3 4 5)) 5))
(print (not (find (quote (1 2 3 4 5)) 6)))

(define fib
  (lambda (n)
    (if (< n 2)
      n
      (+ (fib (- n 1)) (fib (- n 2))))))
(print (= (fib 15) 610))

(define t
  (table
    '((cat "tr" "u" "e") true)
    '("false" false)))
(print (get t "true"))

(define map
  (lambda (f l)
    (begin
      (define rmap
        (lambda (f l i)
          (if (not (= (length l) 0))
            (rmap f (cdr l) (cons i (f (car l))))
            i)))
      (rmap f l '()))))
(define by2
  (lambda (n)
    (* n 2)))
(print (= (map by2 '(1 2 3)) '(2 4 6)))

(print (= (string '(2 b or not 2.0 b)) "(2 b or not 2 b)"))

(print (not (find (map bool '(true 5.0 8 "string" name map)) false)))
(print (bool '(1 2 3)))
(print (bool (table '(1 2))))
(print (not (find (map bool '(false 0.0 0 "")) true)))
(print (not (bool '())))
(print (not (bool (table))))

)
