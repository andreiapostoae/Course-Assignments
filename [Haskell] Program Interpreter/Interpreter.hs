module Interpreter
  (
    -- * Types
    Prog,

    -- * Functions
    evalRaw,
    evalAdt,
  ) where

import Data.Either.Utils
-------------------------------------------------------------------------------
--------------------------------- The Expr ADT  -------------------------------
-------------------------------------------------------------------------------
data Expr = Add Expr Expr
          | Sub Expr Expr
          | Mult Expr Expr
          | Equal Expr Expr
          | Smaller Expr Expr
          | Symbol String
          | Value Int deriving (Show, Read)

-- [Optional] TODO Implement a parser for the Expr ADT.
--

-------------------------------------------------------------------------------
---------------------------------- The Prog ADT -------------------------------
-------------------------------------------------------------------------------
data Prog = Eq String Expr
          | Seq Prog Prog
          | If Expr Prog Prog
          | While Expr Prog
          | Return Expr deriving (Show, Read)

-- [Optional] TODO Implement a parser for the Prog ADT.
--

-- [Optional] TODO The *parse* function.  It receives a String - the program in
-- a "raw" format and it could return *Just* a program as an instance of the
-- *Prog* data type if no parsing errors are encountered, or Nothing if parsing
-- failed.
--
-- This is composed with evalAdt to yield the evalRaw function.
parse :: String -> Maybe Prog
parse = \_ -> Nothing

------------------------------------------------------------------------------------------------

noReturn = -100000
notFound = -100001
true = -100002
false = -100003

getValue :: [(String, Integer)] -> String -> Int
getValue (e:l) s = if ((fst e) == s)
						then (fromIntegral (snd e))
						else (getValue l s) 
getValue [] s = notFound


evalExpr :: Expr -> [(String, Integer)] -> ([(String, Integer)], Int)
evalExpr (Value a) list = (list, a)
evalExpr (Add expr1 expr2) list = if( snd(evalExpr expr1 list) /= notFound && snd (evalExpr expr2 list) /= notFound)
                                        then (list, (snd(evalExpr expr1 list)) + (snd(evalExpr expr2 list)))
                                        else (list, notFound)
evalExpr (Mult expr1 expr2) list = if( snd(evalExpr expr1 list) /= notFound && snd (evalExpr expr2 list) /= notFound)
                                        then (list, (snd(evalExpr expr1 list)) * (snd(evalExpr expr2 list)))
                                        else (list, notFound)
evalExpr (Sub expr1 expr2) list = if( snd(evalExpr expr1 list) /= notFound && snd (evalExpr expr2 list) /= notFound)
                                        then (list, (snd(evalExpr expr1 list)) - (snd(evalExpr expr2 list)))
                                        else (list, notFound)
evalExpr (Equal expr1 expr2) list = if( snd(evalExpr expr1 list) /= notFound && snd (evalExpr expr2 list) /= notFound)
                                        then 
                                            if (snd(evalExpr expr1 list)) == (snd(evalExpr expr2 list))
                                              then (list, true)
                                              else (list, false)
                                        else (list, notFound)
evalExpr (Smaller expr1 expr2) list = if( snd(evalExpr expr1 list) /= notFound && snd (evalExpr expr2 list) /= notFound)
                                        then 
                                            if (snd(evalExpr expr1 list)) < (snd(evalExpr expr2 list))
                                              then (list, true)
                                              else (list, false)
                                        else (list, notFound)
evalExpr (Symbol string) list = (list, getValue(list) string)

evalAux :: Prog -> [(String, Integer)] -> ([(String, Integer)], Int)
evalAux (Eq string expr) list = if(snd(evalExpr expr list)) == notFound
                                    then (list, notFound)
                                    else ((string, toInteger (snd(evalExpr expr list)) ):list, noReturn)

evalAux (Seq prog1 prog2) list = if(snd(evalAux prog1 list) == notFound || snd (evalAux prog2 (fst(evalAux prog1 list))) == notFound)
                                    then (list, notFound)
                                    else ((fst (evalAux prog2 (fst (evalAux prog1 list)))), (snd (evalAux prog2 (fst (evalAux prog1 list)))))

evalAux (If expr prog1 prog2) list = if(snd(evalExpr expr list)) == notFound
                                          then (list, notFound)
                                          else if(snd(evalExpr expr list)) == true
                                                  then evalAux prog1 list
                                                  else evalAux prog2 list

evalAux (While expr prog) list = if(snd(evalExpr expr list)) == notFound
                                      then (list, notFound)
                                      else if(snd(evalExpr expr list)) == true
                                              then if(snd(evalAux prog list)) /= notFound
                                                    then evalAux (While expr prog) (fst (evalAux prog list))
                                                    else (list, notFound)
                                              else (list, noReturn)

evalAux (Return expr) list = if(snd(evalExpr expr list)) == notFound
                                then (list, notFound)
                                else (list, (snd(evalExpr expr list)))


evalAdt :: Prog -> Either String Int
evalAdt (Eq string expr) = if snd (evalAux (Eq string expr) []) == notFound
                            then Left "Uninitialized variable"
                            else if snd (evalAux (Eq string expr) []) == noReturn
                                    then Left "Missing return"
                                    else Right (snd (evalAux (Eq string expr) []))
evalAdt (Seq prog1 prog2) = if snd (evalAux (Seq prog1 prog2) []) == notFound
                            then Left "Uninitialized variable"
                            else if snd (evalAux (Seq prog1 prog2) []) == noReturn
                                    then Left "Missing return"
                                    else Right(snd (evalAux (Seq prog1 prog2) []))
evalAdt (If expr prog1 prog2) = if snd (evalAux (If expr prog1 prog2) []) == notFound
                            then Left "Uninitialized variable"
                            else if snd (evalAux (If expr prog1 prog2) []) == noReturn
                                    then Left "Missing return"
                                    else Right(snd (evalAux (If expr prog1 prog2) []))
evalAdt (Return expr) = if snd (evalAux (Return expr) []) == notFound
                            then Left "Uninitialized variable"
                            else if snd (evalAux (Return expr) []) == noReturn
                                    then Left "Missing return"
                                    else Right(snd (evalAux (Return expr) []))
evalAdt (While expr prog) = if snd (evalAux (While expr prog) []) == notFound
                            then Left "Uninitialized variable"
                            else if snd (evalAux (While expr prog) []) == noReturn
                                    then Left "Missing return"
                                    else Right(snd (evalAux (While expr prog) []))


-- The *evalRaw* function is already implemented, but it relies on the *parse*
-- function which you have to implement.
--
-- Of couse, you can change this definition.  Only its name and type are
-- important.
evalRaw :: String -> Either String Int
evalRaw rawProg = case parse rawProg of
                    Just prog -> evalAdt prog
                    Nothing   -> Left "Syntax error"