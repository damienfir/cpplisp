#include "catch.hpp"

#include "../src/lisp.h"

TEST_CASE("Basic arithmetic") {
  auto res = eval_program("(+ 1 2)");
  REQUIRE(std::get<Number>(res) == 3);
}

TEST_CASE("Nested arithmetic") {
  auto res = eval_program("(+ (- 0 1 2) (+ 1 9 10))");
  REQUIRE(std::get<Number>(res) == 17);
}

TEST_CASE("Let") {
  SECTION("base case") {
    auto res = eval_program("(let (x 1 y 2) (+ x (* 1 y)))");
    REQUIRE(std::get<Number>(res) == 3);
  }

  SECTION("second var depend on first var") {
    auto res = eval_program("(let (x 1 y (+ x 1)) (+ x (* 1 y)))");
    REQUIRE(std::get<Number>(res) == 3);
  }
}

TEST_CASE("println") {
  REQUIRE_NOTHROW(eval_program("(println)"));
  REQUIRE_NOTHROW(eval_program("(println 1)"));
  REQUIRE(std::holds_alternative<Nil>(eval_program("(println 1)")));
}

TEST_CASE("do/define") {
  auto res = eval_program("(do (define x 1) (define y 2) (+ x y))");
  REQUIRE(std::get<Number>(res) == 3);
}

TEST_CASE("lambda") {
  SECTION("basic") {
    auto res = eval_program("((lambda (x) (+ x 1)) 1)");
    REQUIRE(std::get<Number>(res) == 2);
  }

  SECTION("no arguments") {
    auto res = eval_program("((lambda () (+ 1 1)))");
    REQUIRE(std::get<Number>(res) == 2);
  }
}

TEST_CASE("if") {
  auto res = eval_program("(if (= 1 1) 1 2)");
  REQUIRE(std::get<Number>(res) == 1);
}

TEST_CASE("if 2") {
  auto res = eval_program("(if (= 1 2) 1 2)");
  REQUIRE(std::get<Number>(res) == 2);
}

TEST_CASE("equals") {
  SECTION("true") {
    auto res = eval_program("(= 1 1)");
    REQUIRE(std::get<bool>(res));
  }

  SECTION("false") {
    auto res = eval_program("(= 1 2)");
    REQUIRE(!std::get<bool>(res));
  }
}

TEST_CASE("lt") {
  SECTION("true") {
    auto res = eval_program("(< 1 2)");
    REQUIRE(std::get<bool>(res));
  }
  SECTION("false") {
    auto res = eval_program("(< 2 1)");
    REQUIRE(!std::get<bool>(res));
  }
}

TEST_CASE("gt") {
  SECTION("true") {
    auto res = eval_program("(> 3 2)");
    REQUIRE(std::get<bool>(res));
  }

  SECTION("false") {
    auto res = eval_program("(> 2 3)");
    REQUIRE(!std::get<bool>(res));
  }
}

TEST_CASE("gte") {
  SECTION("true gt") {
    auto res = eval_program("(>= 3 2)");
    REQUIRE(std::get<bool>(res));
  }

  SECTION("true equal") {
    auto res = eval_program("(>= 2 2)");
    REQUIRE(std::get<bool>(res));
  }

  SECTION("false") {
    auto res = eval_program("(>= 1 2)");
    REQUIRE(!std::get<bool>(res));
  }
}

TEST_CASE("lte") {
  SECTION("true lt") {
    auto res = eval_program("(<= 1 2)");
    REQUIRE(std::get<bool>(res));
  }
  SECTION("true equals") {
    auto res = eval_program("(<= 2 2)");
    REQUIRE(std::get<bool>(res));
  }
  SECTION("false") {
    auto res = eval_program("(<= 3 2)");
    REQUIRE(!std::get<bool>(res));
  }
}

TEST_CASE("and") {
  SECTION("true") {
    auto res = eval_program("(and 1 1)");
    REQUIRE(std::get<bool>(res));
  }

  SECTION("false") {
    auto res = eval_program("(and 1 0)");
    REQUIRE(!std::get<bool>(res));
  }

  SECTION("multiple") {
    auto res = eval_program("(and 1 1 1 1 1 1)");
    REQUIRE(std::get<bool>(res));
  }
}

TEST_CASE("or") {
  SECTION("true") {
    auto res = eval_program("(or 1 0)");
    REQUIRE(std::get<bool>(res));
  }

  SECTION("false") {
    auto res = eval_program("(or 0 0)");
    REQUIRE(!std::get<bool>(res));
  }

  SECTION("multiple") {
    auto res = eval_program("(or 0 0 0 1 0)");
    REQUIRE(std::get<bool>(res));
  }
}

TEST_CASE("list") {
  SECTION("init") {
    auto res = eval_program("(list 1 2 3)");
    auto l = std::get<List>(res).list;
    REQUIRE(std::get<Number>(l[0]) == 1);
    REQUIRE(std::get<Number>(l[1]) == 2);
    REQUIRE(std::get<Number>(l[2]) == 3);
  }

  SECTION("empty list") {
    auto res = eval_program("(list)");
    REQUIRE(std::get<List>(res).list.empty());
  }

  SECTION("list first") {
    auto res = eval_program("(first (list 1 2 3))");
    REQUIRE(std::get<Number>(res) == 1);
  }

  SECTION("list rest") {
    auto res = eval_program("(rest (list 1 2 3))");
    auto l = std::get<List>(res).list;
    REQUIRE(std::get<Number>(l[0]) == 2);
    REQUIRE(std::get<Number>(l[1]) == 3);
  }

  SECTION("length 0") {
    auto res = eval_program("(length (list))");
    REQUIRE(std::get<Number>(res) == 0);
  }

  SECTION("length") {
    auto res = eval_program("(length (list 1 2))");
    REQUIRE(std::get<Number>(res) == 2);
  }
}

TEST_CASE("recursion") {
  SECTION("simple sum") {

    auto res = eval_program(R"lisp(
   (define fn (lambda (x)
       (do
           (if (= x 1)
               1
               (+ x (fn (- x 1)))))))
   (fn 10)
)lisp");
    REQUIRE(std::get<Number>(res) == 55);
  }

  SECTION("factorial") {
    auto res = eval_program(R"lisp(
(define factorial (lambda (n)
    (if (= n 0)
        1
        (* n (factorial (- n 1))))))
(factorial 10))lisp");
    REQUIRE(std::get<Number>(res) == 3628800);
  }
}

TEST_CASE("multi-instruction") {
  auto res = eval_program("(define x 1) (define y 2) (+ x y)");
  REQUIRE(std::get<Number>(res) == 3);
}

TEST_CASE("cond") {
  auto res = eval_program("(cond ((= 1 2) 1) ((= 1 1) 2))");
  REQUIRE(std::get<Number>(res) == 2);
}

TEST_CASE("string") {
  auto res = eval_program("(do (define s \"hello, world!\") s)");
  REQUIRE(std::get<String>(res) == "hello, world!");
}

TEST_CASE("comments") {
  auto res = eval_program(R"lisp(
(define x 1)
; this is a comment
(define x 2)
x
)lisp");
  REQUIRE(std::get<Number>(res) == 2);
}

TEST_CASE("cons") {
  auto res = eval_program("(cons 1 (list 2 3))");
  auto list = std::get<List>(res).list;
  REQUIRE(std::get<Number>(list[0]) == 1);
  REQUIRE(std::get<Number>(list[1]) == 2);
  REQUIRE(std::get<Number>(list[2]) == 3);
}

TEST_CASE("cons empty") {
  auto res = eval_program("(cons 1 (list))");
  auto list = std::get<List>(res).list;
  REQUIRE(list.size() == 1);
  REQUIRE(std::get<Number>(list[0]) == 1);
}

TEST_CASE("list empty", "[stdlib]") {
  SECTION("true") {
    auto res = eval_program_with_stdlib("(empty? (list))");
    REQUIRE(std::get<bool>(res));
  }

  SECTION("false") {
    auto res = eval_program_with_stdlib("(empty? (list 1 2))");
    REQUIRE(!std::get<bool>(res));
  }
}

TEST_CASE("map", "[stdlib]") {
  auto res =
      eval_program_with_stdlib("(map (lambda (x) (+ x 1)) (list 1 2 3))");
  auto l = std::get<List>(res).list;
  REQUIRE(std::get<Number>(l[0]) == 2);
  REQUIRE(std::get<Number>(l[1]) == 3);
  REQUIRE(std::get<Number>(l[2]) == 4);
}

TEST_CASE("closures") {
  SECTION("from global scope") {
    auto res = eval_program("(define x 1) (define fn (lambda (y) (+ x y))) (fn 2)");
    REQUIRE(std::get<Number>(res) == 3);
  }

  SECTION("from local scope") {
    auto res = eval_program("(define fn (let (x 1) (lambda (y) (+ x y)))) (fn 2)");
    REQUIRE(std::get<Number>(res) == 3);
  }
}

TEST_CASE("scopes") {
  SECTION("nested") {
    auto res = eval_program("(let (x 1) (let (y 2) (+ x y)))");
    REQUIRE(std::get<Number>(res) == 3);
  }

  SECTION("separate") {
    REQUIRE_THROWS(eval_program("(let (x 1) x) (let (y 2) (+ x y))"));
  }
}