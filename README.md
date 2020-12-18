# FL_Practicum
Практическое задание по дисциплине Формальные языки.

Задача 8. По данному регулярному выражению в Польской записи над алфавитом {a,b,c,1,*,.,+}, символу x из множества {a, b,c} и натуральному k вывести минимальную длину слова с суффиксом x^k в языке, задаваемым регулярным выражением.

*Описание алгоритма*:
1. Вначале производится чтение входной строки и проверка корректности регулярного выражения. Как известно, чтение регулярного выражения в польской записи производится за O(n), где n - длина входной строки.
2. После того, как мы убедились, что выражение корректно, вызывается функция поиска длины наименьшего слова языка, имеющего префикс x^k.
Мы заводим стек, как и в случае чтения слова в пункте 1. Однако на этот раз в стеке для каждого текущего регулрного выражения мы храним массив nodes[0,..., k], где на i-й позиции стоит число, равное минимальной длине слова с суффиксом x^i в языке, задаваемом данным регулярным выражением. На каждом шаге чтения для считанной буквы m мы генерируем для однобуквенного языка массив и добавляем его в стек. Если же на очередном шаге мы считали операцию, ты мы генерируем node для нового регулярного выражения на основе предыдущих. Функции Update принимают два (или один в случае * ) массива и обновляют их в зависимости от соответствуюшей операции.
Сложность этого алгоритма - O(n * k^2), поскольку на каждом из n прочтений мы вызываем функцию пересчета, которая работает максимум за k^2 (для операции умножения и звездочки). Или за O(k) для опрерации +.

В программе есть пространство имен testing, где написаны функции тестирования. Основная функция, которая запускает все тесты,- это функция DoTesting, вызов ее по уvолчанию стоит в конце функции main. 
