{-# LANGUAGE ScopedTypeVariables #-}
module Main (main) where

import Control.Monad
import System.IO
import System.Environment
import Data.Functor
import Data.Function
import Data.List
import Numeric

type Instance = ([Double], Int)

findNearest :: [Instance] -> Instance -> Int
findNearest set inst = snd . head $ sort $ map (\x -> (euclidean inst x, snd x)) set

euclidean :: Instance -> Instance -> Double
euclidean (a,_) (b,_) = sqrt . sum $ zipWith (*) z z
    where z = zipWith (-) a b

knn :: [Instance] -> [Instance] -> [Int]
knn solution complement = map (findNearest solution) complement

corrects :: [Instance] -> [Int] -> Int
corrects set cat = foldl' count 0 $ zipWith (-) cat (map snd set)
    where count = (\acc x -> if (x == 0) then acc + 1 else acc)

loadData :: String -> IO [Instance]
loadData fileName = readFile fileName >>= return . lines >>= return . (map words) >>= return . toDouble >>= return . (map toInstance)
    where toDouble       = (map (map (\x -> read x :: Double)))
          toInstance xs  = (init xs, (fromInteger $ floor $ last xs) :: Int)

percReduc :: [a] -> [a] -> Double
percReduc a b = (fromIntegral lb) / (fromIntegral $ la + lb)
    where la = length a
          lb = length b

main = do
    solution   <- loadData "solution.txt"
    complement <- loadData "complement.txt"
    putStrLn $ ">>>> Aciertos = " ++  (show $ corrects complement $ knn solution complement)
    putStrLn $ "Tamaño de solucion = " ++ (show $ length solution)
    putStrLn $ "Tamaño de T = " ++ (show $ (length solution) + (length complement))
    putStrLn $ "Porcentaje de reduccion = " ++  (show $ percReduc solution complement)
